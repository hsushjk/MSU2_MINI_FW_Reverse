# Reconstructed source index

| module | file | functions |
|---|---|---|
| boot | `boot.c` | 9 |
| cmd | `cmd.c` | 3 |
| crt | `crt.c` | 1 |
| draw | `draw.c` | 17 |
| flash | `flash.c` | 22 |
| hal | `hal.c` | 43 |
| lcd | `lcd.c` | 8 |
| lib | `lib.c` | 8 |
| main | `main.c` | 1 |
| printf | `printf.c` | 37 |
| uart | `uart.c` | 7 |
| usb | `usb.c` | 9 |

## Function index

| address | name | module | note |
|---|---|---|---|
| 0x0000 | reset_entry | crt | jump to _start |
| 0x0100 | __riscv_save_13 | lib | -msave-restore prologue helper |
| 0x0108 | __riscv_save_12 | lib | -msave-restore prologue helper |
| 0x0116 | __riscv_save_8 | lib | -msave-restore prologue helper |
| 0x0130 | __riscv_save_3 | lib | -msave-restore prologue helper |
| 0x013C | __riscv_restore_1 | lib | -msave-restore epilogue helper |
| 0x0160 | memset | lib | byte fill |
| 0x0208 | lut_get | lib | 8-bit lookup table accessor |
| 0x0238 | memcpy6 | lib | fixed 6-byte copy |
| 0x0250 | cmd_dispatch | cmd | host command dispatcher (6-byte record) |
| 0x0584 | reset_session | cmd | reset tick/state/ring pointers |
| 0x059A | main_loop | cmd | command + streaming loop |
| 0x0A90 | board_clocks_enable | uart | RCC AHB/APB clock enables for the board |
| 0x0ABC | tim3_init | uart | TIM3 1 kHz tick setup |
| 0x0B34 | uart2_gpio_init | uart | USART2 pins |
| 0x0C3A | usb_ep_flags_clear | uart | clear per-endpoint busy flags |
| 0x0CC8 | uart_dma_rx_setup | uart | DMA1 CH6/CH7 for USART2 |
| 0x0D74 | uart_dma_reconfig | uart |  |
| 0x0DEA | usb_uart_reinit | uart | UART reconfiguration called from the USB ISR |
| 0x0E48 | vec_hardfault | usb | infinite loop |
| 0x0E4C | TIM3_IRQHandler | usb | tick counter |
| 0x0E7C | board_gpio_init | usb | GPIO clock + mode setup |
| 0x0E94 | usb_ep_init | usb | endpoint MOD/DMA setup |
| 0x0F14 | usb_ep_reset | usb |  |
| 0x0F56 | usb_device_init | usb | USBFS clock, D+ pull-up, interrupts |
| 0x0FD2 | usb_ep_send | usb | write len/payload to an endpoint and arm it |
| 0x1110 | USBFS_IRQHandler | usb | USB device interrupt handler |
| 0x1A66 | flash_uid_to_record | flash | cache the 128-bit UID in the licence record |
| 0x1A86 | licence_check | flash | 16-pair arithmetic check, destroys the record on failure |
| 0x1B10 | spi_xfer_byte | flash | blocking 8-bit SPI transfer (returns 0 on timeout) |
| 0x1B66 | flash_read_uid | flash | 0x4B |
| 0x1BC6 | spi1_gpio_init | flash | SPI1 + CS + D/C pins (first copy) |
| 0x1CFC | spi1_gpio_init2 | flash | SPI1 + CS + D/C pins (second copy) |
| 0x1E32 | flash_read_status | flash | 0x05 |
| 0x1E6C | flash_wait_busy | flash | poll BUSY, no timeout |
| 0x1E7A | flash_write_enable | flash | 0x06 |
| 0x1EA8 | flash_read_id | flash | 0x90 |
| 0x1F0A | flash_read_data | flash | 0x03 |
| 0x1F80 | flash_read_page | flash | 0x03 by page number |
| 0x1FF6 | flash_read_security | flash | 0x48 |
| 0x2060 | flash_fast_read | flash | 0x0B + DMA |
| 0x20F8 | flash_fast_read_dma | flash | 0x0B + DMA, overlapping CPU expansion |
| 0x2202 | flash_page_program | flash | 0x06 + 0x81 + 0x02 |
| 0x2306 | flash_page_program2 | flash | 0x06 + 0x81 + 0x02 (second copy) |
| 0x23BE | flash_page_erase | flash | 0x06 + vendor 0x81 |
| 0x2412 | flash_sector_erase | flash | 0x06 + 0x20 |
| 0x2466 | flash_block_erase | flash | 0x06 + 0x52 |
| 0x24BA | flash_erase_page_n | flash |  |
| 0x250E | flash_erase_range | flash | page/sector/block planner |
| 0x2668 | lcd_cs_dc | lcd | LCD chip select and D/C |
| 0x26BE | lcd_write_cmd | lcd | command byte |
| 0x2714 | lcd_write_data8 | lcd | data byte |
| 0x2778 | lcd_init | lcd | ST7735S init sequence |
| 0x2A20 | lcd_set_window | lcd | CASET/RASET/RAMWR |
| 0x2A9C | lcd_blit_colour | lcd | RGB565 rectangle from flash page |
| 0x2BCC | adc_init | lcd | ADC1 channel setup |
| 0x2C58 | adc_read | lcd | single conversion on a channel, averaged |
| 0x2C9E | spi_dma_tx_init | draw | DMA1 CH3 (TX) -> SPI1 |
| 0x2CF4 | spi_dma_rx_init | draw | DMA1 CH2 (RX) <- SPI1 |
| 0x2D48 | dma_ring_init | draw | DMA1 CH8 memory-to-memory |
| 0x2DA2 | lcd_draw_glyph8 | draw | 8x8 text primitive |
| 0x2E42 | lcd_draw_text | draw | string with wrap |
| 0x2E80 | lcd_set_tile | draw | build the 256-byte pattern tile |
| 0x2EA4 | lcd_fill_rect | draw | window fill |
| 0x305A | lcd_draw_hline | draw |  |
| 0x30C6 | lcd_stream | draw | stream a host-length buffer through SPI DMA |
| 0x3148 | lcd_stream_end | draw | end of stream |
| 0x3180 | lcd_send_pixels | draw | raw pixel writer |
| 0x31C0 | lcd_blit_colour_bg | draw | colour blit with background |
| 0x33B8 | lcd_image_with_bg | draw | background image + foreground |
| 0x367C | lcd_mono_image_a | draw | 1-bit image expansion |
| 0x3698 | lcd_mono_image_b | draw | 1-bit image expansion |
| 0x36B8 | lcd_mono_image | draw | 1-bit image from a flash page |
| 0x37D4 | lcd_mono_image_mix | draw | 1-bit image over a background page |
| 0x39B4 | main | main | application entry |
| 0x3B32 | SystemInit | boot | HSION + flash wait state |
| 0x3B74 | SystemCoreClockUpdate | boot |  |
| 0x3BB4 | _start | boot | crt0: gp/sp, .data/.bss, CSRs, mret into main |
| 0x3C3E | ADC_Init | boot |  |
| 0x3C94 | ADC_Cmd | boot |  |
| 0x3CA4 | ADC_GetConversionValue | boot |  |
| 0x3D5C | sub_3D5C | boot |  |
| 0x3D66 | sub_3D66 | boot |  |
| 0x3D72 | GetChipID | boot | unique ID register |
| 0x3D7C | DMA_DeInit | hal |  |
| 0x3E1E | DMA_Init | hal | per-channel CFGR/CNTR/PADDR/MADDR |
| 0x3E56 | DMA_Cmd | hal |  |
| 0x3E6A | DMA_GetFlagStatus | hal |  |
| 0x3E78 | DMA_ClearFlag | hal |  |
| 0x3E80 | GPIO_Init | hal |  |
| 0x3FEE | GPIO_SetBits | hal |  |
| 0x3FFC | GPIO_WriteBit | hal |  |
| 0x4010 | write_once_flag | hal | write-only global at gp-0x77C |
| 0x4016 | PWR_SetPLS | hal |  |
| 0x4026 | PWR_GetStatus | hal |  |
| 0x4034 | pvd_probe | hal | brown-out/PVD strapping probe |
| 0x4064 | RCC_GetClocksFreq | hal | wrong clock model |
| 0x40A2 | RCC_AHBPeriphClockCmd | hal |  |
| 0x40C0 | RCC_APB2PeriphClockCmd | hal |  |
| 0x40DE | RCC_APB1PeriphClockCmd | hal |  |
| 0x40FC | RCC_APB2PeriphResetCmd | hal |  |
| 0x411A | RCC_APB1PeriphResetCmd | hal |  |
| 0x4138 | SPI1_CTLR1_merge | hal |  |
| 0x416A | SPI1_Cmd | hal |  |
| 0x4184 | SPI1_CTLR1_write | hal |  |
| 0x4196 | SPI1_SendData | hal |  |
| 0x419A | SPI1_ReceiveData | hal |  |
| 0x419E | SPI1_GetFlagStatus | hal |  |
| 0x41A8 | sub_41A8 | hal |  |
| 0x4202 | SPI1_I2SCFGR? | hal |  |
| 0x4258 | SPI1_GetFlagStatus2? | hal |  |
| 0x4270 | spi_clear_bit | hal |  |
| 0x4282 | spi_set_bit? | hal |  |
| 0x428E | void_428e | hal |  |
| 0x42A6 | void_42a6 | hal |  |
| 0x42B2 | USART_Init | hal | baud divisor from a broken PCLK1 |
| 0x4342 | USART_Cmd | hal |  |
| 0x4358 | USART_SendData8? | hal |  |
| 0x436A | USART_SendData | hal |  |
| 0x4372 | USART_GetFlagStatus | hal |  |
| 0x437C | USART_ClearFlag | hal |  |
| 0x4388 | Delay_Init | hal | loop calibration from SystemCoreClock |
| 0x43B2 | delay_cycles | hal |  |
| 0x43E8 | Delay_Ms | hal |  |
| 0x441E | USART2_Init | hal | 115200 8N1 |
| 0x447E | usart_poll_write | hal | blocking TXE poll per byte |
| 0x44BC | DMA_M2M_Copy | hal |  |
| 0x44E6 | sub_44E6 | usb |  |
| 0x4598 | printf | printf |  |
| 0x45D6 | sub_45D6 | printf |  |
| 0x463C | sub_463C | printf |  |
| 0x4646 | sub_4646 | printf |  |
| 0x468C | sub_468C | printf |  |
| 0x46F8 | sub_46F8 | printf |  |
| 0x4798 | sub_4798 | printf |  |
| 0x4802 | sub_4802 | printf |  |
| 0x48D6 | sub_48D6 | printf |  |
| 0x48FC | sub_48FC | printf |  |
| 0x493E | vfprintf | printf |  |
| 0x4BCA | printf_pad_field | printf |  |
| 0x4CD6 | printf_emit | printf |  |
| 0x4F78 | sub_4F78 | printf |  |
| 0x4FAA | sub_4FAA | printf |  |
| 0x501E | sub_501E | printf |  |
| 0x5054 | sub_5054 | printf |  |
| 0x5112 | _write_r | printf |  |
| 0x5148 | sub_5148 | printf |  |
| 0x5246 | sub_5246 | printf |  |
| 0x5276 | sub_5276 | printf |  |
| 0x53A6 | sub_53A6 | printf |  |
| 0x540E | sub_540E | printf |  |
| 0x5442 | sub_5442 | printf |  |
| 0x5498 | memcpy | printf |  |
| 0x552A | sub_552A | printf |  |
| 0x5542 | sub_5542 | printf |  |
| 0x5544 | sub_5544 | printf |  |
| 0x5546 | sub_5546 | printf |  |
| 0x55EE | strlen | printf |  |
| 0x5622 | strcmp | printf |  |
| 0x5654 | sub_5654 | printf |  |
| 0x5684 | sub_5684 | printf |  |
| 0x5694 | sub_5694 | printf |  |
| 0x56A4 | sub_56A4 | printf |  |
| 0x56B4 | sub_56B4 | printf |  |
| 0x56C4 | sub_56C4 | printf |  |
