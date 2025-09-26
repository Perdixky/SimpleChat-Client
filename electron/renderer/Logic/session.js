// High-level API wrapper: all business calls that go to native addon
// Dependency injected for testability and to decouple UI from transport.
export function createSessionApi({ callNative, ui, dom, state }) {
  return {
    async SignIn(username, password) {
      console.debug(`[API] SignIn attempt for user: ${username}`);
      ui.setLoading(dom.loginButton, true);
      ui.showMessage(dom.messageBox, '');
      try {
        const result = await callNative('SignIn', username, password);
        console.debug(`[API] SignIn parsed result:`, result);
        if (result.status === 'Success') {
          console.log(`[API] Login successful for user: ${username}, user_id: ${result.user_id}`);
          ui.showMessage(dom.messageBox, result.message || 'Login success', 'success');
          state.currentUserId = result.user_id;
          setTimeout(() => ui.showChatPage(), 500);
        } else {
          ui.showMessage(dom.messageBox, result.message || 'Login failed', 'error');
          ui.shake(dom.loginForm);
          ui.setLoading(dom.loginButton, false);
        }
      } catch (err) {
        console.error(`[API] SignIn exception for user ${username}:`, err);
        ui.showMessage(dom.messageBox, err.message || 'An unexpected error occurred.', 'error');
        ui.shake(dom.loginForm);
        ui.setLoading(dom.loginButton, false);
      }
    },

    async getVerificationCode(email) {
      console.debug(`[API] Getting verification code for email: ${email}`);
      dom.getCodeButton.disabled = true;
      dom.getCodeButton.innerHTML = '<span class="iconify" data-icon="line-md:loading-loop"></span>Sending...';
      try {
        const result = await callNative('getVerificationCode', email);
        if (result.success) {
          ui.showMessage(dom.registerMessageBox, 'Verification code sent to your email.', 'success');
          let countdown = 60;
          const interval = setInterval(() => {
            if (countdown > 0) { dom.getCodeButton.textContent = `${countdown}s`; countdown--; }
            else { clearInterval(interval); dom.getCodeButton.innerHTML = '<span class="iconify" data-icon="mdi:email-fast"></span>Get Code'; dom.getCodeButton.disabled = false; }
          }, 1000);
        } else {
          ui.showMessage(dom.registerMessageBox, result.message || 'Failed to send code.', 'error');
          dom.getCodeButton.disabled = false;
          dom.getCodeButton.innerHTML = '<span class="iconify" data-icon="mdi:email-fast"></span>Get Code';
        }
      } catch (err) {
        ui.showMessage(dom.registerMessageBox, err.message || 'An unexpected error occurred.', 'error');
        dom.getCodeButton.disabled = false;
        dom.getCodeButton.innerHTML = '<span class="iconify" data-icon="mdi:email-fast"></span>Get Code';
      }
    },

    async SignUp(username, invitationCode, email, verificationCode, password) {
      ui.setLoading(dom.registerButton, true);
      ui.showMessage(dom.registerMessageBox, '');
      try {
        const result = await callNative('SignUp', username, invitationCode, email, verificationCode, password);
        if (result.success) {
          ui.showMessage(dom.registerMessageBox, result.message || 'Registration successful!', 'success');
          setTimeout(() => ui.showLoginForm(), 1500);
        } else {
          ui.showMessage(dom.registerMessageBox, result.message || 'Registration failed.', 'error');
        }
      } catch (err) {
        ui.showMessage(dom.registerMessageBox, err.message || 'An unexpected error occurred.', 'error');
      } finally {
        ui.setLoading(dom.registerButton, false);
      }
    },

    async GetConversationList() {
      console.debug(`[API] Getting conversation list`);
      try {
        const response = await callNative('GetConversationList');
        const conversations = response.conversations || [];
        state.conversationsCache = conversations;
        ui.renderConversations(conversations);
      } catch (error) {
        console.error(`[API] Failed to get conversations:`, error);
      }
    },

    async GetMessageHistory(conversationId) {
      console.debug(`[API] Getting message history for conversation: ${conversationId}`);
      try {
        const conversation = state.conversationsCache.find(c => c.id === conversationId);
        if (!conversation) {
          console.error(`[API] Conversation not found in cache: ${conversationId}`);
          return;
        }
        let members = state.conversationMembersCache.get(conversationId);
        if (!members) {
          members = await this.GetConversationMemberList(conversationId);
          state.conversationMembersCache.set(conversationId, members);
        }
        const result = await callNative('GetMessageHistory', conversationId);
        const messages = result.messages || [];
        ui.renderMessages(messages, conversation, members);
      } catch (error) {
        console.error(`[API] Failed to get messages for ${conversationId}:`, error);
      }
    },

    async GetConversationMemberList(conversationId) {
      const result = await callNative('GetConversationMemberList', conversationId);
      return result.members || [];
    },

    async AddFriend(userId) {
      ui.setLoading(dom.addFriendSubmitBtn, true);
      ui.showMessage(dom.addFriendMessage, '');
      try {
        const result = await callNative('AddFriend', userId);
        if (result.success) {
          ui.showMessage(dom.addFriendMessage, result.message || 'Friend request sent!', 'success');
          setTimeout(() => { ui.toggleModal(dom.addFriendModal, false); }, 1500);
        } else {
          ui.showMessage(dom.addFriendMessage, result.message || 'Failed to send request.', 'error');
        }
      } catch (err) {
        ui.showMessage(dom.addFriendMessage, err.message || 'An unexpected error occurred.', 'error');
      } finally {
        ui.setLoading(dom.addFriendSubmitBtn, false);
      }
    },

    async FindUsers(query) {
      try {
        const result = await callNative('FindUsers', query);
        ui.renderUserSearchResults(result.users || []);
      } catch (err) {
        console.error('Failed to find user:', err);
        ui.renderUserSearchResults([]);
      }
    },
  };
}
