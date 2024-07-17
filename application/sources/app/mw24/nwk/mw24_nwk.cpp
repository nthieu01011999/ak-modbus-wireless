#include "mw24_nwk.h"
#include "mw24_eeprom.h"

/*************************************************************************************/
/* Variable and Struct Declaration */
/*************************************************************************************/
#define MW24_NRF_MAX_RETRANMIT		  ((1 << HAL_NRF_MAX_RT))
#define MW24_NRF_SEND_DONE			  ((1 << HAL_NRF_TX_DS))
#define MW24_NRF_PACKET_RECEIVED	  ((1 << HAL_NRF_RX_DR))
#define MW24_NRF_ACK_PAYLOAD_RECIEVED ((1 << HAL_NRF_RX_DR) | (1 << HAL_NRF_TX_DS))

/* Mode check wireless */
uint16_t mw24_check_wireless_msg_couter = 0;
bool mw24_i_enable_check_wireless = false;
bool mw24_i_am_master = false;

/* nrf state handware */
static uint8_t mw24_nrf_handware_state = MW24_NRF_HANDWARE_STATE_NONE;
static uint8_t mw24_get_nrf_handware_state();
static void mw24_set_nrf_handware_state(uint8_t state);

/* nrf state send */
static uint8_t mw24_nrf_send_state = MW24_NRF_SEND_STATE_IDLE;
static uint8_t mw24_get_nrf_send_state();
static void mw24_set_nrf_send_state(uint8_t state);

/* nrf state receive*/
static uint8_t mw24_nrf_recv_state = MW24_NRF_RECV_STATE_IDLE;
static uint8_t mw24_get_nrf_recv_state();
static void mw24_set_nrf_recv_state(uint8_t state);

/*************************************************************************************/
/* Action timer:
	1) mw24_end_sending: Update status and mode switch Rx*/
/*************************************************************************************/
void mw24_end_sending() {
	if (mw24_get_nrf_send_state() == MW24_NRF_SEND_STATE_SENDING) {
		mw24_set_nrf_send_state(MW24_NRF_SEND_STATE_IDLE);
		mw24_rx_nrf_mode_switch();
	}
	
	if (mw24_get_nrf_recv_state() == MW24_NRF_RECV_STATE_RECEIVING) {
		mw24_set_nrf_recv_state(MW24_NRF_SEND_STATE_IDLE);
		mw24_io_rs485_dir_low();
	}
}

/*************************************************************************************/
/* Action nRF24l01:
	1) mw24_tx_nrf_mode_switch: Switches the device to transmit (Tx) mode in nRF24l01.
	2) mw24_rx_nrf_mode_switch: Switches the device to receive (Rx) mode in nRF24l01.
	3) mw24_nrf_send_data_byte: Writes data to the transmit (Tx) register in nRF24l01.
	4) mw24_send_data_uart_to_nrf: Transfers data directly from UART to in nRF24l01.
	5) mw24_send_data_check_wireless: Sends data to check the wireless connectivity.
*/
/*************************************************************************************/
void mw24_tx_nrf_mode_switch() {
	CE_LOW();
	hal_nrf_set_operation_mode(HAL_NRF_PTX);
	sys_ctrl_delay_us(150);
	CE_HIGH();
}

void mw24_rx_nrf_mode_switch() {
	CE_LOW();
	hal_nrf_set_address(HAL_NRF_PIPE0, (uint8_t*)mw24_get_address_nrf()); /* Sets recieving address on pipe0 */
	hal_nrf_set_operation_mode(HAL_NRF_PRX);
	sys_ctrl_delay_us(150);
	CE_HIGH();
}

void mw24_nrf_send_data_byte(uint8_t* data, uint8_t len) {

	MW24_DEBUG_DATA("Data_send: [%d]\n", *data);

	/* off rf24 */
	CSN_LOW();
	/* Write command */
	hal_nrf_rw(WR_TX_PLOAD);
	/* Write tx_payload*/
	while (len--)
	{
		hal_nrf_rw(*data++);
	}
	/* On rf24 */
	CSN_HIGH();
}

