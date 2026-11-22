#!/usr/bin/env node
/* mkfw.js -- build a patched MSU2_MINI firmware image (boot screen = colour bars).
 *
 * Patches on top of image.bin (24 224 B, flash 0x0000..0x5E9F):
 *   0x03AAA  jal ra,lcd_fill_colour(0,0,160,80,0xFFFF)  ->  jal ra,bar_demo
 *   0x03AC6  jal ra,lcd_blit_colour_bg(0,4,160,68,...)  ->  jal ra,0x3BB2 (vendor `c.jr ra` stub)
 * and a self-contained routine + palette in the unused tail of the 64 KB flash:
 *   0x6000   bar_demo : eight 20x80 RGB565 bars, each one lcd_fill_colour(0x2A9C) call
 *   0x6100   palette  : 8 x u16 little-endian
 *
 * Output: msu2_bars.hex (Intel HEX, 16-byte records like the vendor file), msu2_bars.bin (64 KB)
 * usage : node mkfw.js [outdir]
 */
const fs = require('fs');
const path = require('path');

const WORK = __dirname;
const IMG = path.join(WORK, 'image.bin');
const ROUT = 0x6000;
const PAL = 0x6100;
const FILL_FN = 0x2A9C;
const RET_STUB = 0x3BB2;
const SITE_FILL = 0x3AAA;
const SITE_LOGO = 0x3AC6;

/* ---------------------------------------------------------------- assembler */
const REG = { zero: 0, ra: 1, sp: 2, gp: 3, tp: 4, t0: 5, t1: 6, t2: 7, s0: 8, s1: 9, a0: 10, a1: 11, a2: 12, a3: 13, a4: 14, a5: 15, a6: 16, a7: 17 };
const r = (n) => {
  const k = String(n).trim();
  if (k in REG) return REG[k];
  const m = /^x(\d+)$/.exec(k);
  if (m) return +m[1];
  throw new Error('bad register ' + k);
};
const sx = (v, bits) => {
  const lo = -(1 << (bits - 1)), hi = (1 << (bits - 1)) - 1;
  if (v < lo || v > hi) throw new Error(`immediate ${v} does not fit in ${bits} signed bits`);
  return v & ((1 << bits) - 1);
};

