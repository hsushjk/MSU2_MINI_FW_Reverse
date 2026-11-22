#!/usr/bin/env node
// Generates the four SVG diagrams in docs/diagrams/ from plain box/edge specs.
// Kept as a script so the diagrams stay reproducible and editable in text form.
//   node tools/gen_diagrams.js
const fs = require('fs');
const path = require('path');

const OUT = path.join(__dirname, '..', 'docs', 'diagrams');
fs.mkdirSync(OUT, { recursive: true });

const FONT = 'Segoe UI, Microsoft YaHei, sans-serif';
const MONO = 'Consolas, Courier New, monospace';

function esc(s) {
  return String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}

function svg(width, height, body, title) {
  return `<?xml version="1.0" encoding="UTF-8"?>
<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}" viewBox="0 0 ${width} ${height}">
<title>${esc(title)}</title>
<defs>
<marker id="a" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
<path d="M 0 0 L 10 5 L 0 10 z" fill="#555"/></marker>
</defs>
<rect width="100%" height="100%" fill="#ffffff"/>
${body}
</svg>
`;
}

// box: x,y,w,h + title + optional lines, tone: normal | accent | warn
function box(x, y, w, h, title, lines = [], tone = 'normal') {
  const fill = tone === 'accent' ? '#eef3f9' : tone === 'warn' ? '#fbf1f0' : '#f7f7f7';
  const stroke = tone === 'accent' ? '#4a6f9c' : tone === 'warn' ? '#a9524a' : '#9a9a9a';
  let s = `<rect x="${x}" y="${y}" width="${w}" height="${h}" rx="4" fill="${fill}" stroke="${stroke}" stroke-width="1.2"/>`;
  s += `<text x="${x + 10}" y="${y + 21}" font-family="${FONT}" font-size="14" font-weight="600" fill="#1c1c1c">${esc(title)}</text>`;
  lines.forEach((ln, i) => {
    const mono = /^0x|^[A-Z_]{3,}|\|/.test(ln);
    s += `<text x="${x + 10}" y="${y + 42 + i * 17}" font-family="${mono ? MONO : FONT}" font-size="12" fill="#3a3a3a">${esc(ln)}</text>`;
  });
  return s;
}

function arrow(x1, y1, x2, y2, label, dash = false) {
  let s = `<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="#555" stroke-width="1.3"${dash ? ' stroke-dasharray="5 4"' : ''} marker-end="url(#a)"/>`;
  if (label) {
    const mx = (x1 + x2) / 2, my = (y1 + y2) / 2;
    s += `<text x="${mx + 6}" y="${my - 5}" font-family="${FONT}" font-size="11" fill="#5a5a5a">${esc(label)}</text>`;
  }
  return s;
}

function poly(pts, label) {
  const d = pts.map((p, i) => `${i ? 'L' : 'M'} ${p[0]} ${p[1]}`).join(' ');
  let s = `<path d="${d}" fill="none" stroke="#555" stroke-width="1.3" marker-end="url(#a)"/>`;
  if (label) {
    const [x, y] = pts[pts.length - 2];
    s += `<text x="${x + 6}" y="${y - 6}" font-family="${FONT}" font-size="11" fill="#5a5a5a">${esc(label)}</text>`;
  }
  return s;
}

function caption(x, y, text) {
  return `<text x="${x}" y="${y}" font-family="${FONT}" font-size="12" fill="#6b6b6b">${esc(text)}</text>`;
}

