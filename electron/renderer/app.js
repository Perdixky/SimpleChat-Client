import { callNative, IS_TEST_MODE } from './core/native.js';
import { queryDOM } from './GUI/dom.js';
// Use the extracted UI manager module
import { createUI } from './GUI/uiManager.js';
import { createSessionApi } from './Logic/session.js';
import { createTestAPI } from './Logic/testApi.js';
    document.addEventListener('DOMContentLoaded', () => {
      // ================================================================
      // ======================== THEME MANAGER =========================
      // ================================================================
      (function initTheme() {
        const root = document.documentElement;
        const STORAGE_KEY = 'sc-theme';
        const toggleBtn = document.getElementById('theme-toggle');

        // Determine default: user selection > system preference > dark
        const stored = localStorage.getItem(STORAGE_KEY);
        const prefersDark = window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
        const initial = stored || (prefersDark ? 'dark' : 'light');
        root.setAttribute('data-theme', initial);
        updateToggleIcon(initial);

        // If no manual selection, follow system changes
        if (!stored && window.matchMedia) {
          const mq = window.matchMedia('(prefers-color-scheme: dark)');
          mq.addEventListener('change', (e) => {
            if (!localStorage.getItem(STORAGE_KEY)) {
              const next = e.matches ? 'dark' : 'light';
              root.setAttribute('data-theme', next);
              updateToggleIcon(next);
            }
          });
        }

        toggleBtn.addEventListener('click', () => {
          const current = root.getAttribute('data-theme') || 'dark';
          const next = current === 'dark' ? 'light' : 'dark';
          root.setAttribute('data-theme', next);
          localStorage.setItem(STORAGE_KEY, next);
          updateToggleIcon(next);
        });

        function updateToggleIcon(theme) {
          const icon = toggleBtn.querySelector('.iconify');
          if (!icon) return;
          // Sun icon on dark, moon on light
          icon.setAttribute('data-icon', theme === 'dark' ? 'mdi:white-balance-sunny' : 'mdi:weather-night');
        }
      })();

      // Accent Manager: allow users to pick a richer accent to avoid a monotone feel
      (function initAccent() {
        const root = document.documentElement;
        const STORAGE_KEY = 'sc-accent';
        const defaultAccent = 'blue';
        const saved = localStorage.getItem(STORAGE_KEY) || defaultAccent;
        root.setAttribute('data-accent', saved);

        function refreshIndicators(current) {
          document.querySelectorAll('.accent-dot').forEach(btn => {
            const isActive = btn.getAttribute('data-accent') === current;
            btn.setAttribute('aria-current', isActive ? 'true' : 'false');
          });
        }
        refreshIndicators(saved);

        document.getElementById('accent-switcher')?.addEventListener('click', (e) => {
          const btn = e.target.closest('.accent-dot');
          if (!btn) return;
          const value = btn.getAttribute('data-accent') || defaultAccent;
          root.setAttribute('data-accent', value);
          localStorage.setItem(STORAGE_KEY, value);
          refreshIndicators(value);
        });
      })();

      // =================================================================
      // ====================== NATIVE LOGGER SETUP ======================
      // =================================================================
      (function () {
        const LogLevel = {
          TRACE: 0,
          DEBUG: 1,
          INFO: 2,
          WARNING: 3,
          ERROR: 4,
          FATAL: 5
        };
        const originals = {
          log: console.log.bind(console),
          warn: console.warn.bind(console),
          error: console.error.bind(console),
          debug: console.debug.bind(console),
          trace: console.trace ? console.trace.bind(console) : console.log.bind(console)
        };

        function nativeLog(level, args) {
          const message = args.map(arg => {
            if (typeof arg === 'object') {
              try {
                return JSON.stringify(arg);
              } catch (e) {
                return 'Unserializable Object';
              }
            }
            return arg;
          }).join(' ');

          try {
            if (window.bridge && typeof window.bridge.log === 'function') {
              window.bridge.log(level, message);
            } else {
              originals.log(`[FALLBACK] ${message}`);
            }
          } catch (e) {
            originals.error("Native 'log' function call failed.", e);
            originals.log(`[FALLBACK] ${message}`);
          }
        }

        console.trace = (...args) => nativeLog(LogLevel.TRACE, args);
        console.debug = (...args) => nativeLog(LogLevel.DEBUG, args);
        console.log = (...args) => nativeLog(LogLevel.INFO, args);
        console.warn = (...args) => nativeLog(LogLevel.WARNING, args);
        console.error = (...args) => nativeLog(LogLevel.ERROR, args);
        console.fatal = (...args) => nativeLog(LogLevel.FATAL, args);

        window.addEventListener('error', (event) => {
          console.error(`[GLOBAL ERROR] ${event.message} at ${event.filename}:${event.lineno}:${event.colno}`);
          console.error(`[GLOBAL ERROR] Stack trace:`, event.error);
        });

        window.addEventListener('unhandledrejection', (event) => {
          console.error('[GLOBAL PROMISE] Unhandled promise rejection:', event.reason);
          if (event.reason && event.reason.stack) {
            console.error('[GLOBAL PROMISE] Stack trace:', event.reason.stack);
          }
        });

        // Network/connectivity logging
        window.addEventListener('online', () => {
          console.log('[NETWORK] Connection restored - back online');
        });

        window.addEventListener('offline', () => {
          console.warn('[NETWORK] Connection lost - gone offline');
        });

        // Performance logging
        window.addEventListener('load', () => {
          console.debug(`[PERF] Page fully loaded in ${performance.now().toFixed(2)}ms`);
        });

        console.log("[LOGGER] Unified logger initialized with enhanced debugging");
      })();

      // =================================================================
      // ===================== DOM & STATE MANAGEMENT ====================
      // =================================================================
      const dom = queryDOM();
;

      let state = {
        currentUserId: null,
        conversationsCache: [],
        conversationMembersCache: new Map(),
        debounceTimer: null,
        selectedFriendId: null,
      }
      const ui = createUI(dom, state);
      let api = IS_TEST_MODE ? createTestAPI({ ui, dom, state }) : createSessionApi({ callNative, ui, dom, state });
      try { ui.bindApi(api); } catch {}
      if (IS_TEST_MODE) { try { ui.showMessage(dom.messageBox, 'Running in TEST MODE (no native bindings detected)', 'success'); } catch {} }

;

      // =================================================================
      // ========================= UI UTILITIES ==========================
      // =================================================================
      // Lightweight hashing for deterministic colors
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

      // =================================================================
      // ===================== NATIVE C++ BINDINGS =======================
      // =================================================================
      

      // =================================================================
      // ======================== UI MANAGER =============================
      // =================================================================
      ;

      // =================================================================
      // ======================== PAGE TRANSITIONS =======================
      // =================================================================
      ;

      // =================================================================
      // ===================== TEST MODE (DECOUPLED) ====================
      // =================================================================
      

      // =================================================================
      // =========================== API CALLS ===========================
      // =================================================================
      ;

      // --- Activate Test Mode if no native 'log' binding is present ---
      if (IS_TEST_MODE) {
        try { ui.showMessage(dom.messageBox, 'Running in TEST MODE (no native bindings detected)', 'success'); } catch {}
      }

      // =================================================================
      // ======================== EVENT LISTENERS ========================
      // =================================================================
      dom.authForm.addEventListener('submit', (e) => {
        e.preventDefault();
        console.debug("[EVENT] Login form submitted");

        const username = document.getElementById('username').value.trim();
        const password = document.getElementById('password').value;
        console.debug(`[EVENT] Login attempt - username: ${username}, password: ${password ? '[MASKED]' : '[EMPTY]'}`);

        if (username && password) {
          api.SignIn(username, password);
        } else {
          console.warn("[EVENT] Login form validation failed - missing fields");
          ui.showMessage(dom.messageBox, 'Username and Password are required.', 'error');
          ui.shake(dom.loginForm);
        }
      });

      dom.registerForm.addEventListener('submit', (e) => {
        e.preventDefault();
        console.debug("[EVENT] Register form submitted");

        const username = document.getElementById('register-username').value.trim();
        const invitationCode = document.getElementById('invitation-code').value.trim();
        const email = document.getElementById('register-email').value.trim();
        const verificationCode = document.getElementById('verification-code').value.trim();
        const password = document.getElementById('register-password').value;

        console.debug(`[EVENT] Register data - username: ${username}, email: ${email}, invitationCode: ${invitationCode}, verificationCode: ${verificationCode}, password: ${password ? '[MASKED]' : '[EMPTY]'}`);

        if (!username || !invitationCode || !email || !verificationCode || !password) {
          console.warn("[EVENT] Register form validation failed - missing required fields");
          ui.showMessage(dom.registerMessageBox, 'All fields are required.', 'error');
          return;
        }
        api.SignUp(username, invitationCode, email, verificationCode, password);
      });

      dom.getCodeButton.addEventListener('click', () => {
        console.debug("[EVENT] Get verification code button clicked");

        const emailInput = document.getElementById('register-email');
        const email = emailInput.value.trim();
        console.debug(`[EVENT] Requesting verification code for email: ${email}`);

        if (!emailInput.checkValidity() || !email) {
          console.warn(`[EVENT] Email validation failed: ${email}`);
          ui.showMessage(dom.registerMessageBox, 'Please enter a valid email address.', 'error');
          emailInput.reportValidity();
          return;
        }
        api.getVerificationCode(email);
      });

      dom.showRegisterLink.addEventListener('click', (e) => {
        e.preventDefault();
        console.debug("[EVENT] Show register form link clicked");
        ui.showRegisterForm();
      });

      dom.showLoginLink.addEventListener('click', (e) => {
        e.preventDefault();
        console.debug("[EVENT] Show login form link clicked");
        ui.showLoginForm();
      });

      dom.userList.addEventListener('click', (e) => {
        const userItem = e.target.closest('.user-item');
        if (userItem) {
          const conversationId = Number(userItem.dataset.conversationId);
          console.debug(`[EVENT] User item clicked - conversationId: ${conversationId}`);

          // Update UI state
          document.querySelectorAll('.user-item').forEach(item => {
            const wasActive = item.classList.contains('active');
            item.classList.remove('active');
            if (wasActive && item !== userItem) {
              console.debug(`[UI] Deactivated conversation: ${item.dataset.conversationId}`);
            }
          });
          userItem.classList.add('active');
          console.debug(`[UI] Activated conversation: ${conversationId}`);

          api.GetMessageHistory(conversationId);
        }
      });

      // --- Logout Button Listener ---
      const logoutBtn = document.getElementById('logout-btn');
      if (logoutBtn) {
        logoutBtn.addEventListener('click', () => {
          console.debug("[EVENT] Logout button clicked");
          ui.showLoginPage();
        });
      }

      // --- Add Friend Modal Listeners ---
      dom.addFriendBtn.addEventListener('click', () => {
        ui.toggleModal(dom.addFriendModal, true);
        dom.friendSearchResults.innerHTML = ''; // Clear previous results
      });

      dom.addFriendModalCloseBtn.addEventListener('click', () => {
        ui.toggleModal(dom.addFriendModal, false);
      });

      dom.addFriendForm.addEventListener('submit', (e) => {
        e.preventDefault();
        if (state.selectedFriendId) {
          api.AddFriend(state.selectedFriendId);
          state.selectedFriendId = null;
          dom.addFriendUsernameInput.value = '';
        } else {
          ui.showMessage(dom.addFriendMessage, 'Please search for a user and select one from the list.', 'error');
        }
      });

      dom.addFriendUsernameInput.addEventListener('input', (e) => {
        clearTimeout(state.debounceTimer);
        const query = e.target.value.trim();
        // Reset selection when typing new query
        state.selectedFriendId = null;
        dom.friendSearchResults.querySelectorAll('.friend-search-item.selected').forEach(n => n.classList.remove('selected'));
        if (query.length > 0) {
          state.debounceTimer = setTimeout(() => {
            api.FindUsers(query);
          }, 500);
        } else {
          dom.friendSearchResults.innerHTML = ''; // Clear if input is empty
        }
      });

      dom.friendSearchResults.addEventListener('click', (e) => {
        const selectedItem = e.target.closest('.friend-search-item');
        if (selectedItem) {
          const username = selectedItem.dataset.username;
          const userId = Number(selectedItem.dataset.userId);
          dom.addFriendUsernameInput.value = username;
          state.selectedFriendId = userId;
          // Highlight selected and keep the list visible
          dom.friendSearchResults.querySelectorAll('.friend-search-item.selected').forEach(n => n.classList.remove('selected'));
          selectedItem.classList.add('selected');
          dom.addFriendUsernameInput.parentElement.classList.add('has-content');
        }
      });


      // =================================================================
      // ====================== INITIALIZATION ===========================
      // =================================================================

      // Add initial logging
      console.debug("[INIT] Starting application initialization");
      console.debug(`[INIT] User agent: ${navigator.userAgent}`);
      console.debug(`[INIT] Screen size: ${screen.width}x${screen.height}`);
      console.debug(`[INIT] Viewport size: ${window.innerWidth}x${window.innerHeight}`);

      particlesJS('particles-js', {
        particles: {
          number: {
            value: 60,
            density: {
              enable: true,
              value_area: 800
            }
          },
          color: {
            value: '#ffffff'
          },
          shape: {
            type: 'circle'
          },
          opacity: {
            value: 0.5,
            random: true
          },
          size: {
            value: 3,
            random: true
          },
          line_linked: {
            enable: true,
            distance: 150,
            color: '#ffffff',
            opacity: 0.2,
            width: 1
          },
          move: {
            enable: true,
            speed: 2,
            direction: 'none',
            random: true,
            straight: false,
            out_mode: 'out'
          }
        },
        interactivity: {
          detect_on: 'canvas',
          events: {
            onhover: {
              enable: true,
              mode: 'grab'
            },
            onclick: {
              enable: true,
              mode: 'push'
            },
            resize: true
          },
          modes: {
            grab: {
              distance: 140,
              line_opacity: 0.5
            },
            push: {
              particles_nb: 4
            }
          }
        },
        retina_detect: true
      });

      ui.updateInputLabels();

      // Native push events subscription (server -> UI)
      if (window.bridge && typeof window.bridge.onEvent === 'function') {
        const unsubscribe = window.bridge.onEvent((evt) => {
          try {
            console.debug('[PUSH] native event:', evt);
            // Minimal demo: if event.type === 'MessageReceived', try refresh current history
            if (evt && evt.type && evt.type.toLowerCase().includes('message')) {
              // naive refresh; in future, append incrementally
              try {
                // Match the actual list item class and dataset used by UI renderer
                const active = document.querySelector('.user-item.active');
                if (active) {
                  const id = Number(active.dataset.conversationId);
                  if (!Number.isNaN(id) && typeof api?.GetMessageHistory === 'function') {
                    api.GetMessageHistory(id);
                  }
                }
              } catch {}
            }
          } catch (e) {
            console.warn('[PUSH] failed to process event', e);
          }
        });
        // Optional: store unsubscribe somewhere if needed
        window.__nativeUnsub = unsubscribe;
      }

      // Final initialization logging
      console.debug("[INIT] Application initialization complete");
      console.debug("[INIT] DOM ready, event listeners attached, particles initialized");

    });
