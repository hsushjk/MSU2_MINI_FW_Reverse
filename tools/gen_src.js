// gen_src.js -- assemble a readable source tree from the Ghidra dumps.
//   node gen_src.js
// reads : dump7/funcs/*.c, symbols.tsv, regmap.json, image.bin, q2sites.txt
// writes: src/<module>.c, src/strings.h, src/index.md
const fs = require('fs');
const path = require('path');
const ROOT = __dirname;
const FUNCS = path.join(ROOT, 'dump7', 'funcs');
const OUT = path.join(ROOT, 'src');
const IMG = fs.readFileSync(path.join(ROOT, 'image.bin'));

// ---- symbol table ---------------------------------------------------------
const sym = new Map();
for (const line of fs.readFileSync(path.join(ROOT, 'symbols.tsv'), 'utf8').split(/\r?\n/)) {
  if (!line.trim() || line.startsWith('#')) continue;
  const [a, name, module, note] = line.split('\t');
  sym.set(parseInt(a, 16), { name, module: module || 'misc', note: note || '' });
}
const hex4 = (a) => a.toString(16).toUpperCase().padStart(4, '0');
const nameOf = (addr) => (sym.get(addr) ? sym.get(addr).name : 'sub_' + hex4(addr));
const modOf = (addr) => (sym.get(addr) || { module: 'misc' }).module;

// ---- registers -----------------------------------------------------------
const regmap = new Map(Object.entries(JSON.parse(fs.readFileSync(path.join(ROOT, 'regmap.json'), 'utf8')))
  .map(([a, n]) => [parseInt(a, 16), n]));

// ---- ram globals ---------------------------------------------------------
const globals = [
  [0x20000080, 'SystemCoreClock', 'u32', 'copy of the flash constant 0x02DC6C00 = 48 000 000'],
  [0x200000ce, 'g_main_state', 'u8', 'main-loop state: 0 = command mode, 1 = stream mode'],
  [0x200000cc, 'g_tick', 'u16', 'incremented by the TIM3 update ISR'],
  [0x200000d2, 'g_rx_cmd', 'u16', 'last command word received over the serial channel'],
  [0x200000d0, 'g_adc_value', 'u16', 'last analogue reading'],
  [0x200000d4, 'g_filter_k', 'u16', 'smoothing coefficient set by command 0x30 / sub 1'],
  [0x200000d8, 'g_last_read', 'u16', 'result of the last class-8 read request'],
  [0x200000c4, 'g_reply_buf', 'u8[6]', '6-byte reply buffer'],
  [0x200000c0, 'g_frame_state', 'u8', 'per-frame state byte copied out of the rx queue'],
  [0x200000b4, 'g_rect_x0', 'u16', ''], [0x200000b8, 'g_rect_y0', 'u16', ''],
  [0x200000b6, 'g_rect_x1', 'u16', ''], [0x200000ba, 'g_rect_y1', 'u16', ''],
  [0x200000b2, 'g_page_a', 'u16', 'flash page of the foreground object'],
  [0x200000b0, 'g_bg_page', 'u16', 'flash page of the background image / second blit argument'],
  [0x200000bd, 'g_display_rotation', 'u8', 'MADCTL-style rotation selector, set by command 2/3/A'],
  [0x20000050, 'g_page_b', 'u16', 'flash page of the background image'],
  [0x20000130, 'g_draw_params', 'u8[]', 'drawing parameter block'],
  [0x2000012c, 'g_param_records', 'u8[]', 'parameter records, 16 bytes each, bytes at +0,+4,+8,+0xC'],
  [0x200002e4, 'g_lut_256', 'u8[256]', '256-entry byte table filled by command 0x30 / sub 4'],
  [0x20000be4, 'g_ep2_buf', 'u8[16][64]', 'USB EP2 OUT staging buffer, 16 slots of 64 bytes'],
  [0x20000fe4, 'g_rxq', 'rxq_t', 'received-packet queue descriptor'],
  [0x20000234, 'g_dma_ring', 'u8[144]', 'staging ring fed by DMA1 channel 8, drained in 6-byte steps'],
];
const gmap = new Map(globals.map(([a, name]) => [a, name]));

