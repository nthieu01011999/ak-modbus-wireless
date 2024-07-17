#include "scr_setting.h"

/*****************************************************************************/
/* Variable and Struct Declaration - Setting game */
/*****************************************************************************/
// Screen
#define STEP_SETTING_CHOSSE			   (16)
#define NUMBER_SETTING_ITEMS		   (3)
#define SCREEN_SETTING_H			   (46)

#define OPTION_SETTING_BAUDRATE		   (0)
#define OPTION_SETTING_ADDRESS		   (1)
#define OPTION_SETTING_CHANNEL		   (2)

#define SETTING_ITEMS_ICON_COLOR()                                     \
	do {                                                               \
		setting_items_icon_color[0] = !setting_chosse.items.is_item_1; \
		setting_items_icon_color[1] = !setting_chosse.items.is_item_2; \
		setting_items_icon_color[2] = !setting_chosse.items.is_item_3; \
	} while (0);

struct setting_items {
	// setting items
	unsigned int is_item_1 : 1;
	unsigned int is_item_2 : 1;
	unsigned int is_item_3 : 1;
};

// Setting items name
static char setting_items_name[NUMBER_SETTING_ITEMS][20] = {
	" Baudrate     ",	 // item 1
	" Address      ",	 // item 2
	" Channel      ",	 // item 3
};

// Setting items color
uint8_t setting_items_icon_color[NUMBER_SETTING_ITEMS];

// Screen and item location on the Setting
typedef struct {
	int screen;
	int location;
} screen_t;

// Setting items ID
union scr_setting_t {
	uint32_t _id = 1;	 // if items >> => uint8_t -> uint16_t ->uint32_t
	setting_items items;
};

// Scroll bar
typedef struct {
	uint8_t axis_x = 126;
	uint8_t axis_y = 18;
	uint8_t size_W = 3;
	uint8_t size_h = SCREEN_SETTING_H / NUMBER_SETTING_ITEMS;
} scr_setting_scroll_bar_t;

// Frames
typedef struct {
	uint8_t axis_x = 0;
	uint8_t axis_y = 0;
	uint8_t size_w = 80;
	uint8_t size_h = 15;
	uint8_t size_r = 3;
} scr_setting_frames_t;

screen_t screen_setting;
scr_setting_t setting_chosse;
scr_setting_scroll_bar_t setting_scroll_bar;
scr_setting_frames_t frame_white_setting;
scr_setting_frames_t frame_setting[3];

uint16_t setting_data_reboot[NUMBER_SETTING_ITEMS];

mw24_data_cfg_t setting;

bool is_force_option = 1;
uint8_t option[NUMBER_SETTING_ITEMS];
static bool reboot;
/*****************************************************************************/
/* View - Setting game */
/*****************************************************************************/
static void view_scr_setting();

view_dynamic_t dyn_view_setting = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	 },
	view_scr_setting
};

