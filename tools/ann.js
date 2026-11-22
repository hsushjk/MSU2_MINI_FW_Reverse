// ann.js -- annotate the WCH QingKe custom compressed halfword load/store sites.
// Ghidra decodes those slots as marker instructions (c.lbu.sp / c.sb.sp with no
// operands) because Sleigh rejects a 3-bit register operand in a quadrant-2
// pattern. This script restores the real operands from the raw halfword and
// reports which function each site belongs to.
//
// Quadrant-2 (b1:0 = 10) custom opcode, funct3 = 001 / 101:
//   rd   = x8 + (b4:b2)
//   base = x8 + (b9:b7)
//   off  = 8*(b12:b10) + 2*(b6:b5)       (even, 0..62)
//   op   = lhu / sh                      (16-bit access, zero-extended)
//
// usage: node ann.js [listingfile] [indexfile]
//   defaults: listing7.txt, dump7/index.tsv
const fs = require('fs');
const path = require('path');

const dir = __dirname;
const listingFile = process.argv[2] || path.join(dir, 'listing7.txt');
const indexFile = process.argv[3] || path.join(dir, 'dump7', 'index.tsv');

const X = ['zero', 'ra', 'sp', 'gp', 'tp', 't0', 't1', 't2',
           's0', 's1', 'a0', 'a1', 'a2', 'a3', 'a4', 'a5',
           'a6', 'a7', 's2', 's3', 's4', 's5', 's6', 's7',
           's8', 's9', 's10', 's11', 't3', 't4', 't5', 't6'];

function hw(hexbytes) {
  // listing hex column is memory order: "1a20" -> halfword 0x201A
  const b = hexbytes.match(/../g) || [];
  return parseInt(b[1] + b[0], 16) >>> 0;
}

// function table: addr + size (bytes) from index.tsv
const funcs = [];
if (fs.existsSync(indexFile)) {
  const lines = fs.readFileSync(indexFile, 'utf8').split(/\r?\n/);
  const head = lines[0].split('\t');
  const ia = head.indexOf('addr');
  const isz = head.indexOf('bytes') >= 0 ? head.indexOf('bytes') : head.indexOf('size');
  for (const ln of lines.slice(1)) {
    if (!ln.trim()) continue;
    const f = ln.split('\t');
    if (f.length <= Math.max(ia, isz)) continue;
    funcs.push({ addr: parseInt(f[ia], 16), size: parseInt(f[isz], 10), name: f[1] });
  }
  funcs.sort((a, b) => a.addr - b.addr);
}
function owner(addr) {
  let best = null;
  for (const f of funcs) if (f.addr <= addr && (best === null || f.addr > best.addr)) best = f;
  if (best && best.size && addr >= best.addr + best.size) return null;
  return best;
}

const out = [];
const perFunc = new Map();
const lines = fs.readFileSync(listingFile, 'utf8').split(/\r?\n/);
for (const ln of lines) {
  const m = ln.match(/^\s*ram:([0-9a-f]{8})\s+([0-9a-f]+)\s+(\S+)(.*)$/i);
  if (!m) continue;
  const addr = parseInt(m[1], 16);
  const bytes = m[2];
  const mn = m[3];
  if (mn !== 'c.lbu.sp' && mn !== 'c.sb.sp') continue;
  const h = hw(bytes);
  if ((h & 3) !== 2) continue;
  const rd = (h >> 2) & 7;
  const base = (h >> 7) & 7;
  const off = 8 * ((h >> 10) & 7) + 2 * ((h >> 5) & 3);
  const f3 = (h >> 13) & 7;
  const isLoad = f3 === 1;
  const txt = isLoad
    ? `lhu ${X[8 + rd]},0x${off.toString(16)}(${X[8 + base]})`
    : `sh  ${X[8 + rd]},0x${off.toString(16)}(${X[8 + base]})`;
  const f = owner(addr);
  const fname = f ? f.name : '(no function)';
  out.push(`ram:${m[1]} 0x${h.toString(16)} ${txt}\t${fname}`);
  perFunc.set(fname, (perFunc.get(fname) || 0) + 1);
}
fs.writeFileSync(path.join(dir, 'q2sites.txt'), out.join('\n') + '\n');
console.log(`sites: ${out.length}`);
console.log('functions with sites: ' + perFunc.size);
for (const [k, v] of [...perFunc.entries()].sort((a, b) => b[1] - a[1])) {
  console.log(`  ${v.toString().padStart(3)}  ${k}`);
}