// UI Manager extracted from app.js
// Responsibilities: visual helpers, rendering lists, page transitions

// Local helpers to generate deterministic fallback avatars (SVG data URI)
function hash32(str) {
  let h = 2166136261 >>> 0; // FNV-1a basis
  for (let i = 0; i < str.length; i++) {
    h ^= str.charCodeAt(i);
    h = Math.imul(h, 16777619);
  }
  return h >>> 0;
}

function initialsFrom(nameOrEmail) {
  const s = String(nameOrEmail || '').trim();
  if (!s) return 'U';
  const parts = s.replace(/[_.-]/g, ' ').split(/\s+/).filter(Boolean);
  const first = parts[0] ? parts[0][0] : 'U';
  const second = parts.length > 1 ? parts[1][0] : '';
  return (first + second).toUpperCase();
}

function gradientFor(seed) {
  const palettes = [
    ['#4f46e5', '#22d3ee'], // indigo → cyan
    ['#7c3aed', '#f472b6'], // violet → pink
    ['#0ea5e9', '#34d399'], // sky → green
    ['#f59e0b', '#ef4444'], // amber → red
    ['#6366f1', '#a855f7'], // indigo → purple
  ];
  const h = hash32(String(seed));
  return palettes[h % palettes.length];
}

function svgAvatarDataURI(seed, label, size = 64) {
  const [c1, c2] = gradientFor(seed);
  const text = initialsFrom(label);
  const id = 'g' + (hash32(String(seed + label)) % 1e9);
  const svg = `<?xml version="1.0" encoding="UTF-8"?>\n` +
    `<svg xmlns="http://www.w3.org/2000/svg" width="${size}" height="${size}" viewBox="0 0 ${size} ${size}">` +
    `<defs>` +
    `<linearGradient id="${id}" x1="0" y1="0" x2="1" y2="1">` +
    `<stop offset="0%" stop-color="${c1}"/>` +
    `<stop offset="100%" stop-color="${c2}"/>` +
    `</linearGradient>` +
    `</defs>` +
    `<rect width="100%" height="100%" rx="${Math.floor(size/2)}" fill="url(#${id})"/>` +
    `<text x="50%" y="54%" text-anchor="middle" dominant-baseline="middle" fill="white" font-family="Poppins, Arial, sans-serif" font-size="${Math.floor(size*0.42)}" font-weight="600">${text}</text>` +
    `</svg>`;
  return 'data:image/svg+xml;utf8,' + encodeURIComponent(svg);
}

