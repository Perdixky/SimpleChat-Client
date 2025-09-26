// Renderer -> main logging bridge (planned extraction)
// Replace console methods to forward to main via preload bridge
export function hookConsole() {
  if (!window.bridge || typeof window.bridge.log !== 'function') return;
  const LogLevel = { TRACE: 0, DEBUG: 1, INFO: 2, WARNING: 3, ERROR: 4, FATAL: 5 };
  const orig = {
    log: console.log.bind(console),
    warn: console.warn.bind(console),
    error: console.error.bind(console),
    debug: console.debug.bind(console),
    trace: console.trace ? console.trace.bind(console) : console.log.bind(console)
  };
  function send(level, args) {
    const message = args.map(v => typeof v === 'object' ? safeStringify(v) : v).join(' ');
    try { window.bridge.log(level, message); } catch { orig.log(message); }
  }
  console.trace = (...a) => send(LogLevel.TRACE, a);
  console.debug = (...a) => send(LogLevel.DEBUG, a);
  console.log   = (...a) => send(LogLevel.INFO, a);
  console.warn  = (...a) => send(LogLevel.WARNING, a);
  console.error = (...a) => send(LogLevel.ERROR, a);
}

function safeStringify(obj) { try { return JSON.stringify(obj); } catch { return '[Unserializable Object]'; } }

