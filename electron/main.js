// Electron main process: spawns C++ ChatDaemon and bridges IPC to renderer
const { app, BrowserWindow, ipcMain, Menu } = require('electron');
const path = require('path');
const { spawn } = require('child_process');

let daemon = null;
let nextId = 1;
const pending = new Map(); // rpc_id -> {resolve,reject}

function daemonPath() {
  const root = path.resolve(__dirname, '..');
  const exe = process.platform === 'win32' ? 'ChatDaemon.exe' : 'ChatDaemon';
  return path.join(root, 'bin', exe);
}

function startDaemon() {
  const exe = daemonPath();
  daemon = spawn(exe, [], {
    cwd: path.dirname(exe),
    stdio: ['pipe', 'pipe', 'pipe']
  });

  daemon.on('error', (err) => {
    console.error('[daemon] spawn error:', err);
  });

  daemon.stderr.on('data', (buf) => {
    process.stderr.write(Buffer.from(`[daemon] ${buf.toString()}`));
  });

  let buffer = '';
  daemon.stdout.on('data', (data) => {
    buffer += data.toString('utf8');
    let idx;
    while ((idx = buffer.indexOf('\n')) >= 0) {
      const line = buffer.slice(0, idx);
      buffer = buffer.slice(idx + 1);
      if (!line.trim()) continue;
      try {
        const msg = JSON.parse(line);
        const slot = pending.get(msg.rpc_id);
        if (!slot) continue;
        pending.delete(msg.rpc_id);
        if (msg.ok) slot.resolve(msg.result);
        else slot.reject(new Error(msg.error || 'Unknown error'));
      } catch (e) {
        console.error('[daemon] invalid json:', line, e);
      }
    }
  });

  daemon.on('exit', (code, signal) => {
    console.warn(`[daemon] exited code=${code} signal=${signal}`);
    // Reject all pending
    for (const [, slot] of pending) slot.reject(new Error('Daemon exited'));
    pending.clear();
  });
}

function call(method, ...params) {
  if (!daemon) throw new Error('Daemon not running');
  const rpc_id = nextId++;
  const payload = { rpc_id, method, params };
  return new Promise((resolve, reject) => {
    pending.set(rpc_id, { resolve, reject });
    daemon.stdin.write(JSON.stringify(payload) + '\n');
  });
}

function createWindow() {
  const win = new BrowserWindow({
    width: 1024,
    height: 768,
    autoHideMenuBar: true,
    webPreferences: {
      contextIsolation: true,
      preload: path.join(__dirname, 'preload.js')
    }
  });

  // Remove application menu and the visible menu bar
  try {
    Menu.setApplicationMenu(null);
    win.setMenuBarVisibility(false);
  } catch (e) { console.warn('[electron] failed to remove menu bar:', e); }

  const indexPath = path.resolve(__dirname, '..', 'include', 'GUI', 'index.html');
  win.loadFile(indexPath);
}

app.whenReady().then(() => {
  // Minimal default: do not force Wayland or GPU flags; user can provide env/flags
  startDaemon();

  ipcMain.handle('rpc', async (_evt, method, ...args) => {
    return call(method, ...args);
  });

  ipcMain.handle('log', (_evt, level, message) => {
    const levels = ['trace', 'debug', 'info', 'warn', 'error', 'fatal'];
    const tag = levels[level] || 'info';
    console.log(`[renderer:${tag}]`, message);
    return true;
  });

  createWindow();

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});

app.on('quit', () => {
  try {
    if (daemon && daemon.stdin.writable) {
      daemon.stdin.write(JSON.stringify({ rpc_id: 0, method: 'Quit' }) + '\n');
    }
  } catch {}
  if (daemon) daemon.kill();
});