export function createUI(dom, state) {
  let apiRef = null;

  const ui = {
    // Smoothly resize the login card to match target content height with a gentle bounce
    // Simplified: rely on natural layout; avoid measuring hidden content and
    // absolute off-screen hacks that can cause flicker or overflow on some
    // platforms. Keep bounce handled by callers if desired.
    _resizeCardTo(_targetEl) { return; },
    // Dependency injection to use API calls in page transitions
    bindApi(api) { apiRef = api; },

    setLoading(button, isLoading) {
      if (!button) return;
      button.classList.toggle('btn-loading', isLoading);
      // The loading spinner is now handled by CSS ::after pseudo-element
    },

    getAvatar(obj) {
      if (!obj) return svgAvatarDataURI('unknown', 'U', 64);
      const explicit = obj.avatar;
      if (explicit && typeof explicit === 'string' && explicit.trim().startsWith('data:')) return explicit;
      const seed = obj.id ?? (obj.name || obj.title || 'user');
      const label = obj.name || obj.title || 'User';
      return svgAvatarDataURI(String(seed), label, 64);
    },

    shake(element) {
      if (!element) return;
      element.classList.remove('shake');
      void element.offsetWidth; // force reflow to restart animation
      element.classList.add('shake');
      element.addEventListener('animationend', () => element.classList.remove('shake'), { once: true });
    },

    showMessage(element, text, type = 'info') {
      if (!element) return;
      element.textContent = text || '';
      element.className = 'mt-4 text-center text-sm min-h-[20px]';
      if (type === 'error') element.classList.add('message-error');
      if (type === 'success') element.classList.add('message-success');
    },

    renderConversations(conversations) {
      dom.userList.innerHTML = '';
      (conversations || []).forEach(convo => {
        const userItem = document.createElement('div');
        userItem.className = 'user-item';
        userItem.dataset.conversationId = convo.id;
        const avatarSrc = ui.getAvatar(convo);
        userItem.innerHTML = `
          <img src="${avatarSrc}" alt="User Avatar">
          <div class="user-info">
            <div class="name">${convo.title}</div>
            <div class="last-message">${convo.lastMessage || ''}</div>
          </div>
        `;
        dom.userList.appendChild(userItem);
      });
    },

    renderMessages(messages, conversation, members) {
      dom.messageList.innerHTML = '';
      const membersMap = new Map((members || []).map(m => [m.id, m]));

      const chAvatar = ui.getAvatar(conversation);
      dom.chatHeader.innerHTML = `
        <img src="${chAvatar}" alt="Active User Avatar">
        <div>
          <div class="name">${conversation.name}</div>
          <div class="status">Online</div>
        </div>
      `;

      (messages || []).forEach(msg => {
        const sender = membersMap.get(msg.sender_id);
        const isMine = msg.sender_id === state.currentUserId;
        const messageDiv = document.createElement('div');
        messageDiv.className = `message ${isMine ? 'my-message' : 'their-message'}`;
        let content = '';
        if (!isMine) {
          const avatarSrc = ui.getAvatar(sender || { id: 'unknown', name: '?' });
          content += `<img class="avatar" src="${avatarSrc}" alt="Avatar">`;
        }
        content += `<div class="message-content">${msg.text}</div>`;
        messageDiv.innerHTML = content;
        dom.messageList.appendChild(messageDiv);
      });
    },

    updateInputLabels() {
      // Simplified input handling - no need for complex label animations
      // Just ensure focus styles work properly
      document.querySelectorAll('.input-group input').forEach(input => {
        // Remove any legacy animation classes
        input.parentElement.classList.remove('has-content');
      });
    },

    toggleModal(modal, show) {
      if (!modal) return;
      modal.classList.toggle('modal-visible', show);
      if (show) {
        const form = modal.querySelector('form');
        if (form) form.reset();
        const messageEl = modal.querySelector('#add-friend-message');
        if (messageEl) ui.showMessage(messageEl, '');
      }
    },

    renderUserSearchResults(users) {
      dom.friendSearchResults.innerHTML = '';
      if (!users || users.length === 0) return;
      users.forEach(user => {
        const item = document.createElement('div');
        item.className = 'friend-search-item';
        item.dataset.username = user.name;
        item.dataset.userId = user.id;
        const avatarSrc = ui.getAvatar(user);
        item.innerHTML = `
          <img src="${avatarSrc}" alt="Avatar">
          <div class="info">
            <div class="name">${user.name}</div>
            <div class="email">${user.email}</div>
          </div>
        `;
        dom.friendSearchResults.appendChild(item);
      });
      if (state.selectedFriendId) {
        const selectedEl = dom.friendSearchResults.querySelector(`.friend-search-item[data-user-id="${state.selectedFriendId}"]`);
        if (selectedEl) selectedEl.classList.add('selected');
      }
    },

    // Page transitions (was pageManager)
    showChatPage() {
      const loginContainer = document.querySelector('.login-container');
      const chatApp = document.getElementById('chat-app');
      // graceful scale + fade
      loginContainer.classList.add('scale-fade-out');
      setTimeout(() => {
        loginContainer.classList.add('hidden');
        // fully remove from layout to avoid pushing chat below
        loginContainer.style.display = 'none';
        chatApp.classList.add('active');
        chatApp.classList.add('scale-fade-in');
        try { apiRef && apiRef.GetConversationList && apiRef.GetConversationList(); } catch {}
        // Enable legacy stylesheet for chat until chat UI is migrated to Tailwind
        try { document.getElementById('legacy-css')?.setAttribute('media','all'); } catch {}
      }, 180);
    },

    showLoginPage() {
      const loginContainer = document.getElementById('login-container');
      const chatApp = document.getElementById('chat-app');
      chatApp.classList.remove('active');
      loginContainer.classList.remove('login-hidden');
      setTimeout(() => {
        dom.loginFormContent.classList.remove('hidden');
        dom.registerFormContent.classList.add('hidden');
        ui.showMessage(dom.messageBox, '');
        ui.showMessage(dom.registerMessageBox, '');
      }, 120);
      // Disable legacy stylesheet on login screens
      try { document.getElementById('legacy-css')?.setAttribute('media','not all'); } catch {}
    },

    showRegisterForm() {
      const card = document.getElementById('login-form');
      if (card) { card.classList.add('animate-bounce-in'); setTimeout(() => card.classList.remove('animate-bounce-in'), 420); }
      setTimeout(() => {
        dom.loginFormContent.classList.add('hidden');
        dom.registerFormContent.classList.remove('hidden');
      }, 160);
    },

    showLoginForm() {
      const card = document.getElementById('login-form');
      if (card) { card.classList.add('animate-bounce-in'); setTimeout(() => card.classList.remove('animate-bounce-in'), 420); }
      setTimeout(() => {
        dom.registerFormContent.classList.add('hidden');
        dom.loginFormContent.classList.remove('hidden');
      }, 160);
    },
  };

  return ui;
}
