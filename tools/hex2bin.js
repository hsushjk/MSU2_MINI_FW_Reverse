#!/usr/bin/env node
// Intel HEX -> flat binary, with integrity report.
//   node hex2bin.js "MSU2_MINI_Firmware_V1.1.hex" image.bin
//
// Records understood: 00 (data), 01 (EOF), 02 (extended segment address),
// 04 (extended linear address), 05 (start linear address).
// Gaps are filled with 0xFF. Overlapping data is an error.

const fs = require('fs');
const crypto = require('crypto');

const [inFile, outFile] = process.argv.slice(2);
if (!inFile || !outFile) {
  console.error('usage: node hex2bin.js <input.hex> <output.bin>');
  process.exit(2);
}

const text = fs.readFileSync(inFile, 'latin1');
const lines = text.split(/\r?\n/);

const bytes = new Map();
let extBase = 0;
let startLinear = null;
let records = 0;
let eof = false;

for (const [i, raw] of lines.entries()) {
  const line = raw.trim();
  if (!line) continue;
  if (line[0] !== ':') throw new Error(`line ${i + 1}: record does not start with ':'`);
  const blob = Buffer.from(line.slice(1), 'hex');
  if (blob.length < 5) throw new Error(`line ${i + 1}: record too short`);
  const len = blob[0];
  if (blob.length !== len + 5) throw new Error(`line ${i + 1}: length mismatch`);
  const addr = (blob[1] << 8) | blob[2];
  const type = blob[3];
  const data = blob.subarray(4, 4 + len);
  let sum = 0;
  for (const b of blob) sum = (sum + b) & 0xff;
  if (sum !== 0) throw new Error(`line ${i + 1}: checksum error`);

  records++;
  if (type === 0x00) {
    for (let k = 0; k < data.length; k++) {
      const a = extBase + addr + k;
      if (bytes.has(a)) throw new Error(`overlapping data at 0x${a.toString(16)}`);
      bytes.set(a, data[k]);
    }
  } else if (type === 0x01) {
    eof = true;
    break;
  } else if (type === 0x02) {
    extBase = ((data[0] << 8) | data[1]) << 4;
  } else if (type === 0x04) {
    extBase = ((data[0] << 8) | data[1]) << 16;
  } else if (type === 0x05) {
    startLinear = data.readUInt32BE(0);
  } else {
    throw new Error(`line ${i + 1}: unsupported record type 0x${type.toString(16)}`);
  }
}

if (!eof) console.warn('warning: no EOF record');

const addrs = [...bytes.keys()].sort((a, b) => a - b);
const lo = addrs[0];
const hi = addrs[addrs.length - 1];
if (hi - lo + 1 !== bytes.size) {
  const holes = [];
  let prev = lo;
  for (const a of addrs) {
    if (a - prev > 1) holes.push(`0x${(prev + 1).toString(16)}-0x${(a - 1).toString(16)}`);
    prev = a;
  }
  console.warn(`warning: ${bytes.size} bytes over range 0x${lo.toString(16)}-0x${hi.toString(16)}, holes: ${holes.join(', ')}`);
}

const out = Buffer.alloc(hi - lo + 1, 0xff);
for (const [a, v] of bytes) out[a - lo] = v;
fs.writeFileSync(outFile, out);

const sha = (b) => crypto.createHash('sha256').update(b).digest('hex');
console.log(`records   : ${records}`);
console.log(`range     : 0x${lo.toString(16).padStart(4, '0')} - 0x${hi.toString(16).padStart(4, '0')} (${out.length} bytes)`);
if (startLinear !== null) console.log(`entry     : 0x${startLinear.toString(8).padStart(8, '0')} (start linear address record)`);
console.log(`sha256    : ${sha(out)}`);
console.log(`sha256(raw): ${sha(fs.readFileSync(inFile))}`);