// ---- string literals -----------------------------------------------------
const strings = new Map(); // addr -> text
function cstr(addr) {
  if (strings.has(addr)) return strings.get(addr);
  let s = '';
  for (let i = addr; i < IMG.length && IMG[i]; i++) s += String.fromCharCode(IMG[i]);
  strings.set(addr, s);
  return s;
}
const strName = (addr) => 'str_' + hex4(addr);

// ---- known data structures in the constant area --------------------------
const dataSym = new Map([
  [0x5c74, 'usb_config_desc'],
  [0x5ce0, 'usb_device_desc'],
  [0x5cf4, 'usb_report_desc'],
  [0x5d18, 'usb_str_manufacturer'],
  [0x5d28, 'usb_str_product'],
  [0x5d3c, 'usb_str_serial'],
  [0x5718, 'flash_info_table'],
  [0x5848, 'logo_glyphs'],
]);

// ---- WCH quadrant-2 marker sites, per function --------------------------
const q2 = new Map();
// addr -> Ghidra's own function name (q2sites.txt is keyed by it)
const gh = new Map();
try {
  const ls = fs.readFileSync(path.join(ROOT, 'dump7', 'index.tsv'), 'utf8').split(/\r?\n/);
  for (const ln of ls.slice(1)) {
    const f = ln.split('\t');
    if (f.length > 1) gh.set(parseInt(f[0], 16), f[1]);
  }
} catch (e) { /* optional */ }
const ghidraName = (a) => gh.get(a) || '';
try {
  for (const ln of fs.readFileSync(path.join(ROOT, 'q2sites.txt'), 'utf8').split(/\r?\n/)) {
    const m = ln.match(/^ram:([0-9a-f]{8}) 0x([0-9a-f]+) (.+?)\t(.+)$/);
    if (!m) continue;
    const fn = m[4];
    if (!q2.has(fn)) q2.set(fn, []);
    q2.get(fn).push({ addr: parseInt(m[1], 16), text: m[3].replace(/\s+/g, ' ') });
  }
} catch (e) { /* optional */ }

// ---- collect and sort functions -----------------------------------------
const funcs = [];
for (const f of fs.readdirSync(FUNCS)) {
  const m = f.match(/^([0-9a-f]{8})_.*\.c$/);
  if (m) funcs.push({ addr: parseInt(m[1], 16), file: path.join(FUNCS, f) });
}
funcs.sort((a, b) => a.addr - b.addr);

