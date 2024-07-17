#ifndef __MW24_IO_CFG_H__
#define __MW24_IO_CFG_H__

#include "stdint.h"
#include <stdbool.h>
#include "io_cfg.h"
#include "platform.h"
#include "sys_dbg.h"
#include "sys_ctrl.h"
#include "app_dbg.h"
#include "eeprom.h"
#include "system.h"

/*****************************************************************************
 *Pin map nRF24l01
******************************************************************************/
#define NRF_CE_IO_PIN					(GPIO_Pin_8)
#define NRF_CE_IO_PORT					(GPIOA)
#define NRF_CE_IO_CLOCK					(RCC_AHBPeriph_GPIOA)

#define NRF_CSN_IO_PIN					(GPIO_Pin_9)
#define NRF_CSN_IO_PORT					(GPIOB)
#define NRF_CSN_IO_CLOCK				(RCC_AHBPeriph_GPIOB)

#define NRF_IRQ_IO_PIN					(GPIO_Pin_1)
#define NRF_IRQ_IO_PORT					(GPIOB)
#define NRF_IRQ_IO_CLOCK				(RCC_AHBPeriph_GPIOB)

#define NRF_TIM						    (TIM6)
#define NRF_TIM_PERIPH				    (RCC_APB1Periph_TIM6)
#define NRF_TIM_IRQ					    (TIM6_IRQn)

/*****************************************************************************
 *SPI nRF24l01
******************************************************************************/
#define SPI_SCK_IO_PIN                  (GPIO_Pin_5)
#define SPI_SCK_IO_PORT                 (GPIOA)
#define SPI_SCK_IO_CLOCK                (RCC_AHBPeriph_GPIOA)
#define SPI_SCK_IO_SOURCE               (GPIO_PinSource5)

#define SPI_MISO_IO_PIN                 (GPIO_Pin_6)
#define SPI_MISO_IO_PORT                (GPIOA)
#define SPI_MISO_IO_CLOCK               (RCC_AHBPeriph_GPIOA)
#define SPI_MISO_IO_SOURCE              (GPIO_PinSource6)

#define SPI_MOSI_IO_PIN                 (GPIO_Pin_7)
#define SPI_MOSI_IO_PORT                (GPIOA)
#define SPI_MOSI_IO_CLOCK               (RCC_AHBPeriph_GPIOA)
#define SPI_MOSI_IO_SOURCE              (GPIO_PinSource7)

#define SPI_NRF                        (SPI1)
#define SPI_NRF_GPIO                   (GPIO_AF_SPI1)
#define SPI_NRF_CLOCK                  (RCC_APB2Periph_SPI1)

/****************************************************************************
 *UART RS485 - RS485 dir io config
*****************************************************************************/
#define USART_RS485						(USART2)
#define USART_RS485_CLK					(RCC_APB1Periph_USART2)
#define USART_RS485_IRQn				(USART2_IRQn)

#define USART_RS485_TX_PIN				(GPIO_Pin_3)
#define USART_RS485_TX_GPIO_PORT		(GPIOA)
#define USART_RS485_TX_GPIO_CLK			(RCC_AHBPeriph_GPIOA)
#define USART_RS485_TX_SOURCE			(GPIO_PinSource3)
#define USART_RS485_TX_AF				(GPIO_AF_USART2)

#define USART_RS485_RX_PIN				(GPIO_Pin_2)
#define USART_RS485_RX_GPIO_PORT		(GPIOA)
#define USART_RS485_RX_GPIO_CLK			(RCC_AHBPeriph_GPIOA)
#define USART_RS485_RX_SOURCE			(GPIO_PinSource2)
#define USART_RS485_RX_AF				(GPIO_AF_USART2)

#define RS485_TIM						(TIM4)
#define RS485_TIM_PERIPH				(RCC_APB1Periph_TIM4)
#define RS485_TIM_IRQ					(TIM4_IRQn)

/*RS485 dir IO*/
#define RS485_DIR_IO_PIN				(GPIO_Pin_1)
#define RS485_DIR_IO_PORT				(GPIOA)
#define RS485_DIR_IO_CLOCK				(RCC_AHBPeriph_GPIOA)

/*****************************************************************************
 *io uart for rs485-modbusRTU
******************************************************************************/
extern void mw24_io_uart_rs485_cfg();
extern void mw24_io_rs485_dir_mode_output();
extern void mw24_io_rs485_dir_low();
extern void mw24_io_rs485_dir_high();

extern void mw24_rs485_uart2_init();
extern void mw24_tx_rs485_uart2_enable();
extern void mw24_tx_rs485_uart2_disable();

#endif //__MW24_IO_CFG_H__