function asm(lines, base) {
  const labels = {};
  let pc = base;
  const items = [];
  for (const raw of lines) {
    const line = raw.replace(/#.*/, '').replace(/;.*/, '').trim();
    if (!line) continue;
    const lab = /^([A-Za-z_.$][\w.$]*):(.*)$/.exec(line);
    if (lab) {
      labels[lab[1]] = pc;
      if (!lab[2].trim()) continue;
      items.push({ pc, text: lab[2].trim() });
      pc += 4;
      continue;
    }
    items.push({ pc, text: line });
    pc += 4;
  }
  const val = (t) => {
    const k = String(t).trim();
    if (/^-?0x[0-9a-f]+$/i.test(k)) return k.toLowerCase().startsWith('-') ? -parseInt(k.slice(1), 16) : parseInt(k, 16);
    if (/^-?\d+$/.test(k)) return +k;
    if (k in labels) return labels[k];
    throw new Error('bad operand ' + k);
  };
  const out = Buffer.alloc(items.length * 4);
  items.forEach((it, i) => {
    const f = it.text.replace(/,/g, ' ').split(/\s+/).filter(Boolean);
    const op = f[0].toLowerCase();
    const I = (imm, rs1, funct3, rd, opcode) => (((imm & 0xfff) << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode) >>> 0;
    const Rf = (funct7, rs2, rs1, funct3, rd, opcode) => ((funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode) >>> 0;
    const St = (imm, rs2, rs1, funct3, opcode) => (((imm & 0xfe0) << 20) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | ((imm & 0x1f) << 7) | opcode) >>> 0;
    const B = (off, rs2, rs1, funct3) => {
      const o = off & 0x1fff;
      return (((o >> 12) & 1) << 31 | ((o >> 5) & 0x3f) << 25 | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | ((o >> 1) & 0xf) << 8 | ((o >> 11) & 1) << 7 | 0x63) >>> 0;
    };
    const J = (off, rd) => {
      const o = off & 0x1fffff;
      return (((o >> 20) & 1) << 31 | ((o >> 1) & 0x3ff) << 21 | ((o >> 11) & 1) << 20 | ((o >> 12) & 0xff) << 12 | (rd << 7) | 0x6f) >>> 0;
    };
    const mem = (t) => {
      const m = /^(-?(?:0x[0-9a-f]+|\d+))\((\w+)\)$/i.exec(String(t).replace(/\s+/g, ''));
      if (!m) throw new Error('bad memory operand ' + t);
      return { imm: val(m[1]), rs1: r(m[2]) };
    };
    let w;
    switch (op) {
      case 'addi': w = I(sx(val(f[3]), 12), r(f[2]), 0, r(f[1]), 0x13); break;
      case 'slli': w = I(val(f[3]) & 0x1f, r(f[2]), 1, r(f[1]), 0x13); break;
      case 'lui': w = (((val(f[2]) & 0xfffff) << 12) | (r(f[1]) << 7) | 0x37) >>> 0; break;
      case 'add': w = Rf(0, r(f[3]), r(f[2]), 0, r(f[1]), 0x33); break;
      case 'mul': w = Rf(1, r(f[3]), r(f[2]), 0, r(f[1]), 0x33); break;
      case 'sw': { const m = mem(f[2]); w = St(sx(m.imm, 12), r(f[1]), m.rs1, 2, 0x23); break; }
      case 'lw': { const m = mem(f[2]); w = I(sx(m.imm, 12), m.rs1, 2, r(f[1]), 0x03); break; }
      case 'lhu': { const m = mem(f[2]); w = I(sx(m.imm, 12), m.rs1, 5, r(f[1]), 0x03); break; }
      case 'lbu': { const m = mem(f[2]); w = I(sx(m.imm, 12), m.rs1, 4, r(f[1]), 0x03); break; }
      case 'sb': { const m = mem(f[2]); w = St(sx(m.imm, 12), r(f[1]), m.rs1, 0, 0x23); break; }
      case 'jalr': { const m = mem(f[3]); w = I(sx(m.imm, 12), m.rs1, 0, r(f[1]), 0x67); break; }
      case 'ret': w = I(0, 1, 0, 0, 0x67); break;
      case 'li': {
        const v = val(f[2]);
        if (v >= -2048 && v <= 2047) { w = I(sx(v, 12), 0, 0, r(f[1]), 0x13); break; }
        throw new Error('li: only 12-bit immediates are supported here: ' + v);
      }
      case 'call': w = J(val(f[2]) - it.pc, r(f[1])); break;          // call ra,<abs addr|label>
      case 'j': w = J(val(f[1]) - it.pc, 0); break;
      case 'blt': w = B(val(f[3]) - it.pc, r(f[2]), r(f[1]), 4); break;
      case 'bne': w = B(val(f[3]) - it.pc, r(f[2]), r(f[1]), 1); break;
      case 'beq': w = B(val(f[3]) - it.pc, r(f[2]), r(f[1]), 0); break;
      default: throw new Error('unsupported mnemonic ' + op);
    }
    out.writeUInt32LE(w >>> 0, i * 4);
  });
  return { bytes: out, labels };
}

/* ---------------------------------------------------------------- bar routine */
const SRC = [
  'bar_demo:',
  '  addi sp sp -16',
  '  sw ra 12(sp)',
  '  sw s0 8(sp)',
  '  sw s1 4(sp)',
  '  lui s1 0x6',           // s1 = 0x6000
  '  addi s1 s1 0x100',     // s1 = palette (0x6100)
  '  li s0 0',              // bar index i
  'loop:',
  '  li a0 20',
  '  mul a0 a0 s0',         // x = 20 * i
  '  li a1 0',              // y
  '  li a2 20',             // w
  '  li a3 80',             // h  (full panel height)
  '  slli t0 s0 1',
  '  add t0 s1 t0',
  '  lhu a4 0(t0)',         // colour from palette
  '  call ra 0x2a9c',       // lcd_fill_colour(x, y, w, h, colour)
  '  addi s0 s0 1',
  '  li t0 8',
  '  blt s0 t0 loop',
  '  lw ra 12(sp)',
  '  lw s0 8(sp)',
  '  lw s1 4(sp)',
  '  addi sp sp 16',
  '  ret',
  'stub_ret:',              // target for the disabled logo blit
  '  ret',
];
const BARS = [0xFFFF, 0xFFE0, 0x07FF, 0x07E0, 0xF81F, 0xF800, 0x001F, 0x0000];

/* ---------------------------------------------------------------- build */
const { bytes: code, labels } = asm(SRC, ROUT);
const STUB_RET = labels.stub_ret;
const patchFill = asm([`call ra 0x${ROUT.toString(16)}`], SITE_FILL).bytes;
const patchLogo = asm([`call ra 0x${STUB_RET.toString(16)}`], SITE_LOGO).bytes;

const img = fs.readFileSync(IMG);
if (img.length !== 0x5ea0) throw new Error('unexpected image size ' + img.length);

const out = Buffer.alloc(0x10000, 0xff);
img.copy(out);
patchFill.copy(out, SITE_FILL);
patchLogo.copy(out, SITE_LOGO);
code.copy(out, ROUT);
BARS.forEach((c, i) => out.writeUInt16LE(c, PAL + i * 2));

const END = PAL + BARS.length * 2;
const crcLine = (addr, data, type) => {
  let sum = data.length + ((addr >> 8) & 0xff) + (addr & 0xff) + type;
  let s = ':' + data.length.toString(16).padStart(2, '0').toUpperCase() + (addr & 0xffff).toString(16).padStart(4, '0').toUpperCase() + type.toString(16).padStart(2, '0').toUpperCase();
  for (const b of data) { sum += b; s += b.toString(16).padStart(2, '0').toUpperCase(); }
  return s + ((0x100 - (sum & 0xff)) & 0xff).toString(16).padStart(2, '0').toUpperCase();
};
const lines = [];
for (let a = 0; a < END; a += 16) {
  const chunk = [...out.slice(a, Math.min(a + 16, END))];
  if (chunk.every((b) => b === 0xff)) continue;
  lines.push(crcLine(a, chunk, 0));
}
lines.push(':00000001FF');

const outdir = process.argv[2] || WORK;
fs.writeFileSync(path.join(outdir, 'msu2_bars.hex'), lines.join('\r\n') + '\r\n');
fs.writeFileSync(path.join(outdir, 'msu2_bars.bin'), out);

console.log('bar_demo      : ' + code.length + ' bytes @ 0x' + ROUT.toString(16));
console.log('palette       : 8 u16 @ 0x' + PAL.toString(16));
console.log('patch 0x' + SITE_FILL.toString(16) + ' : ' + patchFill.toString('hex') + '  (jal ra, bar_demo)');
console.log('patch 0x' + SITE_LOGO.toString(16) + ' : ' + patchLogo.toString('hex') + '  (jal ra, ret stub)');
console.log('hex records   : ' + (lines.length - 1) + ' + EOF');
console.log('out           : ' + path.join(outdir, 'msu2_bars.hex'));
console.log('out           : ' + path.join(outdir, 'msu2_bars.bin') + ' (64 KB)');