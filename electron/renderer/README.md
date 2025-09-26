Electron renderer structure (WIP extraction)

- core/
  - native.js: preload bridge helpers (IS_TEST_MODE, callNative)
- styles/
  - app.css: all UI styles extracted from index.html; split further later
- app.js: bootstraps UI, event wiring; to be refactored into modules below
- GUI/ (planned)
  - uiManager.js: view logic (renderConversations, renderMessages, modals)
  - dom.js: element querying and refs
- Logic/ (planned)
  - session.js: high-level API (SignIn, GetConversationList, ...)
  - testApi.js: local test-mode implementation
- Network/ (planned)
  - connection.js: WebSocket client, routing, heartbeats
- Utils/ (planned)
  - logger.js: renderer->main logging bridge
  - format.js: helpers for timestamps, ids, avatars

Notes
- The first step has been completed: CSS + JS moved out of index.html.
- Next, peel UI/API/state into the planned modules incrementally.

