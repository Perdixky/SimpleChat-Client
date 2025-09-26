// Query and cache all DOM elements used by the renderer UI
export function queryDOM() {
  return {
    loginForm: document.getElementById('login-form'),
    loginFormContent: document.getElementById('login-form-content'),
    registerFormContent: document.getElementById('register-form-content'),
    authForm: document.getElementById('auth-form'),
    loginButton: document.getElementById('login-button'),
    messageBox: document.getElementById('message'),
    registerForm: document.getElementById('register-form'),
    registerButton: document.getElementById('register-button'),
    registerMessageBox: document.getElementById('register-message'),
    getCodeButton: document.getElementById('get-code-btn'),
    showRegisterLink: document.getElementById('show-register-link'),
    showLoginLink: document.getElementById('show-login-link'),
    userList: document.getElementById('user-list'),
    messageList: document.getElementById('message-list'),
    chatHeader: document.getElementById('chat-header'),
    messageForm: document.getElementById('message-form'),
    addFriendModal: document.getElementById('add-friend-modal'),
    addFriendBtn: document.getElementById('add-friend-btn'),
    addFriendModalCloseBtn: document.getElementById('add-friend-modal-close-btn'),
    addFriendForm: document.getElementById('add-friend-form'),
    addFriendUsernameInput: document.getElementById('add-friend-username'),
    friendSearchResults: document.getElementById('friend-search-results'),
    addFriendSubmitBtn: document.getElementById('add-friend-submit-btn'),
    addFriendMessage: document.getElementById('add-friend-message'),
  };
}