void mw24_send_data_to_rf24_tx(uint8_t data) {
	if (mw24_get_nrf_handware_state() == MW24_NRF_HANDWARE_STATE_NONE) {
		return;
	}

	MW24_DEBUG_TX_ON();

	if (mw24_get_nrf_send_state() == MW24_NRF_SEND_STATE_IDLE) {
		mw24_tx_nrf_mode_switch();

		/* Enable timer change mode */
		mw24_timer_reset();
		mw24_timer_enable();
	}

	/* Clear data Tx */
	hal_nrf_flush_tx();

	/* Send data rf24*/
	mw24_nrf_send_data_byte((uint8_t*)&data, sizeof(data));

	/* update status rf24 */
	mw24_set_nrf_send_state(MW24_NRF_SEND_STATE_SENDING);

	MW24_DEBUG_TX_OFF();
}

void mw24_send_data_check_wireless() {
	uint8_t data = 7;
	if (mw24_get_nrf_handware_state() == MW24_NRF_HANDWARE_STATE_NONE) {
		return;
	}
	
	if (mw24_get_nrf_send_state() == MW24_NRF_SEND_STATE_IDLE) {
		mw24_tx_nrf_mode_switch();

		/* Enable timer change mode */
		mw24_timer_reset();
		mw24_timer_enable();
	}

	/* Clear data Tx */
	hal_nrf_flush_tx();

	/* Send data rf24*/
	mw24_nrf_send_data_byte((uint8_t*)&data, sizeof(data));

	/* update status rf24 */
	mw24_set_nrf_send_state(MW24_NRF_SEND_STATE_SENDING);
}

/*************************************************************************************/
/* Interrup nRF24l01 */
/*************************************************************************************/
void mw24_irq_nrf() {
	if (mw24_get_nrf_handware_state() == MW24_NRF_HANDWARE_STATE_NONE) {
		return;
	}

	uint8_t nrf24_irq_mask = hal_nrf_get_clear_irq_flags();
	MW24_DEBUG_FLAGS("RF24_flags: [%d]", nrf24_irq_mask);
	
	switch (nrf24_irq_mask) {
	case MW24_NRF_MAX_RETRANMIT: {
		MW24_DEBUG_STATUS("RF24_state_send: max retranmit\n");
		hal_nrf_flush_tx(); // clear tx
		mw24_rx_nrf_mode_switch(); // chuyen che do
	}
		break;

	case MW24_NRF_SEND_DONE: { /* send done */
		MW24_DEBUG_STATUS("RF24_state_send: send done\n");
		MW24_DEBUG_TX_ON();
		mw24_timer_reset(); /* reset timer change mode rf24 */
		MW24_DEBUG_TX_OFF();
	}
		break;

	case MW24_NRF_PACKET_RECEIVED: /* Packet received */
		MW24_DEBUG_STATUS("RF24_state_received: data_rf24\n");
		MW24_DEBUG_RX_ON();
		/* test connect */
		if (mw24_i_enable_check_wireless) {
			if (mw24_i_am_master == false) {
				mw24_check_wireless_msg_couter++;
			}
		}
		
		if (!hal_nrf_rx_fifo_empty()) {
			/* Receive data */
			uint8_t pl_len;
			volatile uint8_t k;
			pl_len = hal_nrf_read_rx_pload((uint8_t*)&k);

			if (pl_len != MW24_FRAME_DATA_RF24_PAYLOAD_LEN) {
				FATAL("RF24", 0x05);
			}

			MW24_DEBUG_RX_OFF();

			/* Clear data rx*/
			hal_nrf_flush_rx();

			MW24_DEBUG_RX_ON();

			/* Transformation mode rs485 -> Tx */
			// mw24_tx_rs485_uart2_enable();
			if (mw24_get_nrf_recv_state() == MW24_NRF_RECV_STATE_IDLE) {
				mw24_timer_reset();
				mw24_io_rs485_dir_high();
				mw24_timer_enable();
				mw24_set_nrf_recv_state(MW24_NRF_RECV_STATE_RECEIVING);
			}
			
			mw24_timer_reset();
			USART_SendData(USART2, k);

			MW24_DEBUG_RX_OFF();
		}
		break;

	case MW24_NRF_ACK_PAYLOAD_RECIEVED: { /* Ack payload recieved */
	}
		break;

	default: {
		hal_nrf_get_clear_irq_flags();
	}
		break;
	}
}

