#include "scr_menu.h"
/*****************************************************************************/
/* Instructions for adding new items to the menu*/
/*****************************************************************************/
/* ** To quickly navigate, press "Ctrl + F" and search by keywords **
	Step 1: Prepare icons for the item to be added with a size of 13-18 px*px.
	Step 2: Add an item to the menu:
		- Increase the number of items: increase "NUMBER_MENU_ITEMS" by 1.
		- Add an identifier: increase "NUMBER_MENU_ITEMS" by 1 item.
		- Add color formatting: increase "MENU_ITEMS_ICON_COLOR" by 1 item.
	Step 3: Add names for the item to "menu_items_name[]".
	Step 4: Add icons for the item:
		- Add icon: menu_items_icon[]
		- Add size: menu_items_icon_size_w[], menu_items_icon_size_h[]
	Step 5: Add an address for the item:
		- Add an item to "screen_tran_menu()" and enter the desired address to navigate to.
	If the icon is not aligned, adjust it by using "menu_items_icon_axis_y[3]".
*/

/*****************************************************************************/
/* Variable and Struct Declaration - Menu game */
/*****************************************************************************/
// Screen 
#define STEP_MENU_CHOSSE				(22)
#define NUMBER_MENU_ITEMS				(3)
#define	SCREEN_MENU_H					(64)

#define MENU_ITEMS_ICON_COLOR() \
do { \
	menu_items_icon_color[0]	= !menu_chosse.items.is_item_1; \
	menu_items_icon_color[1]	= !menu_chosse.items.is_item_2; \
	menu_items_icon_color[2]	= !menu_chosse.items.is_item_3; \
} while(0);

struct menu_items{
// menu items
	unsigned int is_item_1 : 1;
	unsigned int is_item_2 : 1;
	unsigned int is_item_3 : 1;
};

// Menu items name
static char menu_items_name[NUMBER_MENU_ITEMS][20] = {
	"  NETWORK       ",		// item 1
	"  SETTING       ",		// item 2
	"  CHECK WIRELESS",		// item 3
};

// Menu items icon
static const uint8_t *menu_items_icon[NUMBER_MENU_ITEMS] = {
	inform_icon,				// item 1
	setting_icon,				// item 2
	check_connect_icon,			// item 3
};

// Menu items size W
uint8_t menu_items_icon_size_w[NUMBER_MENU_ITEMS] = {
	15,							// item 1
	16,							// item 2
	15,							// item 3
};

uint8_t menu_items_icon_size_h[NUMBER_MENU_ITEMS] = {
	15,							// item 1
	16,							// item 2
	15,							// item 3
};

// Menu items color
uint8_t menu_items_icon_color[NUMBER_MENU_ITEMS];
// Menu items axis Y
uint8_t menu_items_icon_axis_y[3] = {
	2,							// icon frame 1
	24,							// icon frame 2
	46							// icon frame 3
};

// Screen and item location on the Menu
typedef struct {
	int screen;
	int location;
} screen_t;

// Menu items ID
union scr_menu_t {
	uint32_t _id = 1;		// if items >> => uint8_t -> uint16_t ->uint32_t
	menu_items items;
};

// Scroll bar
typedef struct {
	uint8_t axis_x = 126;
	uint8_t axis_y = 0;
	uint8_t size_W = 3;
	uint8_t size_h = SCREEN_MENU_H / NUMBER_MENU_ITEMS;
} scr_menu_scroll_bar_t;

// Frames
typedef struct {
	uint8_t axis_x = 0;
	uint8_t axis_y = 0;
	uint8_t size_w = 123;
	uint8_t size_h = 20;
	uint8_t size_r = 3;
} scr_menu_frames_t;

screen_t screen_menu;
scr_menu_t menu_chosse;
scr_menu_scroll_bar_t scroll_bar;
scr_menu_frames_t frame_white;
scr_menu_frames_t frame[3];

/*****************************************************************************/
/* View - Menu game */
/*****************************************************************************/
static void view_scr_menu();

view_dynamic_t dyn_view_menu = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_menu
};

