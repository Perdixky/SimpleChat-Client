// Preload: expose a single safe bridge into the main world
const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('bridge', {
  invoke: (name, ...args) => ipcRenderer.invoke('rpc', name, ...args),
  // Forward renderer PID so main can print unified [pid]
  log: (severity, message) => ipcRenderer.invoke('log', severity, message, process.pid),
  // Subscribe to native push events (broadcasts)
  onEvent: (handler) => {
    const channel = 'native:event';
    const listener = (_evt, payload) => handler && handler(payload);
    ipcRenderer.on(channel, listener);
    // Ensure main has a subscriber (no-op but keeps API explicit)
    try { ipcRenderer.send('events:subscribe'); } catch {}
    return () => ipcRenderer.removeListener(channel, listener);
  }
});