/*************************************************************************************/
/* Init nRF24l01 */
/*************************************************************************************/
void mw24_nrf_init() {
	/* init io control of nrf24 (CE, NCS, IRQ) */
	mw24_io_nrf_ctrl_init();
	/* init SPI1 */
	mw24_spi_nrf_ctrl_init();

	CE_LOW();
	sys_ctrl_delay_ms(100);

	hal_nrf_set_power_mode(HAL_NRF_PWR_DOWN);
	hal_nrf_get_clear_irq_flags();

	hal_nrf_close_pipe(HAL_NRF_ALL); /* First close all radio pipes, Pipe 0 and 1 open by default */
	hal_nrf_open_pipe(HAL_NRF_PIPE0, false); /* Open pipe0, without/autoack (autoack) */

	hal_nrf_set_crc_mode(HAL_NRF_CRC_8BIT); /* Operates in 16bits CRC mode */
	// hal_nrf_set_auto_retr(2, 750); /* Enable auto retransmit */

	hal_nrf_set_address_width(HAL_NRF_AW_5BYTES); /* 5 bytes address width */
	hal_nrf_set_address(HAL_NRF_TX, (uint8_t*)mw24_get_address_nrf()); /* Set device's addresses */
	hal_nrf_set_address(HAL_NRF_PIPE0, (uint8_t*)mw24_get_address_nrf()); /* Sets recieving address on pipe0 */

	hal_nrf_set_operation_mode(HAL_NRF_PRX);
	hal_nrf_set_rx_pload_width((uint8_t)HAL_NRF_PIPE0, MW24_FRAME_DATA_RF24_PAYLOAD_LEN);

	hal_nrf_set_rf_channel(mw24_get_channel_nrf());
	hal_nrf_set_output_power(HAL_NRF_0DBM);
	hal_nrf_set_lna_gain(HAL_NRF_LNA_HCURR);
	hal_nrf_set_datarate(HAL_NRF_2MBPS);

	hal_nrf_set_power_mode(HAL_NRF_PWR_UP); /* Power up device */

	hal_nrf_set_irq_mode(HAL_NRF_MAX_RT, true);
	hal_nrf_set_irq_mode(HAL_NRF_TX_DS, true);
	hal_nrf_set_irq_mode(HAL_NRF_RX_DR, true);

	hal_nrf_flush_rx();
	hal_nrf_flush_tx();

	sys_ctrl_delay_ms(2);
	CE_HIGH();

	ENTRY_CRITICAL();
	mw24_set_nrf_handware_state(MW24_NRF_HANDWARE_STATE_STARTED);
	mw24_set_nrf_send_state(MW24_NRF_SEND_STATE_IDLE);
	EXIT_CRITICAL();
}

/*************************************************************************************/
/* Status nRF24l01 */
/*************************************************************************************/
/* Status RF24 */
uint8_t mw24_get_nrf_handware_state() {
	return mw24_nrf_handware_state;
}

uint8_t mw24_get_nrf_send_state() {
	return mw24_nrf_send_state;
}

uint8_t mw24_get_nrf_recv_state() {
	return mw24_nrf_recv_state;
}

void mw24_set_nrf_handware_state(uint8_t state) {
	mw24_nrf_handware_state = state;
	MW24_DEBUG_STATUS("RF24_state_handware: [%d]\n", state);
}

void mw24_set_nrf_send_state(uint8_t state) {
	mw24_nrf_send_state = state;
	MW24_DEBUG_STATUS("RF24_state_send: [%d]\n", state);
}

void mw24_set_nrf_recv_state(uint8_t state) {
	mw24_nrf_recv_state = state;
	MW24_DEBUG_STATUS("RF24_state_recv: [%d]\n", state);
}
