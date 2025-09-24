// Preload: exposes safe APIs to the renderer matching the old webview bindings
const { contextBridge, ipcRenderer } = require('electron');

const methods = [
  'SignIn',
  'SignUp',
  'GetConversationList',
  'GetConversationMemberList',
  'GetMessageHistory',
  'AddFriend',
  'FindUsers',
  'Heartbeat',
  'Echo'
];

const api = {};
for (const m of methods) {
  api[m] = (...args) => ipcRenderer.invoke('rpc', m, ...args);
}

// Native logger used by index.html enhancements
api.log = (severity, message) => ipcRenderer.invoke('log', severity, message);

contextBridge.exposeInMainWorld('SignIn', api.SignIn);
contextBridge.exposeInMainWorld('SignUp', api.SignUp);
contextBridge.exposeInMainWorld('GetConversationList', api.GetConversationList);
contextBridge.exposeInMainWorld('GetConversationMemberList', api.GetConversationMemberList);
contextBridge.exposeInMainWorld('GetMessageHistory', api.GetMessageHistory);
contextBridge.exposeInMainWorld('AddFriend', api.AddFriend);
contextBridge.exposeInMainWorld('FindUsers', api.FindUsers);
contextBridge.exposeInMainWorld('log', api.log);