view_screen_t scr_setting = {
	&dyn_view_setting,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_setting() {
#define AR_GAME_SETTING_ICON_AXIS_X	  (7)
#define AR_GAME_SETTING_TEXT_AXIS_X_1 (3)
#define AR_GAME_SETTING_TEXT_AXIS_X_2 (84)
	if (reboot) {
		view_render.clear();
		view_render.setTextSize(1);
		view_render.setTextColor(WHITE);
		view_render.setCursor(25, 25);
		view_render.print("Loading.....");
	}
	else {
		// Scroll bar
		view_render.fillRect(setting_scroll_bar.axis_x - 1, setting_scroll_bar.axis_y + 18, setting_scroll_bar.size_W, setting_scroll_bar.size_h, WHITE);
		view_render.drawBitmap(setting_scroll_bar.axis_x, 18, dot_icon, 1, SCREEN_SETTING_H, WHITE);

		// Frame_setting White
		if (is_force_option) {
			view_render.fillRoundRect(frame_white_setting.axis_x, frame_white_setting.axis_y, frame_white_setting.size_w, frame_white_setting.size_h, frame_white_setting.size_r,
									  WHITE);
		}
		else {
			view_render.fillRoundRect(81, frame_white_setting.axis_y, 40, frame_white_setting.size_h, frame_white_setting.size_r, WHITE);
		}

		/* Text */
		view_render.setTextSize(1);
		view_render.setTextColor(WHITE);
		view_render.setCursor(45, 3);
		view_render.print("SETTING");

		view_render.drawRect(0, 0, 128, 15, WHITE);

		for (uint8_t i = 0; i < 3; i++) {
			// Frames
			view_render.drawRect(frame_setting[i].axis_x, frame_setting[i].axis_y, frame_setting[i].size_w, frame_setting[i].size_h, WHITE);

			view_render.drawRect(81, frame_setting[i].axis_y, 40, frame_setting[i].size_h, WHITE);

			// Text Setting
			if (is_force_option) {
				view_render.setTextColor(setting_items_icon_color[screen_setting.screen + i]);
			}
			else {
				view_render.setTextColor(WHITE);
			}
			view_render.setCursor(AR_GAME_SETTING_TEXT_AXIS_X_1, frame_setting[i].axis_y + 5);
			view_render.print(setting_items_name[screen_setting.screen + i]);

			if (!is_force_option) {
				view_render.setTextColor(setting_items_icon_color[screen_setting.screen + i]);
			}
			else {
				view_render.setTextColor(WHITE);
			}
			view_render.setCursor(AR_GAME_SETTING_TEXT_AXIS_X_2, frame_setting[i].axis_y + 5);
			
			if (i == OPTION_SETTING_BAUDRATE) {
				view_render.print((int)mw24_modbus_baudrate_array[option[i]]);
			}
			else if (i == OPTION_SETTING_ADDRESS) {
				view_render.print(option[i]);
			}
			else if (i == OPTION_SETTING_CHANNEL) {
				view_render.print(option[i]);
			}
		}
	}
}

/*****************************************************************************/
/* Handle - Setting game */
/*****************************************************************************/
void update_setting_screen_chosse() {
	// Frames location
	frame_setting[0].axis_y	   = 16;
	frame_setting[1].axis_y	   = 32;
	frame_setting[2].axis_y	   = 48;
	frame_white_setting.axis_y = frame_setting[screen_setting.location - screen_setting.screen].axis_y;
	// update color setting
	setting_chosse._id = 1 << screen_setting.location;
	SETTING_ITEMS_ICON_COLOR();
	// update scroll bar
	setting_scroll_bar.axis_y = (SCREEN_SETTING_H * screen_setting.location / NUMBER_SETTING_ITEMS);
}

void read_setting_option_eeprom() {
	option[0] = (uint8_t)mw24_get_number_baudrate_modbus();
	option[1] = (uint8_t)mw24_get_number_address_nrf();
	option[2] = (uint8_t)mw24_get_channel_nrf();
}

void scr_setting_handle(ak_msg_t *msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		view_render.initialize();

		read_setting_option_eeprom();

		update_setting_screen_chosse();
	} break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		if (is_force_option) {
			is_force_option = 0;
		}
		else {
			is_force_option = 1;
		}
	} break;

	case AC_DISPLAY_BUTTON_MODE_LONG_PRESSED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_LONG_PRESSED\n");
		reboot = true;

		setting.number_baudrate = option[0];
		setting.address	 = option[1];
		setting.channel	 = option[2];

		mw24_eeprom_write_data_cfg(&setting);
		mw24_eeprom_read_data_cfg(&setting);
		mw24_set_data_cfg(&setting);

		mw24_nrf_init();
		mw24_rs485_uart2_init();

		timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SETTING_UPDATE, AC_DISPLAY_LOADING_SETTING_INTERVAL, TIMER_ONE_SHOT);
	} break;

	case AC_DISPLAY_SETTING_UPDATE: {
		APP_DBG_SIG("AC_DISPLAY_SETTING_UPDATE\n");
		reboot = false;
		SCREEN_TRAN(scr_menu_handle, &scr_menu);
	} break;

	case AC_DISPLAY_BUTTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		if (is_force_option) {
			if (screen_setting.location > 0) {
				screen_setting.location--;
			}
			if (frame_white_setting.axis_y == frame_setting[0].axis_y) {
				if (screen_setting.screen > 0) {
					screen_setting.screen--;
				}
			}
			else if (frame_white_setting.axis_y == frame_setting[1].axis_y) {
				frame_white_setting.axis_y = frame_setting[0].axis_y;
			}
			else if (frame_white_setting.axis_y == frame_setting[2].axis_y) {
				frame_white_setting.axis_y = frame_setting[1].axis_y;
			}
			update_setting_screen_chosse();
		}
		else {
			if (screen_setting.location == OPTION_SETTING_BAUDRATE) {
				if (option[OPTION_SETTING_BAUDRATE] >= MW24_NUMBER_BAUDRATE_MODBUS_MAX) {
					option[OPTION_SETTING_BAUDRATE] = MW24_NUMBER_BAUDRATE_MODBUS_MIN;
				}
				else {
					option[OPTION_SETTING_BAUDRATE]++;
				}
			}
			else if (screen_setting.location == OPTION_SETTING_ADDRESS) {
				if (option[OPTION_SETTING_ADDRESS] >= MW24_ADDRESS_RF24_MAX) {
					option[OPTION_SETTING_ADDRESS] = MW24_ADDRESS_RF24_MIN;
				}
				else 
				{
					option[OPTION_SETTING_ADDRESS]++;
				}
			}
			else if (screen_setting.location == OPTION_SETTING_CHANNEL) {
				if (option[OPTION_SETTING_CHANNEL] >= MW24_CHANNEL_RF24_MAX) {
					option[OPTION_SETTING_CHANNEL] = MW24_CHANNEL_RF24_MIN;
				}
				else 
				{
					option[OPTION_SETTING_CHANNEL]++;
				}
			}
		}
	} break;

	case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		if (is_force_option) {
			if (screen_setting.location < NUMBER_SETTING_ITEMS - 1) {
				screen_setting.location++;
			}
			if (frame_white_setting.axis_y == frame_setting[0].axis_y) {
				frame_white_setting.axis_y = frame_setting[1].axis_y;
			}
			else if (frame_white_setting.axis_y == frame_setting[1].axis_y) {
				frame_white_setting.axis_y = frame_setting[2].axis_y;
			}
			else if (frame_white_setting.axis_y == frame_setting[2].axis_y) {
				if (screen_setting.screen < NUMBER_SETTING_ITEMS - 3) {
					screen_setting.screen++;
				}
			}
			update_setting_screen_chosse();
		}
		else {
			if (screen_setting.location == OPTION_SETTING_BAUDRATE) {
				if (option[OPTION_SETTING_BAUDRATE] <= MW24_NUMBER_BAUDRATE_MODBUS_MIN) {
					option[OPTION_SETTING_BAUDRATE] = MW24_NUMBER_BAUDRATE_MODBUS_MAX;
				}
				else {
					option[OPTION_SETTING_BAUDRATE]--;
				}
			}
			else if (screen_setting.location == OPTION_SETTING_ADDRESS) {
				if (option[OPTION_SETTING_ADDRESS] <= MW24_ADDRESS_RF24_MIN) {
					option[OPTION_SETTING_ADDRESS] = MW24_ADDRESS_RF24_MAX;
				}
				else 
				{
					option[OPTION_SETTING_ADDRESS]--;
				}
			}
			else if (screen_setting.location == OPTION_SETTING_CHANNEL) {
				if (option[OPTION_SETTING_CHANNEL] <= MW24_CHANNEL_RF24_MIN) {
					option[OPTION_SETTING_CHANNEL] = MW24_CHANNEL_RF24_MAX;
				}
				else 
				{
					option[OPTION_SETTING_CHANNEL]--;
				}
			}
		}
	} break;

	case AC_DISPLAY_BUTTON_UP_LONG_PRESSED: {
		option[OPTION_SETTING_BAUDRATE] = MW24_NUMBER_BAUDRATE_MODBUS_MAX;
		option[OPTION_SETTING_ADDRESS] = MW24_ADDRESS_RF24_MAX;
		option[OPTION_SETTING_CHANNEL] = MW24_CHANNEL_RF24_MAX;
	} break;

	case AC_DISPLAY_BUTTON_DOWN_LONG_RELEASED: {
		option[OPTION_SETTING_BAUDRATE] = MW24_NUMBER_BAUDRATE_MODBUS_MIN;
		option[OPTION_SETTING_ADDRESS] = MW24_ADDRESS_RF24_MIN;
		option[OPTION_SETTING_CHANNEL] = MW24_CHANNEL_RF24_MIN;
	} break;

	default:
		break;
	}
}
