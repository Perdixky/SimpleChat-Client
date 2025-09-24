# Repository Guidelines

## Project Structure & Module Organization
- `src/` – C++ application entry-point and runtime glue (e.g., `src/main.cpp`).
- `include/` – Header-only modules organized by domain:
  - `GUI/` – Desktop WebView UI (`GUI.hpp`, embedded `index.html`).
  - `Logic/` – Domain types and session API (`LogicType.hpp`, `Session.hpp`).
  - `Network/` – TLS/WebSocket connection, routing, senders.
  - `Utils/` and `Async/` – logging, formatting, and stdexec-based loop.
- `extra/` – Optional TUI components.
- `test/` – Local tools (e.g., `WebsocketEchoServer.py`) and `requirements.txt`.
- `security/` – Development certificates/keys (for local TLS only).

## Build, Test, and Development Commands
- Configure (Debug): `xmake f -m debug`
- Build: `xmake build`
- Run client: `xmake run Client`
- Optional echo server (TLS):
  - `python -m venv .venv && source .venv/bin/activate`
  - `pip install -r test/requirements.txt`
  - Adjust cert paths/port in `test/WebsocketEchoServer.py` to match `src/main.cpp` (default 127.0.0.1:8888), then: `python test/WebsocketEchoServer.py`

## Coding Style & Naming Conventions
- C++23, header-first design; 2-space indent, no tabs; self-explanatory names; minimal comments.
- Public API names follow LogicTypes `Request::<Name>::method_name` (PascalCase).
  - JS `callNative('Name')`, GUI bindings, and `Logic::Session` method names must match exactly (e.g., `SignIn`, `GetConversationList`).
- Prefer modern std/Boost APIs; avoid raw pointers; use senders/receivers.

## Testing Guidelines
- Smoke run against local echo server to validate handshake, routing, and JSON/MsgPack round-trips.
- Add lightweight tests/tools under `test/`; name files by feature (e.g., `WebsocketEchoServer.py`).
- Verify UI flows manually: sign-in, conversation list, message history, add friend, user search.

## Commit & Pull Request Guidelines
- Use concise, imperative messages; prefer Conventional Commits (e.g., `feat:`, `fix:`, `refactor:`, `docs:`).
- PRs should include: clear description, rationale, before/after behavior, and screenshots for UI changes.
- Keep changes focused; do not reformat or revert unrelated files. Reference issues when applicable.

## Security & Configuration Tips
- TLS CA path is configured in `include/Network/Connection.hpp`; for local dev, point to `security/ca.crt` or a trusted CA on your system.
- Never commit real secrets. Dev certificates in `security/` are for local testing only.