// ---------------------------------------------------------------- 1. boot chain
{
  const W = 980, H = 660;
  let b = '';
  b += `<text x="30" y="34" font-family="${FONT}" font-size="17" font-weight="700" fill="#111">MSU2_MINI V1.1 启动链</text>`;
  b += box(30, 56, 300, 62, '上电复位', ['rom:0x0000 → 向量表取 PC（mtvec = 7）', '异常向量地址 = 4 + 4n']);
  b += box(30, 148, 300, 118, 'crt0  _start  0x3BB4', [
    'gp  = 0x20000880   sp = 0x20005000',
    '.data 0x5DF0 → 0x20000000（176 B）',
    '.bss  0x200000B0 → 0x200011AF（4352 B）',
    '写 mtvec / 开全局中断',
  ]);
  b += box(30, 296, 300, 96, 'SystemInit  0x3B32', [
    'RCC->CTLR |= HSION',
    'FLASH->ACTLR = (x & ~3) | 2',
    'RCC->CFGR0 &= 0xF8FFFF0F  ⇒ 48 MHz',
    '不启用 PLL，无外部晶振',
  ]);
  b += box(30, 422, 300, 74, 'SystemCoreClockUpdate  0x3B74', [
    'SystemCoreClock = 0x02DC6C00',
    '写入 RAM 0x20000080（48 000 000）',
  ], 'accent');
  b += box(30, 526, 300, 88, '跳转 main（不可见于调用流）', [
    'lui t0, 0x4 ; csrw mepc, t0',
    'mret            → 0x39B4',
    '任何调用流分析都追不到此处',
  ], 'warn');
  b += arrow(180, 118, 180, 148);
  b += arrow(180, 266, 180, 296);
  b += arrow(180, 392, 180, 422);
  b += arrow(180, 496, 180, 526);

  b += box(376, 56, 574, 250, 'main  0x39B4 —— 初始化顺序', [
    '__riscv_save_8 / write_once_flag(1) / SystemCoreClockUpdate / Delay_Init',
    'USART2_Init(0x1C200) → printf("SystemClk:%d") → GetChipID → printf("ChipID:%08x")',
    'board_clocks_enable / tim3_init / board_gpio_init',
    'pvd_probe → usb_device_init(1, 上拉检测)',
    'spi_dma_tx_init(CH3) / spi_dma_rx_init(CH2) / dma_ring_init(CH8)',
    'spi1_gpio_init / flash_uid→record / licence_check / spi1_gpio_init2',
    'adc_init / lcd_init / lcd_fill_colour(0,0,160,80,白)',
    '授权失败时叠三行提示文字（不影响任何功能）',
    'reset_session()',
  ]);
  b += box(376, 336, 574, 96, 'main_loop  0x59A', [
    '每轮：取环内一帧 → cmd_dispatch → 组 6 字节响应 → EP3 IN',
    'TIMS 1 kHz tick：超过 5 秒无新帧 → reset_session',
    'class 4（批量像素）在此直接处理，不经过 cmd_dispatch',
  ], 'accent');
  b += arrow(676, 306, 676, 336);
  b += poly([[330, 566], [376, 566], [376, 306]], 'mret');
  b += caption(30, 648, '说明：左侧为真实执行顺序；右侧为 main 内部的调用顺序，箭头仅表示先后，不表示所有调用都发生在同一层。');
  fs.writeFileSync(path.join(OUT, 'boot.svg'), svg(W, H, b, '启动链'));
}

// ---------------------------------------------------------------- 2. state machine
{
  const W = 980, H = 470;
  let b = '';
  b += `<text x="30" y="34" font-family="${FONT}" font-size="17" font-weight="700" fill="#111">main_loop 状态机</text>`;
  b += box(70, 66, 260, 104, '状态 0：空闲', ['g_main_state = 0', '逐帧读取并分派', '等待握手帧'], 'normal');
  b += box(640, 66, 300, 104, '状态 1：流式', ['g_main_state = 1', '受理 LCD 数据帧与批量像素', '握手后进入，超时后退回'], 'accent');
  b += arrow(330, 118, 640, 118, '');
  b += `<text x="396" y="108" font-family="${FONT}" font-size="11" fill="#5a5a5a">握手帧 00 4D 53 4E 43 4E</text>`;

  b += box(70, 230, 870, 58, '每轮开头：会话超时判定', ['g_tick 由 TIM3 每 1 ms 累加；与上次投递的差值 ≥ 4999（约 5 秒）即复位会话'], 'warn');
  b += arrow(200, 170, 200, 230);
  b += arrow(790, 170, 790, 230);

  b += box(70, 344, 260, 86, 'reset_session  0x584', ['g_tick = 0、清帧状态', 'g_main_state = 0、清接收命令'], 'normal');
  b += arrow(200, 288, 200, 344);
  b += poly([[330, 387], [40, 387], [40, 118], [70, 118]], '');
  b += `<text x="34" y="300" font-family="${FONT}" font-size="11" fill="#5a5a5a" transform="rotate(-90 34 300)">复位后回到空闲</text>`;

  b += box(640, 344, 300, 86, '未授权分支（仅显示）', ['licence_check 0x1A86 返回 0 时', '开机叠三行提示，功能不受限'], 'normal');
  b += caption(70, 456, '两个状态的切换点只有握手帧；其余命令不改变状态。超时判定在每轮循环开头执行，与当前状态无关。');
  fs.writeFileSync(path.join(OUT, 'states.svg'), svg(W, H, b, '状态机'));
}

