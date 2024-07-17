#include "mw24_io_cfg.h"
#include "mw24_eeprom.h"

/******************************************************************************
* nfr24l01 IO function
*******************************************************************************/
void mw24_io_nrf_ctrl_init() {
	/* CE / CSN / IRQ */
	GPIO_InitTypeDef        GPIO_InitStructure;
	EXTI_InitTypeDef        EXTI_InitStruct;
	NVIC_InitTypeDef        NVIC_InitStruct;

	/* GPIOA Periph clock enable */
	RCC_AHBPeriphClockCmd(NRF_CE_IO_CLOCK, ENABLE);
	RCC_AHBPeriphClockCmd(NRF_CSN_IO_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/*CE -> PA8*/
	GPIO_InitStructure.GPIO_Pin = NRF_CE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(NRF_CE_IO_PORT, &GPIO_InitStructure);

	/*CNS -> PB9*/
	GPIO_InitStructure.GPIO_Pin = NRF_CSN_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(NRF_CSN_IO_PORT, &GPIO_InitStructure);

	/* IRQ -> PB1 */
	GPIO_InitStructure.GPIO_Pin = NRF_IRQ_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(NRF_IRQ_IO_PORT, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/*****************************************************************************
 *SPI nRF24l01
******************************************************************************/
void mw24_spi_nrf_ctrl_init() {
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;

	/*!< SPI GPIO Periph clock enable */
	RCC_AHBPeriphClockCmd(SPI_SCK_IO_CLOCK, ENABLE);

	/*!< SPI Periph clock enable */
	RCC_APB2PeriphClockCmd(SPI_NRF_CLOCK, ENABLE);

	/*!< Configure SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(SPI_SCK_IO_PORT, &GPIO_InitStructure);

	/*!< Configure SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_IO_PIN;
	GPIO_Init(SPI_MISO_IO_PORT, &GPIO_InitStructure);

	/*!< Configure SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_IO_PIN;
	GPIO_Init(SPI_MOSI_IO_PORT, &GPIO_InitStructure);

	/* Connect PXx to SPI_SCK */
	GPIO_PinAFConfig(SPI_SCK_IO_PORT, SPI_SCK_IO_SOURCE, SPI_NRF_GPIO);

	/* Connect PXx to SPI_MISO */
	GPIO_PinAFConfig(SPI_MISO_IO_PORT, SPI_MISO_IO_SOURCE, SPI_NRF_GPIO);

	/* Connect PXx to SPI_MOSI */
	GPIO_PinAFConfig(SPI_MOSI_IO_PORT, SPI_MOSI_IO_SOURCE, SPI_NRF_GPIO);

	/*!< SPI Config */
	SPI_DeInit(SPI_NRF);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_NRF, &SPI_InitStructure);

	SPI_Cmd(SPI_NRF, ENABLE); /*!< SPI enable */
}

void mw24_ce_nrf_low() {
	GPIO_ResetBits(NRF_CE_IO_PORT, NRF_CE_IO_PIN);
}

void mw24_ce_nrf_high() {
	GPIO_SetBits(NRF_CE_IO_PORT, NRF_CE_IO_PIN);
}

void mw24_csn_nrf_low() {
	GPIO_ResetBits(NRF_CSN_IO_PORT, NRF_CSN_IO_PIN);
}

void mw24_csn_nrf_high() {
	GPIO_SetBits(NRF_CSN_IO_PORT, NRF_CSN_IO_PIN);
}

uint8_t mw24_spi_nrf_rw(uint8_t data) {
	unsigned long rxtxData = data;
	uint32_t counter;

	/* waiting send idle then send data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI_NRF, SPI_I2S_FLAG_TXE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x01);
		}
	}

	SPI_I2S_SendData(SPI_NRF, (uint8_t)rxtxData);

	/* waiting conplete rev data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI_NRF, SPI_I2S_FLAG_RXNE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x02);
		}
	}

	rxtxData = (uint8_t)SPI_I2S_ReceiveData(SPI_NRF);

	return (uint8_t)rxtxData;
}

/*****************************************************************************
 *io uart for rs485
******************************************************************************/
void mw24_io_uart_rs485_cfg() {
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(USART_RS485_TX_GPIO_CLK | USART_RS485_RX_GPIO_CLK, ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(USART_RS485_CLK, ENABLE);

	/* Connect PXx to USART2_Tx */
	GPIO_PinAFConfig(USART_RS485_TX_GPIO_PORT, USART_RS485_TX_SOURCE, USART_RS485_TX_AF);

	/* Connect PXx to USART2_Rx */
	GPIO_PinAFConfig(USART_RS485_RX_GPIO_PORT, USART_RS485_RX_SOURCE, USART_RS485_RX_AF);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin		= USART_RS485_TX_PIN;
	GPIO_Init(USART_RS485_TX_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin		= USART_RS485_RX_PIN;
	GPIO_Init(USART_RS485_RX_GPIO_PORT, &GPIO_InitStructure);

	/* NVIC configuration */
	/* Configure the Priority Group to 4 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = USART_RS485_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void mw24_io_rs485_dir_mode_output() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RS485_DIR_IO_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin		= RS485_DIR_IO_PIN;
	GPIO_Init(RS485_DIR_IO_PORT, &GPIO_InitStructure);
}

void mw24_rs485_uart2_init() {

	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStructure;

	ENTRY_CRITICAL();
	/* GPIO - Uart2 */
	mw24_io_uart_rs485_cfg();
	
	/* GPIO - rs485 tx/rx */
	mw24_io_rs485_dir_mode_output();

	/* rs485 rx */
	mw24_io_rs485_dir_low();

	/* USART2 configuration */
	USART_InitStructure.USART_BaudRate = mw24_get_baudrate_modbus();
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_RS485, &USART_InitStructure);

	/* Default other settings. */
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART_RS485, &USART_ClockInitStructure);

	USART_Cmd( USART_RS485, ENABLE );

	USART_ITConfig( USART_RS485, USART_IT_RXNE, ENABLE );
	USART_ITConfig( USART_RS485, USART_IT_TXE, DISABLE );

	EXIT_CRITICAL();
}

void mw24_tx_rs485_uart2_enable() {
	mw24_io_rs485_dir_high();
	USART_ITConfig( USART_RS485, USART_IT_TXE, ENABLE );
}

void mw24_tx_rs485_uart2_disable() {
	mw24_io_rs485_dir_low();
}

void mw24_io_rs485_dir_low() {
	GPIO_ResetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN);
}

void mw24_io_rs485_dir_high() {
	GPIO_SetBits(RS485_DIR_IO_PORT, RS485_DIR_IO_PIN);
}
