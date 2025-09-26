// Electron main process: bridges renderer IPC to native addon (Node-API)
const { app, BrowserWindow, ipcMain, Menu } = require('electron');
const path = require('path');
const fs = require('fs');
const { printUnified, normalizeLevel } = require('./common/logger');

// Load native addon built via xmake (preferred; node-gyp fallback removed)
let addon;
let session;
const guessXmakeAddon = () => {
  const plat = process.platform; // linux, win32, darwin
  const archNode = process.arch; // x64, arm64
  // Map node arch -> xmake arch folder
  const archXmake = archNode === 'x64' ? 'x86_64' : archNode;
  const mode = process.env.NODE_ENV === 'production' ? 'release' : 'debug';
  return [
    path.resolve(__dirname, '..', 'build', plat, archXmake, mode, 'native.node'),
    path.resolve(__dirname, '..', 'build', plat, archNode, mode, 'native.node')
  ];
};
const xmakePaths = [path.resolve(__dirname, 'native.node'), ...guessXmakeAddon()];
const packagedPath = path.join(process.resourcesPath || '', 'native.node');
if (process.resourcesPath && fs.existsSync(packagedPath)) {
  addon = require(packagedPath);
  printUnified('info', process.pid, `addon loaded native: ${packagedPath}`);
} else {
  const candidate = xmakePaths.find(p => fs.existsSync(p));
  if (!candidate) throw new Error('xmake native.node not found in candidates: ' + xmakePaths.join(', '));
  addon = require(candidate);
  printUnified('info', process.pid, `addon loaded native: ${candidate}`);
}
if (!addon || !addon.Session) throw new Error('Invalid native addon: missing Session class export');
session = new addon.Session();

const ALLOWED = new Set([
  'SignIn', 'SignUp', 'GetConversationList', 'GetConversationMemberList',
  'GetMessageHistory', 'AddFriend', 'FindUsers', 'Heartbeat', 'Echo',
  'getVerificationCode', 'GetVerificationCode'
]);

function call(method, ...params) {
  // New Session class: methods are PascalCase to match Logic::Request names
  if (method === 'getVerificationCode' || method === 'GetVerificationCode') {
    // Local stub: UI expects an immediate success here
    return Promise.resolve({ success: true, message: 'Verification code sent' });
  }
  const fn = session[method];
  if (typeof fn !== 'function') throw new Error(`Unknown method: ${method}`);
  return fn.call(session, ...params);
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
  } catch (e) { printUnified('warn', process.pid, `[electron] failed to remove menu bar: ${e && e.message ? e.message : e}`); }

  // Load the new React + shadcn/ui renderer built by Vite
  const indexPath = path.resolve(__dirname, 'ui', 'dist', 'index.html');
  win.loadFile(indexPath);
  // Helpful diagnostics if the renderer fails to load
  win.webContents.on('did-fail-load', (_e, code, desc, url) => {
    try { printUnified('error', process.pid, `[electron] did-fail-load code=${code} desc=${desc} url=${url}`); } catch {}
  });
  win.webContents.on('render-process-gone', (_e, details) => {
    try { printUnified('error', process.pid, `[electron] render-process-gone: ${JSON.stringify(details)}`); } catch {}
  });
  if (process.env.DEBUG_UI === '1') {
    try { win.webContents.openDevTools({ mode: 'detach' }); } catch {}
  }

  // Hook native push events -> forward to renderer
  try {
    session.OnEvent((evt) => {
      try {
        win.webContents.send('native:event', evt);
      } catch (e) {
        printUnified('warn', process.pid, `[electron] failed to forward native event: ${e && e.message ? e.message : e}`);
      }
    });
  } catch (e) {
    printUnified('warn', process.pid, `[electron] Session.OnEvent not available: ${e && e.message ? e.message : e}`);
  }
}

app.whenReady().then(() => {
  ipcMain.handle('rpc', async (_evt, method, ...args) => {
    if (!ALLOWED.has(method)) throw new Error(`Disallowed method: ${method}`);
    return call(method, ...args);
  });

  ipcMain.handle('log', (_evt, level, message, originPid) => {
    const tag = normalizeLevel(level);
    const pid = originPid || process.pid;
    printUnified(tag, pid, message);
    return true;
  });

  // Renderer can explicitly subscribe (no-op here since we forward globally)
  ipcMain.on('events:subscribe', (evt) => {
    evt.returnValue = true;
  });

  createWindow();

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});

function safeClean() {
  try {
    if (session && typeof session.Clean === 'function') {
      session.Clean();
      printUnified('info', process.pid, 'addon session.Clean called');
      return;
    }
    if (session && typeof session.Close === 'function') {
      session.Close();
      printUnified('info', process.pid, 'addon session.Close called');
      return;
    }
    if (addon && typeof addon.Clean === 'function') {
      addon.Clean();
      printUnified('info', process.pid, 'addon addon.Clean called');
      return;
    }
    if (addon && typeof addon.shutdown === 'function') {
      addon.shutdown();
      printUnified('info', process.pid, 'addon addon.shutdown called');
    }
  } catch (e) {
    printUnified('warn', process.pid, `[addon] cleanup failed: ${e && e.message ? e.message : e}`);
  }
}

app.on('before-quit', safeClean);
app.on('quit', safeClean);
process.on('exit', safeClean);
