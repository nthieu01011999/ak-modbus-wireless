#include "scr_inform.h"

static uint32_t inform_baudrate = 0;
static mw24_data_cfg_t inform_option;

static void view_scr_inform();

view_dynamic_t dyn_view_inform = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	 },
	view_scr_inform
};

view_screen_t scr_inform = {
	&dyn_view_inform,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_inform() {
	view_render.clear();
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(45, 4);
	view_render.print("NETWORK");
	view_render.drawFastHLine(0, 12, 128, WHITE);

	/* Sensor Inform */
	view_render.setCursor(3, 17);
	view_render.print("RF Address:");
	view_render.setCursor(75, 17);
	view_render.print(inform_option.address);
	view_render.setCursor(3, 29);
	view_render.print("RF Channel:");
	view_render.setCursor(75, 29);
	view_render.print(inform_option.channel);
	view_render.setCursor(3, 41);
	view_render.print("Baudrate:");
	view_render.setCursor(75, 41);
	view_render.print(inform_baudrate);

	/* text bar */
	view_render.fillRect(0, 52, 128, 11, BLACK);
	view_render.drawRect(0, 0, 128, 64, WHITE);
	view_render.drawFastHLine(1, 52, 126, WHITE);

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(22, 55);
	view_render.print("Version 1.1.3");
}

void read_inform_eeprom() {
	inform_baudrate		  = mw24_get_baudrate_modbus();
	inform_option.address = mw24_get_number_address_nrf();
	inform_option.channel = mw24_get_channel_nrf();
}

void scr_inform_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		view_render.initialize();
		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);

		read_inform_eeprom();
	} break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		SCREEN_TRAN(scr_menu_handle, &scr_menu);
	} break;

	case AC_DISPLAY_BUTTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		SCREEN_TRAN(scr_menu_handle, &scr_menu);
	} break;

	case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		SCREEN_TRAN(scr_menu_handle, &scr_menu);
	} break;

	case AC_DISPLAY_SHOW_IDLE: {
		APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
		SCREEN_TRAN(scr_idle_handle, &scr_idle);
	} break;

	default:
		break;
	}
}
