// fingerprints for the MSU2_MINI V1.1 image: whole-file + per-region SHA-256
const fs = require('fs');
const crypto = require('crypto');
const root = 'C:\\Users\\hsush\\Desktop\\MSU2_MINI';

const hexPath = root + '\\[\u56fa\u4ef6]MSU2_MINI_Firmware_V1.1.hex';
const binPath = root + '\\work\\image.bin';

const sha = (b) => crypto.createHash('sha256').update(b).digest('hex');
const hex = fs.readFileSync(hexPath);
const bin = fs.readFileSync(binPath);

const out = [];
out.push('HEX ' + hex.length + ' ' + sha(hex));
out.push('IMAGE ' + bin.length + ' ' + sha(bin));

const regions = [
  ['0x0000-0x0100', 'vector table + reset thunk', 0x0000, 0x0100],
  ['0x0100-0x0250', 'lib: save/restore stubs, memset, lut, memcpy', 0x0100, 0x0250],
  ['0x0250-0x0A90', 'command dispatch', 0x0250, 0x0A90],
  ['0x0A90-0x0E48', 'board init: clocks, TIM3, UART2', 0x0A90, 0x0E48],
  ['0x0E48-0x1110', 'USB endpoint helpers', 0x0E48, 0x1110],
  ['0x1110-0x1A66', 'USBFS ISR', 0x1110, 0x1A66],
  ['0x1A66-0x2668', 'SPI flash driver', 0x1A66, 0x2668],
  ['0x2668-0x2C9E', 'ST7735S driver', 0x2668, 0x2C9E],
  ['0x2C9E-0x39B4', 'drawing and streaming', 0x2C9E, 0x39B4],
  ['0x39B4-0x3D7C', 'main + boot + system', 0x39B4, 0x3D7C],
  ['0x3D7C-0x44E6', 'peripheral HAL', 0x3D7C, 0x44E6],
  ['0x44E6-0x4598', 'usart poll write + DMA m2m', 0x44E6, 0x4598],
  ['0x4598-0x56C5', 'printf family', 0x4598, 0x56C5],
  ['0x56C5-0x5EA0', 'jump table, strings, descriptors, constants', 0x56C5, 0x5EA0],
];
for (const [range, label, a, b] of regions) {
  out.push('REGION ' + range + ' ' + (b - a) + ' ' + sha(bin.subarray(a, b)) + ' ' + label);
}
console.log(out.join('\n'));