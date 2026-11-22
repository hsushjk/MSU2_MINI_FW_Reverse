// regmap.js -- build an address -> "PERIPH->FIELD" map from ch32x035.h struct layouts.
//   node regmap.js  -> writes regmap.json
const fs = require('fs');
const path = require('path');
const ROOT = __dirname;
const hdr = fs.readFileSync(path.join(ROOT, 'ch32x035.h'), 'utf8');

const BASES = {
  GPIOA: 0x40010800, GPIOB: 0x40010C00, GPIOC: 0x40011000,
  SPI1: 0x40013000, USART1: 0x40013800, USART2: 0x40004400, USART3: 0x40004800,
  USART4: 0x40004C00, I2C1: 0x40005400,
  DMA1: 0x40020000, RCC: 0x40021000, FLASH_R: 0x40022000,
  TIM1: 0x40012C00, TIM2: 0x40000000, TIM3: 0x40000400,
  AFIO: 0x40010000, EXTI: 0x40010400, ADC1: 0x40012400,
  OPA: 0x40026000, PIOC: 0x40026C00, USBPD: 0x40027000, AWU: 0x40007000,
  WWDG: 0x40002C00, PWR: 0x40007000, IWDG: 0x40003000,
};
// instance name -> struct name in the header
const TYPEOF = {
  GPIOA: 'GPIO', GPIOB: 'GPIO', GPIOC: 'GPIO', SPI1: 'SPI', USART1: 'USART', USART2: 'USART',
  USART3: 'USART', USART4: 'USART', I2C1: 'I2C', DMA1: 'DMA', RCC: 'RCC', FLASH_R: 'FLASH',
  TIM1: 'TIM', TIM2: 'TIM', TIM3: 'TIM', AFIO: 'AFIO', EXTI: 'EXTI', ADC1: 'ADC', OPA: 'OPA',
  PIOC: 'PIOC', USBPD: 'USBPD', AWU: 'AWU', WWDG: 'WWDG', PWR: 'PWR', IWDG: 'IWDG',
};
// DMA channels: DMA_Channel layout, base = 0x40020000 + 0x08 + (n-1)*0x14
for (let n = 1; n <= 8; n++) {
  BASES['DMA1_CH' + n] = 0x40020000 + 0x08 + (n - 1) * 0x14;
  TYPEOF['DMA1_CH' + n] = 'DMA_Channel';
}

// parse "typedef struct { ... } NAME_TypeDef;" blocks
const structs = {};
const re = /typedef struct\s*\{([\s\S]*?)\}\s*(\w+)_TypeDef\s*;/g;
let m;
while ((m = re.exec(hdr))) {
  const fields = [];
  let off = 0;
  for (let line of m[1].split(/\r?\n/)) {
    line = line.replace(/\/\*[\s\S]*?\*\//g, '').trim();
    if (!line || line.startsWith('#')) continue;
    // e.g. "__IO uint32_t CTLR;"  /  "__IO uint8_t RESERVED0[4];"  / "uint32_t X:4;"
    const fm = line.match(/^(_IO|__IO|volatile)?\s*(u?int\d+_t|uint\d+_t|__IO)\s+(\w+)\s*(\[[^\]]*\])?\s*;?$/);
    if (!fm) continue;
    const type = fm[2];
    const name = fm[3];
    const arr = fm[4];
    const size = /8/.test(type) ? 1 : /16/.test(type) ? 2 : 4;
    const cnt = arr ? parseInt(arr.replace(/[\[\]]/g, '')) || 1 : 1;
    // align to the element size
    off = (off + size - 1) & ~(size - 1);
    fields.push({ name, off, size, count: cnt });
    off += size * cnt;
  }
  structs[m[2]] = fields;
}

const out = {};
for (const [pname, base] of Object.entries(BASES)) {
  const fields = structs[TYPEOF[pname] || pname];
  if (!fields) continue;
  for (const f of fields) {
    if (/^RESERVED|^RES0|^RES1|^RES2/i.test(f.name)) continue;
    const a = base + f.off;
    if (!(a in out)) out[a] = `${pname}->${f.name}`;
    // also name byte lanes inside a wider field
    if (f.size > 1) {
      for (let k = 1; k < f.size; k++) if (!((a + k) in out)) out[a + k] = `${pname}->${f.name} /*+${k}*/`;
    }
  }
}
// USBFSD is declared with unions that the simple parser cannot follow: use the verified map.
{
  const u = { 0x00: 'BASE_CTRL', 0x01: 'UDEV_CTRL', 0x02: 'INT_EN', 0x03: 'DEV_ADDR', 0x05: 'MIS_ST',
    0x06: 'INT_FG', 0x07: 'INT_ST', 0x08: 'RX_LEN', 0x0c: 'UEP4_1_MOD', 0x0d: 'UEP2_3_MOD',
    0x0e: 'UEP567_MOD', 0x10: 'UEP0_DMA', 0x14: 'UEP1_DMA', 0x18: 'UEP2_DMA', 0x1c: 'UEP3_DMA',
    0x20: 'UEP0_CTRL', 0x22: 'UEP0_CTRL_H', 0x24: 'UEP1_CTRL', 0x26: 'UEP1_CTRL_H',
    0x28: 'UEP2_CTRL', 0x2a: 'UEP2_CTRL_H', 0x2c: 'UEP3_CTRL', 0x2e: 'UEP3_CTRL_H',
    0x30: 'UEP4_CTRL', 0x32: 'UEP4_CTRL_H', 0x54: 'UEP5_DMA', 0x58: 'UEP6_DMA', 0x5c: 'UEP7_DMA',
    0x64: 'UEP5_CTRL', 0x68: 'UEP6_CTRL', 0x6c: 'UEP7_CTRL', 0x70: 'UEPX_MOD' };
  for (const [off, n] of Object.entries(u)) out[0x40023400 + Number(off)] = `USBFSD->${n}`;
}
// a few things the header does not describe
Object.assign(out, {
  0xe000e100: 'NVIC->ISER[0]', 0xe000e104: 'NVIC->ISER[1]',
  0xe000e180: 'NVIC->ICER[0]', 0xe000e184: 'NVIC->ICER[1]',
  0xe000ed04: 'SCB->ICSR', 0xe000ed08: 'SCB->VTOR',
});
const list = {};
for (const [a, n] of Object.entries(out)) list['0x' + (+a).toString(16)] = n;
fs.writeFileSync(path.join(ROOT, 'regmap.json'), JSON.stringify(list, null, 0));
console.log('structs:', Object.keys(structs).join(','));
console.log('registers:', Object.keys(list).length);
for (const a of ['0x40023400', '0x4002342a', '0x40023408', '0x4000440c', '0x40020094', '0x40012404', '0x40021000', '0x40010018', '0x40007000'])
  console.log(a, '->', list[a] || 'unknown');