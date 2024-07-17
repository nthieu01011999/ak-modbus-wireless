#ifndef __MW24_NWK_H__
#define __MW24_NWK_H__

/***********************************************************************************/
/* Library */
/***********************************************************************************/
#include <stdbool.h>
#include <stdint.h>

/* App */
#include "app.h"
#include "app_dbg.h"
#include "app_data.h"

/* task */
#include "task_list.h"

/* Sources ak*/
#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

/* System debug - interrup - io */
#include "sys_dbg.h"
#include "sys_irq.h"
#include "sys_io.h"

/* Buffer data */
#include "fifo.h"
#include "ring_buffer.h"

/* Hardware Abstraction Layer */
#include "hal_nrf.h"
#include "hal_nrf_hw.h"
#include "hal_nrf_reg.h"

/* Modbus wireless */
#include "io_cfg.h"
#include "mw24_io_cfg.h"
#include "mw24_timer.h"
#include "mw24_config.h"
#include "mw24_debug.h"

/***********************************************************************************/
/* Parameters */
/***********************************************************************************/
#define MW24_FRAME_DATA_MODBUS_MAX_LEN		(256)
#define MW24_FRAME_DATA_RF24_PAYLOAD_LEN	(1)
#define MW24_BUFFER_LENGHT					(256)

extern bool mw24_i_am_master;
extern bool mw24_i_enable_check_wireless;
extern uint16_t mw24_check_wireless_msg_couter;

/***********************************************************************************/
/* Status function */
/***********************************************************************************/
enum mw24_nrf_handware_state_e {
	MW24_NRF_HANDWARE_STATE_NONE,
	MW24_NRF_HANDWARE_STATE_STARTED,
};

enum mw24_nrf_send_state_e {
	MW24_NRF_SEND_STATE_IDLE,
	MW24_NRF_SEND_STATE_SENDING,
};

enum mw24_nrf_recv_state_e {
	MW24_NRF_RECV_STATE_IDLE,
	MW24_NRF_RECV_STATE_RECEIVING,
};

/***********************************************************************************/
/* nRF24l01 */
/***********************************************************************************/
/* init nRF24l01 */
extern void mw24_nrf_init();

/* action nRF24l01 */
extern void mw24_tx_nrf_mode_switch();
extern void mw24_rx_nrf_mode_switch();
extern void mw24_nrf_send_data_byte(uint8_t* data, uint8_t len);

#endif //__MW24_NWK_H__