function rewrite(text) {
  let t = text;
  t = t.replace(/FUN_ram_([0-9a-f]{8})/g, (_, h) => nameOf(parseInt(h, 16)));
  // known descriptor / table addresses
  t = t.replace(/&?DAT_ram_(00005[0-9a-f]{3})\b/g, (m0, h) => {
    const a = parseInt(h, 16);
    return dataSym.has(a) ? dataSym.get(a) : m0;
  });
  // string labels -> str_XXXX
  t = t.replace(/\bs_[A-Za-z0-9_]*(?:ram_)?([0-9a-f]{8})\b/g, (m0, h) => {
    const a = parseInt(h, 16);
    if (a < 0x5700 || a > 0x5EA0) return m0;
    cstr(a);
    return strName(a);
  });
  // data references into the constant/string area become string symbols
  t = t.replace(/&?DAT_ram_(00005[0-9a-f]{3})\b/g, (m0, h) => {
    const a = parseInt(h, 16);
    if (a < 0x5700 || a >= 0x5EA0) return m0;
    if (dataSym.has(a)) return m0;
    if (a >= 0x5D90 && a < 0x5E00) return m0; // printf lookup tables, not text
    const s = cstr(a);
    if (!/^[\x20-\x7e]/.test(s)) return m0;
    return strName(a);
  });
  // MMIO registers
  t = t.replace(/(_?)DAT_ram_([0-9a-f]{8})(?:_\d+)?\b/gi, (m0, us, h) => {
    const a = parseInt(h, 16);
    if (gmap.has(a)) return gmap.get(a);
    if (regmap.has(a)) return regmap.get(a);
    return m0;
  });
  // gp-relative globals
  t = t.replace(/\*\((?:undefined\d|char|short|int|uint|byte|ushort|u?int\d+_t|longlong) ?\*\)\(gp \+ (-?0x[0-9a-f]+)\)/g, (m0, off) => {
    const a = 0x20000880 + parseInt(off, 16);
    return gmap.has(a) ? gmap.get(a) : m0;
  });
  // bare peripheral-address literals used as pointers/arguments
  t = t.replace(/\b&?0x(4000[0-9a-f]{4}|4001[0-9a-f]{4}|4002[0-9a-f]{4})\b/gi, (m0, h) => {
    const a = parseInt(h, 16);
    if (!regmap.has(a)) return m0;
    return (m0.startsWith('&') ? '&' : '') + regmap.get(a);
  });
  return t;
}

