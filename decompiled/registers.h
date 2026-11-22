/* Peripheral registers referenced by the code (names from the WCH ch32x035.h layouts). */
#ifndef MSU2_REGISTERS_H
#define MSU2_REGISTERS_H

typedef struct { volatile unsigned int r[0x8000]; } periph_t;

#define USART2_CTLR1  (*(volatile uint32_t*)0x4000440C) /* USART2->CTLR1 */
#define AWU_CSR       (*(volatile uint32_t*)0x40007000) /* AWU->CSR */
#define AWU_WR        (*(volatile uint32_t*)0x40007004) /* AWU->WR */
#define AFIO_CTLR     (*(volatile uint32_t*)0x40010018) /* AFIO->CTLR */
#define ADC1_CTLR1    (*(volatile uint32_t*)0x40012404) /* ADC1->CTLR1 */
#define ADC1_IDATAR1  (*(volatile uint32_t*)0x4001243C) /* ADC1->IDATAR1 */
#define ADC1_RDATAR   (*(volatile uint32_t*)0x4001244C) /* ADC1->RDATAR */
#define DMA1_INTFR    (*(volatile uint32_t*)0x40020000) /* DMA1->INTFR */
#define DMA1_INTFCR   (*(volatile uint32_t*)0x40020004) /* DMA1->INTFCR */
#define DMA1_CH2_CFGR (*(volatile uint32_t*)0x4002001C) /* DMA1_CH2->CFGR */
#define DMA1_CH2_CNTR (*(volatile uint32_t*)0x40020020) /* DMA1_CH2->CNTR */
#define DMA1_CH2_MADDR (*(volatile uint32_t*)0x40020028) /* DMA1_CH2->MADDR */
#define DMA1_CH3_CFGR (*(volatile uint32_t*)0x40020030) /* DMA1_CH3->CFGR */
#define DMA1_CH3_CNTR (*(volatile uint32_t*)0x40020034) /* DMA1_CH3->CNTR */
#define DMA1_CH3_MADDR (*(volatile uint32_t*)0x4002003C) /* DMA1_CH3->MADDR */
#define DMA1_CH8_CNTR (*(volatile uint32_t*)0x40020098) /* DMA1_CH8->CNTR */
#define DMA1_CH8_PADDR (*(volatile uint32_t*)0x4002009C) /* DMA1_CH8->PADDR */
#define DMA1_CH8_MADDR (*(volatile uint32_t*)0x400200A0) /* DMA1_CH8->MADDR */
#define RCC_CTLR      (*(volatile uint32_t*)0x40021000) /* RCC->CTLR */
#define RCC_CFGR0     (*(volatile uint32_t*)0x40021004) /* RCC->CFGR0 */
#define RCC_APB2PRSTR (*(volatile uint32_t*)0x4002100C) /* RCC->APB2PRSTR */
#define RCC_APB1PRSTR (*(volatile uint32_t*)0x40021010) /* RCC->APB1PRSTR */
#define RCC_AHBPCENR  (*(volatile uint32_t*)0x40021014) /* RCC->AHBPCENR */
#define RCC_APB2PCENR (*(volatile uint32_t*)0x40021018) /* RCC->APB2PCENR */
#define RCC_APB1PCENR (*(volatile uint32_t*)0x4002101C) /* RCC->APB1PCENR */
#define FLASH_R_ACTLR (*(volatile uint32_t*)0x40022000) /* FLASH_R->ACTLR */
#define USBFSD_BASE_CTRL (*(volatile uint32_t*)0x40023400) /* USBFSD->BASE_CTRL */
#define USBFSD_UDEV_CTRL (*(volatile uint32_t*)0x40023401) /* USBFSD->UDEV_CTRL */
#define USBFSD_INT_EN (*(volatile uint32_t*)0x40023402) /* USBFSD->INT_EN */
#define USBFSD_DEV_ADDR (*(volatile uint32_t*)0x40023403) /* USBFSD->DEV_ADDR */
#define USBFSD_MIS_ST (*(volatile uint32_t*)0x40023405) /* USBFSD->MIS_ST */
#define USBFSD_INT_FG (*(volatile uint32_t*)0x40023406) /* USBFSD->INT_FG */
#define USBFSD_INT_ST (*(volatile uint32_t*)0x40023407) /* USBFSD->INT_ST */
#define USBFSD_RX_LEN (*(volatile uint32_t*)0x40023408) /* USBFSD->RX_LEN */
#define USBFSD_UEP4_1_MOD (*(volatile uint32_t*)0x4002340C) /* USBFSD->UEP4_1_MOD */
#define USBFSD_UEP2_3_MOD (*(volatile uint32_t*)0x4002340D) /* USBFSD->UEP2_3_MOD */
#define USBFSD_UEP567_MOD (*(volatile uint32_t*)0x4002340E) /* USBFSD->UEP567_MOD */
#define USBFSD_UEP0_DMA (*(volatile uint32_t*)0x40023410) /* USBFSD->UEP0_DMA */
#define USBFSD_UEP1_DMA (*(volatile uint32_t*)0x40023414) /* USBFSD->UEP1_DMA */
#define USBFSD_UEP2_DMA (*(volatile uint32_t*)0x40023418) /* USBFSD->UEP2_DMA */
#define USBFSD_UEP3_DMA (*(volatile uint32_t*)0x4002341C) /* USBFSD->UEP3_DMA */
#define USBFSD_UEP0_CTRL (*(volatile uint32_t*)0x40023420) /* USBFSD->UEP0_CTRL */
#define USBFSD_UEP0_CTRL_H (*(volatile uint32_t*)0x40023422) /* USBFSD->UEP0_CTRL_H */
#define USBFSD_UEP1_CTRL (*(volatile uint32_t*)0x40023424) /* USBFSD->UEP1_CTRL */
#define USBFSD_UEP1_CTRL_H (*(volatile uint32_t*)0x40023426) /* USBFSD->UEP1_CTRL_H */
#define USBFSD_UEP2_CTRL_H (*(volatile uint32_t*)0x4002342A) /* USBFSD->UEP2_CTRL_H */
#define USBFSD_UEP3_CTRL (*(volatile uint32_t*)0x4002342C) /* USBFSD->UEP3_CTRL */
#define USBFSD_UEP3_CTRL_H (*(volatile uint32_t*)0x4002342E) /* USBFSD->UEP3_CTRL_H */
#define USBFSD_UEP4_CTRL (*(volatile uint32_t*)0x40023430) /* USBFSD->UEP4_CTRL */
#define USBFSD_UEP4_CTRL_H (*(volatile uint32_t*)0x40023432) /* USBFSD->UEP4_CTRL_H */
#define NVIC_ISER[1]  (*(volatile uint32_t*)0xE000E104) /* NVIC->ISER[1] */
#define NVIC_ICER[1]  (*(volatile uint32_t*)0xE000E184) /* NVIC->ICER[1] */

#endif
