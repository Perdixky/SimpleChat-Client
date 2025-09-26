// Renderer-side logging: override console to forward to main for unified printing
// Keeps browser console readable and terminal output unified with C++ spdlog style

let done = false;

export function setupRendererLoggerOnce() {
  if (done) return; done = true;

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
        try { return JSON.stringify(arg); } catch { return 'Unserializable Object'; }
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

  // Global error/rejection logging
  window.addEventListener('error', (event) => {
    console.error(`[GLOBAL ERROR] ${event.message} at ${event.filename}:${event.lineno}:${event.colno}`);
    if (event.error) console.error(`[GLOBAL ERROR] Stack trace:`, event.error);
  });
  window.addEventListener('unhandledrejection', (event) => {
    console.error('[GLOBAL PROMISE] Unhandled promise rejection:', event.reason);
    if (event.reason && event.reason.stack) {
      console.error('[GLOBAL PROMISE] Stack trace:', event.reason.stack);
    }
  });

  // Connectivity + performance breadcrumbs
  window.addEventListener('online', () => { console.log('[NETWORK] Connection restored - back online'); });
  window.addEventListener('offline', () => { console.warn('[NETWORK] Connection lost - gone offline'); });
  window.addEventListener('load', () => { console.debug(`[PERF] Page fully loaded in ${performance.now().toFixed(2)}ms`); });

  console.log('[LOGGER] Unified logger initialized with enhanced debugging');
}

