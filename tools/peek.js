// peek.js -- print listing lines around an address (skips data-block `db` lines).
// usage: node peek.js <addrHex> [count] [listingFile]
const fs = require('fs');
const path = require('path');
const dir = __dirname;
const addr = parseInt(process.argv[2].replace(/^0x/i, ''), 16);
const count = parseInt(process.argv[3] || '20', 10);
const file = process.argv[4] || path.join(dir, 'listing6.txt');

const rows = [];
for (const ln of fs.readFileSync(file, 'utf8').split(/\r?\n/)) {
  const m = ln.match(/^\s*ram:([0-9a-f]{8})\s+([0-9a-f]{2,16})\s+(\S+)(.*?)\s*\(GhidraScript\)\s*$/i);
  if (!m) continue;
  const a = parseInt(m[1], 16);
  if (m[3] === 'db') continue;
  rows.push({ a, text: `ram:${m[1]} ${m[2]} ${(m[3] + m[4]).replace(/\s+$/, '')}` });
}
rows.sort((x, y) => x.a - y.a);
let start = rows.findIndex(r => r.a >= addr);
if (start < 0) start = rows.length - 1;
for (const r of rows.slice(Math.max(0, start - 2), start + count)) console.log(r.text);