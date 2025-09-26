#!/usr/bin/env node
const fs = require('fs');
const path = require('path');

function resolveCandidate() {
  const root = path.resolve(__dirname, '..', '..');
  const plat = process.platform; // linux, win32, darwin
  const archNode = process.arch; // x64, arm64, ia32
  const archXmake = archNode === 'x64' ? 'x86_64' : archNode;
  const mode = 'release';
  const candidates = [
    path.join(root, 'build', plat, archXmake, mode, 'native.node'),
    path.join(root, 'build', plat, archNode, mode, 'native.node')
  ];
  for (const p of candidates) {
    if (fs.existsSync(p)) return p;
  }
  throw new Error('native.node not found. Built release with xmake first. Candidates: ' + candidates.join(', '));
}

(function main(){
  const src = resolveCandidate();
  const dst = path.resolve(__dirname, '..', 'native.node');
  fs.copyFileSync(src, dst);
  console.log('[prepare-native] copied', src, '->', dst);
})();
