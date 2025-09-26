// Unified logger for Electron (Node) side to match C++ spdlog style
// Format: [HH:MM:SS.ffffff] [level] [pid] message
// Only the [level] token is colorized similar to spdlog's %^%l%$.

const hr0 = process.hrtime.bigint();
const t0 = Date.now();

const LEVELS = ['trace', 'debug', 'info', 'warn', 'error', 'fatal'];

function two(n) { return n < 10 ? `0${n}` : `${n}`; }
function six(n) {
  if (n < 10) return `00000${n}`;
  if (n < 100) return `0000${n}`;
  if (n < 1000) return `000${n}`;
  if (n < 10000) return `00${n}`;
  if (n < 100000) return `0${n}`;
  return `${n}`;
}

function nowFmt() {
  // Derive wall-clock microseconds using a stable offset from hrtime
  const dHr = process.hrtime.bigint() - hr0; // ns
  const dMicro = Number(dHr / 1000n); // microseconds since start
  const msAbs = t0 + Math.floor(dMicro / 1000); // ms
  const microRema = dMicro % 1000000; // 0..999999
  const d = new Date(msAbs);
  return `[${two(d.getHours())}:${two(d.getMinutes())}:${two(d.getSeconds())}.${six(microRema)}]`;
}

const colors = {
  reset: "\x1b[0m",
  gray: "\x1b[90m",
  blue: "\x1b[34m",
  green: "\x1b[32m",
  yellow: "\x1b[33m",
  red: "\x1b[31m",
  magenta: "\x1b[35m",
};

function colorFor(level) {
  switch (level) {
    case 'trace': return colors.gray;
    case 'debug': return colors.blue;
    case 'info': return colors.green;
    case 'warn': return colors.yellow;
    case 'error': return colors.red;
    case 'fatal': return colors.magenta;
    default: return colors.green;
  }
}

function normalizeLevel(level) {
  if (typeof level === 'number') return LEVELS[level] || 'info';
  if (!level) return 'info';
  const s = String(level).toLowerCase();
  return LEVELS.includes(s) ? s : 'info';
}

function printUnified(level, pid, msg) {
  const lvl = normalizeLevel(level);
  const col = colorFor(lvl);
  const levelToken = `[${col}${lvl}${colors.reset}]`;
  const ts = nowFmt();
  const pidTok = `[${pid ?? process.pid}]`;
  const line = `${ts} ${levelToken} ${pidTok} ${msg}`;
  if (lvl === 'error' || lvl === 'fatal' || lvl === 'warn') {
    (lvl === 'warn' ? console.warn : console.error)(line);
  } else {
    console.log(line);
  }
}

module.exports = {
  LEVELS,
  normalizeLevel,
  printUnified,
};

