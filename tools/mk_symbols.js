// mk_symbols.js -- build symbols.tsv from dump7/index.tsv + the NAME table below.
// module is chosen by address band, name by explicit table (else sub_XXXX).
const fs = require('fs');
const path = require('path');
const ROOT = __dirname;

const BANDS = [
  [0x0000, 0x0100, 'crt'],
  [0x0100, 0x0250, 'lib'],
  [0x0250, 0x0A90, 'cmd'],
  [0x0A90, 0x0E48, 'uart'],
  [0x0E48, 0x1110, 'usb'],
  [0x1110, 0x1A66, 'usb'],
  [0x1A66, 0x2668, 'flash'],
  [0x2668, 0x2C9E, 'lcd'],
  [0x2C9E, 0x39B4, 'draw'],
  [0x39B4, 0x3B32, 'main'],
  [0x3B32, 0x3D7C, 'boot'],
  [0x3D7C, 0x44E6, 'hal'],
  [0x44E6, 0x4598, 'usb'],
  [0x4598, 0x56C5, 'printf'],
];
const moduleOf = (a) => { for (const [lo, hi, m] of BANDS) if (a >= lo && a < hi) return m; return 'misc'; };

// addr, name, note
const NAMES = `
00000000 reset_entry|jump to _start
00000100 __riscv_save_13|-msave-restore prologue helper
00000108 __riscv_save_12|-msave-restore prologue helper
00000116 __riscv_save_8|-msave-restore prologue helper
00000130 __riscv_save_3|-msave-restore prologue helper
0000013C __riscv_restore_1|-msave-restore epilogue helper
00000140 __riscv_restore_5|-msave-restore epilogue helper
00000160 memset|byte fill
00000208 lut_get|8-bit lookup table accessor
00000238 memcpy6|fixed 6-byte copy
00000250 cmd_dispatch|host command dispatcher (6-byte record)
00000584 reset_session|reset tick/state/ring pointers
0000059A main_loop|command + streaming loop
00000A90 board_clocks_enable|RCC AHB/APB clock enables for the board
00000ABC tim3_init|TIM3 1 kHz tick setup
00000B34 uart2_gpio_init|USART2 pins
00000C3A usb_ep_flags_clear|clear per-endpoint busy flags
00000CC8 uart_dma_rx_setup|DMA1 CH6/CH7 for USART2
00000D74 uart_dma_reconfig|
00000DEA usb_uart_reinit|UART reconfiguration called from the USB ISR
00000E48 vec_hardfault|infinite loop
00000E4C TIM3_IRQHandler|tick counter
00000E7C board_gpio_init|GPIO clock + mode setup
00000E94 usb_ep_init|endpoint MOD/DMA setup
00000F14 usb_ep_reset|
00000F56 usb_device_init|USBFS clock, D+ pull-up, interrupts
00000FD2 usb_ep_send|write len/payload to an endpoint and arm it
00001110 USBFS_IRQHandler|USB device interrupt handler
00001A66 flash_uid_to_record|cache the 128-bit UID in the licence record
00001A86 licence_check|16-pair arithmetic check, destroys the record on failure
00001B10 spi_xfer_byte|blocking 8-bit SPI transfer (returns 0 on timeout)
00001B66 flash_read_uid|0x4B
00001BC6 spi1_gpio_init|SPI1 + CS + D/C pins (first copy)
00001CFC spi1_gpio_init2|SPI1 + CS + D/C pins (second copy)
00001E32 flash_read_status|0x05
00001E6C flash_wait_busy|poll BUSY, no timeout
00001E7A flash_write_enable|0x06
00001EA8 flash_read_id|0x90
00001F0A flash_read_data|0x03
00001F80 flash_read_page|0x03 by page number
00001FF6 flash_read_security|0x48
00002060 flash_fast_read|0x0B + DMA
000020F8 flash_fast_read_dma|0x0B + DMA, overlapping CPU expansion
00002202 flash_page_program|0x06 + 0x81 + 0x02
00002306 flash_page_program2|0x06 + 0x81 + 0x02 (second copy)
000023BE flash_page_erase|0x06 + vendor 0x81
00002412 flash_sector_erase|0x06 + 0x20
00002466 flash_block_erase|0x06 + 0x52
000024BA flash_erase_page_n|
0000250E flash_erase_range|page/sector/block planner
00002668 lcd_cs_dc|LCD chip select and D/C
000026BE lcd_write_cmd|command byte
00002714 lcd_write_data8|data byte
00002778 lcd_init|ST7735S init sequence
00002A20 lcd_set_window|CASET/RASET/RAMWR
00002A9C lcd_blit_colour|RGB565 rectangle from flash page
00002BCC adc_init|ADC1 channel setup
00002C58 adc_read|single conversion on a channel, averaged
00002C9E spi_dma_tx_init|DMA1 CH3 (TX) -> SPI1
00002CF4 spi_dma_rx_init|DMA1 CH2 (RX) <- SPI1
00002D48 dma_ring_init|DMA1 CH8 memory-to-memory
00002DA2 lcd_draw_glyph8|8x8 text primitive
00002E42 lcd_draw_text|string with wrap
00002E80 lcd_set_tile|build the 256-byte pattern tile
00002EA4 lcd_fill_rect|window fill
0000305A lcd_draw_hline|
000030C6 lcd_stream|stream a host-length buffer through SPI DMA
00003148 lcd_stream_end|end of stream
00003180 lcd_send_pixels|raw pixel writer
000031C0 lcd_blit_colour_bg|colour blit with background
000033B8 lcd_image_with_bg|background image + foreground
0000367C lcd_mono_image_a|1-bit image expansion
00003698 lcd_mono_image_b|1-bit image expansion
000036B8 lcd_mono_image|1-bit image from a flash page
000037D4 lcd_mono_image_mix|1-bit image over a background page
000039B4 main|application entry
00003B32 SystemInit|HSION + flash wait state
00003B74 SystemCoreClockUpdate|
00003BB4 _start|crt0: gp/sp, .data/.bss, CSRs, mret into main
00003C3E ADC_Init|
00003C94 ADC_Cmd|
00003CA4 ADC_GetConversionValue|
00003D72 GetChipID|unique ID register
00003D7C DMA_DeInit|
00003E1E DMA_Init|per-channel CFGR/CNTR/PADDR/MADDR
00003E56 DMA_Cmd|
00003E6A DMA_GetFlagStatus|
00003E78 DMA_ClearFlag|
00003E80 GPIO_Init|
00003FEE GPIO_SetBits|
00003FFC GPIO_WriteBit|
00004010 write_once_flag|write-only global at gp-0x77C
00004016 PWR_SetPLS|
00004026 PWR_GetStatus|
00004034 pvd_probe|brown-out/PVD strapping probe
00004064 RCC_GetClocksFreq|wrong clock model
000040A2 RCC_AHBPeriphClockCmd|
000040C0 RCC_APB2PeriphClockCmd|
000040DE RCC_APB1PeriphClockCmd|
000040FC RCC_APB2PeriphResetCmd|
0000411A RCC_APB1PeriphResetCmd|
00004138 SPI1_CTLR1_merge|
0000416A SPI1_Cmd|
00004184 SPI1_CTLR1_write|
00004196 SPI1_SendData|
0000419A SPI1_ReceiveData|
0000419E SPI1_GetFlagStatus|
00004202 SPI1_I2SCFGR?|
00004258 SPI1_GetFlagStatus2?|
00004270 spi_clear_bit|
00004282 spi_set_bit?|
0000428E void_428e|
000042A6 void_42a6|
000042B2 USART_Init|baud divisor from a broken PCLK1
00004342 USART_Cmd|
00004358 USART_SendData8?|
0000436A USART_SendData|
00004372 USART_GetFlagStatus|
0000437C USART_ClearFlag|
00004388 Delay_Init|loop calibration from SystemCoreClock
000043B2 delay_cycles|
000043E8 Delay_Ms|
0000441E USART2_Init|115200 8N1
0000447E usart_poll_write|blocking TXE poll per byte
000044BC DMA_M2M_Copy|
00004598 printf|
0000493E vfprintf|
00004BCA printf_pad_field|
00004CD6 printf_emit|
00004FD6 __swrite|FILE write method (stdout -> USART2)
00005112 _write_r|
00005498 memcpy|
000055EE strlen|
00005622 strcmp|
`;