function section(rec) {
  const src = fs.readFileSync(rec.file, 'utf8');
  const lines = src.split(/\r?\n/);
  const hdr = lines.filter(l => l.startsWith('// Signature:'));
  const bodyStart = lines.findIndex(l => /^[a-zA-Z_].*\(/.test(l));
  const body = lines.slice(bodyStart).join('\n').replace(/\n{3,}$/, '\n');
  const s = sym.get(rec.addr) || { note: '' };
  const out = [];
  out.push('/* ------------------------------------------------------------------');
  out.push(` * ${nameOf(rec.addr)}   @ 0x${hex4(rec.addr)}`);
  if (s.note) out.push(` * ${s.note}`);
  for (const h of hdr) out.push(' * ' + h.replace(/^\/\/ /, '').slice(0, 160));
  const sites = q2.get(ghidraName(rec.addr));
  if (sites && sites.length) {
    out.push(' *');
    out.push(' * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):');
    for (const st of sites.slice(0, 14)) out.push(' *   0x' + hex4(st.addr) + '  ' + st.text);
    if (sites.length > 14) out.push(' *   ... ' + (sites.length - 14) + ' more');
  }
  out.push(' * ------------------------------------------------------------------ */');
  out.push(rewrite(body).trimEnd());
  return out.join('\n');
}

const byModule = new Map();
for (const rec of funcs) {
  const mod = modOf(rec.addr);
  if (!byModule.has(mod)) byModule.set(mod, []);
  byModule.get(mod).push(rec);
}

fs.mkdirSync(OUT, { recursive: true });
const header = (mod) => `/* MSU2_MINI firmware 1.1 -- reconstructed C, module "${mod}".
 * Target: WCH CH32X033F8P6 (RISC-V RV32IMC + WCH byte load/store extensions), 48 MHz, 64 KB flash,
 * 20 KB SRAM at 0x20000000, 1 MB SPI NOR (P25D80), 0.96" 160x80 ST7735S panel.
 *
 * Reconstructed from the vendor image (24 224 bytes, flash 0x0000-0x5E9F) with Ghidra 12.2
 * (RISCV:LE:32:default) plus a patched riscv.rvc.sinc. The WCH core re-uses the RVC c.fld/c.fsd
 * encodings for byte loads/stores: quadrant 0 is decoded exactly (c.lbu/c.sb), quadrant 2
 * (c.lbu.sp/c.sb.sp, 16-bit lhu/sh) cannot be expressed in Sleigh 12.2 and is left as an
 * operand-free marker; those sites are listed per function below and in work/q2sites.txt.
 * Bodies are the decompiler output with symbol and register names substituted; nothing was
 * "cleaned up", so the original structure is intact. Symbols named sub_XXXX were not
 * identified; str_XXXX are C strings in flash. "extraout_x" values are incoming argument
 * registers that Ghidra lost track of across the -msave-restore prologue helper.
 */
#include "registers.h"
#include "strings.h"
`;
const idx = ['# Reconstructed source index', '', '| module | file | functions |', '|---|---|---|'];
for (const [mod, list] of [...byModule.entries()].sort()) {
  fs.writeFileSync(path.join(OUT, mod + '.c'),
    header(mod) + '\n' + list.map(section).join('\n\n') + '\n');
  idx.push(`| ${mod} | \`${mod}.c\` | ${list.length} |`);
}

// strings.h
{
  const lines = ['/* String literals in flash, decoded 1:1 from image.bin. */', '#ifndef MSU2_STRINGS_H', '#define MSU2_STRINGS_H', ''];
  for (const [a, s] of [...strings.entries()].sort((x, y) => x[0] - y[0])) {
    if (dataSym.has(a)) continue;
    const esc = s.replace(/\\/g, '\\\\').replace(/"/g, '\\"').replace(/\r/g, '\\r').replace(/\n/g, '\\n').replace(/\t/g, '\\t')
      .replace(/[\x00-\x1f\x7f-\xff]/g, (c) => '\\x' + c.charCodeAt(0).toString(16).padStart(2, '0'));
    if (/^[\x20-\x7e\r\n\t]+$/.test(s)) {
      lines.push(`#define ${strName(a).padEnd(12)} "${esc}"   /* flash 0x${hex4(a)} */`);
    } else {
      lines.push(`#define ${strName(a).padEnd(12)} ((const void*)0x${a.toString(16).toUpperCase()})   /* binary data */`);
    }
  }
  lines.push('', '#endif');
  fs.writeFileSync(path.join(OUT, 'strings.h'), lines.join('\n') + '\n');
}

// registers.h -- one macro per referenced register
{
  const used = new Set();
  for (const f of funcs) {
    const t = fs.readFileSync(f.file, 'utf8');
    for (const m of t.matchAll(/(?:_?)DAT_ram_([0-9a-f]{8})/gi)) used.add(parseInt(m[1], 16));
  }
  const lines = ['/* Peripheral registers referenced by the code (names from the WCH ch32x035.h layouts). */',
    '#ifndef MSU2_REGISTERS_H', '#define MSU2_REGISTERS_H', '',
    'typedef struct { volatile unsigned int r[0x8000]; } periph_t;', ''];
  for (const a of [...used].sort((x, y) => x - y)) {
    const n = regmap.get(a);
    if (!n) continue;
    const [inst, field] = n.split('->');
    const f = (field || '').split(' ')[0];
    if (!f) continue;
    lines.push(`#define ${inst}_${f}${' '.repeat(Math.max(1, 14 - (inst + '_' + f).length))}(*(volatile uint32_t*)0x${a.toString(16).toUpperCase()}) /* ${n} */`);
  }
  lines.push('', '#endif');
  fs.writeFileSync(path.join(OUT, 'registers.h'), lines.join('\n') + '\n');
}

// index
idx.push('', '## Function index', '', '| address | name | module | note |', '|---|---|---|---|');
for (const rec of funcs) {
  const s = sym.get(rec.addr) || { module: 'misc', note: '' };
  idx.push(`| 0x${hex4(rec.addr)} | ${nameOf(rec.addr)} | ${s.module} | ${s.note.replace(/\|/g, '\\|')} |`);
}
fs.writeFileSync(path.join(OUT, 'index.md'), idx.join('\n') + '\n');
console.log(`modules ${byModule.size}, functions ${funcs.length}, strings ${strings.size}, regs ${regmap.size}`);
for (const [m, l] of byModule) console.log(' ', m.padEnd(8), l.length);