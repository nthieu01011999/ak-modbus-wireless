#include "scr_check_connect.h"

/*****************************************************************************/
/* System Functionality Overview - Check Connect */
/*****************************************************************************/
/* 	** I'm Master
	Principle: The device assumes the role of the Master in the system and emits
	RF24 signals every 1 second in the surrounding area.

	** I'm Slave
	Principle: The device assumes the role of the Slave in the system. Whenever 
	it receives a signal from the Master, it counts the counter and updates the 
	status. 
*/

/*****************************************************************************/
/* Variable and Struct Declaration - Check Connect */
/*****************************************************************************/

struct scr_check_connect_parameter_t {
	uint8_t address_check;
	uint8_t channel_check;
};


uint8_t msg_run;
uint16_t couter_status;
static scr_check_connect_parameter_t check_cn_option;

enum status_check_connect_e {
	CC_STATUS_OK,
	CC_STATUS_NA,
	CC_STATUS_SEND,
	CC_STATUS_RECV,
};

static status_check_connect_e cc_status = CC_STATUS_NA;
static status_check_connect_e get_check_connect_status();
static void set_check_connect_status(uint16_t couter);

/*****************************************************************************/
/* View - Check Connect */
/*****************************************************************************/
static void view_scr_check_connect();

view_dynamic_t dyn_view_check_connect = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_check_connect
};

view_screen_t scr_check_connect = {
	&dyn_view_check_connect,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_check_connect() {
	/* Frame */
	view_render.drawRect(0,	0, 		128, 	64, 	WHITE);
	view_render.drawFastHLine(1, 	52, 	126, 	WHITE);
	view_render.drawFastHLine(1, 	12, 	126, 	WHITE);
	view_render.drawFastHLine(1, 	32, 	68, 	WHITE);
	view_render.drawFastVLine(68, 	12, 	40, 	WHITE);

	/* Title */
	view_render.setCursor(25,	3);
	view_render.print("CHECK WIRELESS");

	/* Text */
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(3, 	54);
	view_render.print("ADDR:   |CHANNEL: ");
	view_render.setCursor(35, 	54);
	view_render.print(check_cn_option.address_check);
	view_render.setCursor(105, 	54);
	view_render.print(check_cn_option.channel_check);

	/* Couter */
	view_render.setCursor(3, 19);
	view_render.print("Couter:");
	view_render.setCursor(45, 19);
	view_render.print(mw24_check_wireless_msg_couter);

	/* Status connect */
	view_render.setCursor(3, 38);
	view_render.print("Status:");

	switch (get_check_connect_status()) {
	case CC_STATUS_SEND:
		view_render.setCursor(43, 38);
		view_render.print("Send");
		break;

	case CC_STATUS_OK:
		view_render.setCursor(45, 38);
		view_render.print("OK");
		break;

	case CC_STATUS_NA:
	default:
		view_render.setCursor(45, 38);
		view_render.print("N/A");
		break;
	}

	/* Status system */
	view_render.setCursor(72, 33);
	view_render.print("I'm");
	view_render.setCursor(72, 42);
	if (mw24_i_am_master == 1) {
		view_render.print("Master");
	}
	else {
		view_render.print("Slave");
	}
	
	/* Animation */
	if (get_check_connect_status() != CC_STATUS_NA) {
		view_render.drawBitmap(78 + msg_run*2, 20 + msg_run, message_2_icon, 8, 5, WHITE);
	}
	else {
		view_render.setCursor(97, 25);
		view_render.print("x");
	}

	view_render.fillRect(70, 15, 15, 15, BLACK);
	view_render.fillRect(111, 35, 15, 15, BLACK);
	
	view_render.drawBitmap(70, 15, ak_master_icon, 15, 15, WHITE);
	view_render.drawBitmap(111, 35, ak_slave_icon, 15, 15, WHITE);

	view_render.setCursor(115, 15);
	view_render.print("RF");
}

/*****************************************************************************/
/* Handle - Check Connect */
/*****************************************************************************/
void scr_check_connect_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		view_render.initialize();

		mw24_i_enable_check_wireless = true;
		mw24_check_wireless_msg_couter = 0;
		
		check_cn_option.address_check = mw24_get_number_address_nrf();
		check_cn_option.channel_check = mw24_get_channel_nrf();

		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_LETTER, 200, TIMER_PERIODIC);
	}
		break;

	case AC_DISPLAY_SHOW_LETTER: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_LETTER\n");
		// animation and send message
		if (msg_run > 15) {msg_run = 0;}
		msg_run++;

		if (msg_run == 1) {
			set_check_connect_status(mw24_check_wireless_msg_couter);
		}

		if (msg_run == 1) {
			if (mw24_i_am_master) {
				mw24_tx_nrf_mode_switch();
				hal_nrf_flush_tx();
				volatile uint8_t c = 1;
				mw24_nrf_send_data_byte((uint8_t*)&c, sizeof(c));
			}
		}

		if (mw24_check_wireless_msg_couter > 999) {
			mw24_check_wireless_msg_couter = 0;
		}

	}
		break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		mw24_check_wireless_msg_couter = 0;
		mw24_i_enable_check_wireless = false;
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_LETTER);
		SCREEN_TRAN(scr_menu_handle, &scr_menu);
	}
		break;

	case AC_DISPLAY_BUTTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
	}
		break;

	case AC_DISPLAY_BUTTON_DOWN_LONG_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_LONG_RELEASED\n");
		if (mw24_i_am_master == true) {
			mw24_i_am_master = false;
		} 
		else {mw24_i_am_master = true;}
	}
		break;

	default:
		break;
	}
}

status_check_connect_e get_check_connect_status() {
	return cc_status;
}

void set_check_connect_status(uint16_t couter) {
	if (mw24_i_am_master) {
		cc_status = CC_STATUS_SEND;
	}
	else {
		if (couter != couter_status) {
			cc_status = CC_STATUS_OK;
			couter_status = couter;
			BUZZER_PlayTones(tones_1beep);
		}
		else {
			cc_status = CC_STATUS_NA;
		}
	}
}