const names = new Map();
for (const ln of NAMES.trim().split('\n')) {
  const m = ln.match(/^([0-9a-fA-F]{8})\s+(\S+)\|(.*)$/);
  if (!m) { console.error('bad name line: ' + ln); continue; }
  names.set(parseInt(m[1], 16), { name: m[2], note: m[3] });
}

const idx = fs.readFileSync(path.join(ROOT, 'dump7', 'index.tsv'), 'utf8').split(/\r?\n/);
const rows = [];
for (const ln of idx.slice(1)) {
  if (!ln.trim()) continue;
  const f = ln.split('\t');
  const addr = parseInt(f[0], 16);
  const nm = names.get(addr);
  const name = nm ? nm.name : 'sub_' + addr.toString(16).toUpperCase().padStart(4, '0');
  const note = nm ? nm.note : '';
  const insns = f[3] || '';
  rows.push([addr, name, moduleOf(addr), note, insns]);
}
rows.sort((a, b) => a[0] - b[0]);
const out = ['# addr\tname\tmodule\tnote'];
for (const [a, name, mod, note] of rows) {
  out.push(a.toString(16).toUpperCase().padStart(8, '0') + '\t' + name + '\t' + mod + '\t' + note);
}
fs.writeFileSync(path.join(ROOT, 'symbols.tsv'), out.join('\n') + '\n');
console.log('symbols: ' + rows.length);
const per = new Map();
for (const r of rows) per.set(r[2], (per.get(r[2]) || 0) + 1);
console.log([...per.entries()].map(([k, v]) => `${k}:${v}`).join(' '));