view_screen_t scr_menu = {
	&dyn_view_menu,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_menu() {
#define AR_GAME_MENU_ICON_AXIS_X			(7)
#define AR_GAME_MENU_TEXT_AXIS_X			(20)
	// Scroll bar
	view_render.fillRect(	scroll_bar.axis_x - 1, \
							scroll_bar.axis_y, \
							scroll_bar.size_W, \
							scroll_bar.size_h, \
							WHITE);
	view_render.drawBitmap(	scroll_bar.axis_x, \
							0, \
							dot_icon, \
							1, \
							SCREEN_MENU_H, \
							WHITE);
	// Frame White
	view_render.fillRect(	frame_white.axis_x, \
								frame_white.axis_y, \
								frame_white.size_w, \
								frame_white.size_h, \
								WHITE);
	for (uint8_t i = 0; i < 3; i++) {
	// Frames
		view_render.drawRect(	frame[i].axis_x, \
									frame[i].axis_y, \
									frame[i].size_w, \
									frame[i].size_h, \
									WHITE);
	// Icon
		view_render.drawBitmap(	AR_GAME_MENU_ICON_AXIS_X, \
								menu_items_icon_axis_y[i], \
								menu_items_icon[screen_menu.screen + i], \
								menu_items_icon_size_w[screen_menu.screen + i], \
								menu_items_icon_size_h[screen_menu.screen + i], \
								menu_items_icon_color[screen_menu.screen + i]);
	}
	// Text Menu
	view_render.setTextSize(1);
	for (uint8_t i = 0; i < 3; i++) {
		view_render.setTextColor(menu_items_icon_color[screen_menu.screen + i]);
		view_render.setCursor(AR_GAME_MENU_TEXT_AXIS_X, menu_items_icon_axis_y[i]+5);
		view_render.print(menu_items_name[screen_menu.screen + i]);
	}
}

/*****************************************************************************/
/* Handle - Menu game */
/*****************************************************************************/
void update_menu_screen_chosse() {
	// Frames location
	frame_white.axis_y =frame[screen_menu.location-screen_menu.screen].axis_y;
	frame[0].axis_y = 0;
	frame[1].axis_y = 22;
	frame[2].axis_y = 44;
	// update color menu
	menu_chosse._id = 1<<screen_menu.location;
	MENU_ITEMS_ICON_COLOR();
	// update scroll bar
	scroll_bar.axis_y = (SCREEN_MENU_H*screen_menu.location / NUMBER_MENU_ITEMS);
}

void screen_tran_menu() {
	switch (screen_menu.location) {
	case 0:	// item 2
		SCREEN_TRAN(scr_inform_handle,			&scr_inform);
		break;
	case 1:	// item 3
		SCREEN_TRAN(scr_setting_handle,			&scr_setting);
		break;
	case 2: // item 4
		SCREEN_TRAN(scr_check_connect_handle,	&scr_check_connect);
		break;
	
	default:
		break;
	}
}

void scr_menu_handle(ak_msg_t* msg) {
	switch (msg->sig) {
	case SCREEN_ENTRY: {
		APP_DBG_SIG("SCREEN_ENTRY\n");
		view_render.initialize();
		view_render_display_on();
		update_menu_screen_chosse();
		// timer switch to scr_idle
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_IDLE, \
					AC_DISPLAY_IDLE_INTERVAL, \
					TIMER_ONE_SHOT);
	}
		break;

	case AC_DISPLAY_SHOW_IDLE: {
        // timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
		SCREEN_TRAN(scr_idle_handle,&scr_idle);
	}
		break;

	case AC_DISPLAY_BUTTON_MODE_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_MODE_RELEASED\n");
		screen_tran_menu();
	}
		break;

	case AC_DISPLAY_BUTTON_UP_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_UP_RELEASED\n");
		if (screen_menu.location > 0) {
			screen_menu.location--;
		}
		if (frame_white.axis_y == frame[0].axis_y) {
			if (screen_menu.screen > 0) {
				screen_menu.screen--;
			}
		}
		else if (frame_white.axis_y == frame[1].axis_y) {
			frame_white.axis_y = frame[0].axis_y;
		}
		else if (frame_white.axis_y == frame[2].axis_y) {
			frame_white.axis_y = frame[1].axis_y;
		}
		update_menu_screen_chosse();
		// Reset timer switch to scr_idle
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_IDLE, \
					AC_DISPLAY_IDLE_INTERVAL, \
					TIMER_ONE_SHOT);
	}
		// BUZZER_PlayTones(tones_cc);
		break;

	case AC_DISPLAY_BUTTON_DOWN_RELEASED: {
		APP_DBG_SIG("AC_DISPLAY_BUTTON_DOWN_RELEASED\n");
		// 
		if (screen_menu.location < NUMBER_MENU_ITEMS-1) {
			screen_menu.location++;
		}
		if (frame_white.axis_y == frame[0].axis_y) {
			frame_white.axis_y = frame[1].axis_y;
		}
		else if (frame_white.axis_y == frame[1].axis_y) {
			frame_white.axis_y = frame[2].axis_y;
		}
		else if (frame_white.axis_y == frame[2].axis_y) {
			if (screen_menu.screen < NUMBER_MENU_ITEMS-3) {
				screen_menu.screen++;
			}
		}
		update_menu_screen_chosse();
		// Reset timer switch to scr_idle
		timer_set(	AC_TASK_DISPLAY_ID, \
					AC_DISPLAY_SHOW_IDLE, \
					AC_DISPLAY_IDLE_INTERVAL, \
					TIMER_ONE_SHOT);
	}
		// BUZZER_PlayTones(tones_cc);
		break;

	default:
		break;
	}
}