// ---------------------------------------------------------------- 3. command tree
{
  const W = 1060, H = 700;
  let b = '';
  b += `<text x="30" y="34" font-family="${FONT}" font-size="17" font-weight="700" fill="#111">命令分派结构</text>`;
  b += box(400, 56, 260, 82, '6 字节帧', ['[class, sub, p2, p3, p4, p5]', '大端、无校验、无序号'], 'accent');
  b += box(400, 178, 260, 58, 'cmd_dispatch  0x250', ['按 class 分派'], 'accent');
  b += arrow(530, 138, 530, 178);

  const leaves = [
    { x: 20, title: 'class 0  系统', lines: ['0x4D  握手回显', '0x30  SFR 子操作：', '  查表 / 设滤波系数', '  写 0x20000140+addr', '  空操作'] },
    { x: 236, title: 'class 2  LCD', lines: ['0  设置坐标', '1  设置尺寸', '2  设置颜色/页', '3  绘图操作码 0x00–0x0B', '4  设置填充单元'] },
    { x: 452, title: 'class 3  Flash', lines: ['0  按 24 位地址读页', '1  写页', '2  擦除范围', '3  写页（第二实现）'] },
    { x: 668, title: 'class 8  ADC', lines: ['p1 = 通道号', '9 = 触摸，15 = 心跳', '回包 [08, ch, ?, ?, hi, lo]'] },
    { x: 884, title: 'class 4  BULK', lines: ['不进 cmd_dispatch', '由 main_loop 直接处理', 'tile[p1*4] = p2..p5'] , tone: 'warn'},
  ];
  for (const L of leaves) {
    b += box(L.x, 300, 156, 132, L.title, L.lines, L.tone || 'normal');
    b += arrow(530, 236, L.x + 78, 300);
  }

  b += box(20, 480, 500, 176, 'class 2 / sub 3 的操作码（p2）', [
    '0x00–0x06  内建绘图：纯色填充、贴图、单色图、混色图、横线',
    '0x07       装载绘图地址（上位机 DRAW_LOAD_ADDR）',
    '0x08       送出 len = (p3<<8)|p4 字节（DRAW_DATA）',
    '0x0A       设置显示方向并重新初始化屏（DRAW_DIRECTION）',
    '0x09       结束流式（固件内建，上位机未使用）',
    '0x0B       带底色贴图',
  ]);
  b += box(540, 480, 500, 176, '协议层缺陷（详见 ANALYSIS.md）', [
    'len == 0 时按循环次数使用，导致永久等待',
    '擦除长度 16 位累加，P + N > 0xFFFF 会回绕',
    'class 0 / sub 0x30 / 子操作 4 可写 0x20000140 + addr',
    'SET_REPORT 控制传输同样落到该分支，无长度校验',
    '无重传与序号，丢掉一字节后全部帧错位',
  ], 'warn');
  b += caption(20, 686, '注：地址均为固件内地址；上位机侧的帧构造见 Open_MSU2_MINI/docs/PROTOCOL.md。');
  fs.writeFileSync(path.join(OUT, 'commands.svg'), svg(W, H, b, '命令分派'));
}

// ---------------------------------------------------------------- 4. USB data path
{
  const W = 1020, H = 640;
  let b = '';
  b += `<text x="30" y="34" font-family="${FONT}" font-size="17" font-weight="700" fill="#111">USB 数据通路</text>`;
  b += box(30, 70, 190, 70, 'USB 主机', ['CDC-ACM 或 HID', '64 字节包'], 'normal');
  b += box(280, 70, 230, 70, 'EP2 OUT', ['UEP2_DMA = 槽地址', '中断触发即搬运'], 'accent');
  b += box(570, 56, 240, 104, '端点缓冲  0x20000BE4', ['16 个槽 × 64 字节', '槽索引 0–15，满后回卷'], 'normal');
  b += box(570, 210, 240, 104, '长度表  0x20000BEA + idx*2', ['ISR 写入低/高字节', '生产者索引 0x20000BE6'], 'normal');
  b += box(280, 210, 230, 104, '环形队列  0x20000FE4', ['144 字节 = 24 × 6 字节', '头/尾指针按 6 递增'], 'normal');
  b += box(30, 210, 190, 104, 'main_loop 取帧', ['按 6 字节步长前进', '组装 6 字节帧'], 'accent');
  b += box(30, 380, 190, 70, 'cmd_dispatch', ['按 class 分派'], 'accent');
  b += box(280, 380, 230, 70, '响应缓冲  0x200000C4', ['固定 6 字节'], 'normal');
  b += box(570, 380, 240, 70, 'EP3 IN', ['usb_ep_send(3, buf, 6, 1)'], 'accent');
  b += box(870, 380, 120, 70, 'USB 主机', [''], 'normal');

  b += arrow(220, 105, 280, 105);
  b += arrow(510, 105, 570, 105);
  b += arrow(690, 160, 690, 210);
  b += arrow(570, 262, 510, 262);
  b += arrow(280, 262, 220, 262);
  b += arrow(125, 314, 125, 380);
  b += arrow(220, 415, 280, 415);
  b += arrow(510, 415, 570, 415);
  b += arrow(810, 415, 870, 415);

  b += box(30, 490, 460, 130, '控制传输（EP0）', [
    'SETUP 包 → usb_ep_reset / 描述符查询',
    'SET_REPORT 落到 class 0 分支：',
    '  目标地址 = 0x20000140 + 主机给出的 addr',
    '无长度、无地址边界校验 —— 可写穿 20 KB SRAM',
  ], 'warn');
  b += box(520, 490, 470, 130, '中断与时钟', [
    'USBFS_IRQHandler 0x1110：一次搬运一个槽',
    '搬运完成后才更新长度表与生产者索引',
    '主循环与 ISR 不共享锁：索引更新顺序即同步手段',
    'TIM3 1 kHz 仅用于 5 秒会话超时，不参与数据搬运',
  ]);
  b += caption(30, 636, '注：0x20000BE4 / 0x20000FE4 由 gp 相对地址换算得出（gp = 0x20000880）。');
  fs.writeFileSync(path.join(OUT, 'usb-path.svg'), svg(W, H, b, 'USB 数据通路'));
}

console.log('wrote 4 diagrams to ' + OUT);