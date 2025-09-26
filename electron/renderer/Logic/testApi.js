// Test-mode API mirrors the shape of Session API but returns mock data.
export function createTestAPI({ ui, dom, state }) {
  console.warn('[TEST] Native bindings not found. Entering TEST MODE with mock data.');
  const me = { id: 1000, name: 'You', email: 'you@example.com', avatar: `https://i.pravatar.cc/100?u=1000` };
  const users = [
    me,
    { id: 1, name: 'Alice Johnson', email: 'alice@example.com', avatar: `https://i.pravatar.cc/100?u=1` },
    { id: 2, name: 'Bob Smith', email: 'bob@example.com', avatar: `https://i.pravatar.cc/100?u=2` },
    { id: 3, name: 'Clara Lee', email: 'clara@example.com', avatar: `https://i.pravatar.cc/100?u=3` },
    { id: 4, name: 'David Wu', email: 'david@example.com', avatar: `https://i.pravatar.cc/100?u=4` },
    { id: 5, name: 'Eva Green', email: 'eva@example.com', avatar: `https://i.pravatar.cc/100?u=5` },
  ];
  const conversations = [
    { id: 201, name: 'Alice Johnson', title: 'Alice Johnson', avatar: users[1].avatar, lastMessage: 'Let\'s meet tomorrow at 10.' },
    { id: 202, name: 'Bob Smith', title: 'Bob Smith', avatar: users[2].avatar, lastMessage: 'Sending the report now.' },
    { id: 203, name: 'Core Team', title: 'Core Team', avatar: `https://i.pravatar.cc/100?u=team`, lastMessage: 'Standup in 5 minutes.' },
  ];
  const convMembers = new Map([
    [201, [me, users[1]]],
    [202, [me, users[2]]],
    [203, [me, users[1], users[2], users[3]]],
  ]);
  const convMessages = new Map([
    [201, [
      { sender_id: 1, text: 'Hey there! How\'s it going?' },
      { sender_id: 1000, text: 'All good! Working on the UI revamp.' },
      { sender_id: 1, text: 'Looks great already.' },
    ]],
    [202, [
      { sender_id: 2, text: 'I\'ll send the draft shortly.' },
      { sender_id: 1000, text: 'Awesome, thanks!' },
    ]],
    [203, [
      { sender_id: 3, text: 'Morning folks!' },
      { sender_id: 1, text: 'Morning!' },
      { sender_id: 1000, text: 'GM! Ready for standup.' },
    ]],
  ]);

  const delay = (ms) => new Promise(r => setTimeout(r, ms));

  try {
    const badge = document.createElement('div');
    badge.style.cssText = 'position:fixed;left:16px;top:16px;z-index:3;padding:6px 10px;border-radius:10px;background:rgba(255,255,255,.14);backdrop-filter:blur(8px);border:1px solid rgba(255,255,255,.2);font:600 12px/1 Poppins, sans-serif;color:var(--text-primary);box-shadow:0 6px 18px rgba(0,0,0,.35)';
    badge.textContent = 'TEST MODE';
    document.body.appendChild(badge);
  } catch {}

  return {
    async SignIn(username, password) {
      ui.setLoading(dom.loginButton, true);
      ui.showMessage(dom.messageBox, '');
      await delay(480);
      state.currentUserId = me.id;
      ui.showMessage(dom.messageBox, 'Test mode login successful', 'success');
      setTimeout(() => ui.showChatPage(), 400);
    },

    async getVerificationCode(email) {
      dom.getCodeButton.disabled = true;
      dom.getCodeButton.innerHTML = '<span class="iconify" data-icon="line-md:loading-loop"></span>Sending...';
      await delay(800);
      ui.showMessage(dom.registerMessageBox, `Verification code sent to ${email || 'your email'}.`, 'success');
      let countdown = 8; const interval = setInterval(() => {
        if (countdown > 0) { dom.getCodeButton.textContent = `${countdown--}s`; }
        else { clearInterval(interval); dom.getCodeButton.innerHTML = '<span class="iconify" data-icon="mdi:email-fast"></span>Get Code'; dom.getCodeButton.disabled = false; }
      }, 1000);
      return { success: true };
    },

    async SignUp(username, invitationCode, email, verificationCode, password) {
      ui.setLoading(dom.registerButton, true);
      await delay(600);
      ui.showMessage(dom.registerMessageBox, 'Registration successful (test mode)', 'success');
      ui.setLoading(dom.registerButton, false);
      setTimeout(() => ui.showLoginForm(), 1000);
      return { success: true };
    },

    async GetConversationList() {
      await delay(360);
      state.conversationsCache = conversations.slice();
      ui.renderConversations(state.conversationsCache);
      return { conversations: state.conversationsCache };
    },

    async GetConversationMemberList(conversationId) {
      await delay(200);
      return convMembers.get(conversationId) || [];
    },

    async GetMessageHistory(conversationId) {
      await delay(320);
      const conversation = state.conversationsCache.find(c => c.id === conversationId) || conversations[0];
      let members = state.conversationMembersCache.get(conversationId);
      if (!members) {
        members = convMembers.get(conversationId) || [me];
        state.conversationMembersCache.set(conversationId, members);
      }
      const messages = (convMessages.get(conversationId) || []).slice();
      ui.renderMessages(messages, conversation, members);
      return { messages };
    },

    async AddFriend(userId) {
      await delay(500);
      ui.showMessage(dom.addFriendMessage, 'Request sent (test mode)', 'success');
      setTimeout(() => ui.toggleModal(dom.addFriendModal, false), 1000);
      return { success: true, message: 'ok' };
    },

    async FindUsers(query) {
      await delay(200);
      const q = (query || '').toLowerCase();
      const result = users.filter(u => u.id !== me.id && (u.name.toLowerCase().includes(q) || u.email.toLowerCase().includes(q)));
      ui.renderUserSearchResults(result);
      return { users: result };
    },
  };
}
