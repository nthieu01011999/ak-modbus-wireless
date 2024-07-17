#ifndef __SCREENS_H__
#define __SCREENS_H__

#include "view_item.h"

/* screens */
#include "scr_startup.h"
#include "scr_menu.h"
#include "scr_inform.h"
#include "scr_check_connect.h"
#include "scr_setting.h"

// screen startup
extern view_dynamic_t dyn_view_startup;
extern view_screen_t scr_startup;
extern void scr_startup_handle(ak_msg_t* msg);

// screen menu
extern view_dynamic_t dyn_view_menu;
extern view_screen_t scr_menu;
extern void scr_menu_handle(ak_msg_t* msg);

// screen inform
extern view_dynamic_t dyn_view_inform;
extern view_screen_t scr_inform;
extern void scr_inform_handle(ak_msg_t* msg);

// screen test connect
extern view_dynamic_t dyn_view_check_connect;
extern view_screen_t scr_check_connect;
extern void scr_check_connect_handle(ak_msg_t* msg);

// screen setting
extern view_dynamic_t dyn_view_setting;
extern view_screen_t scr_setting;
extern void scr_setting_handle(ak_msg_t* msg);

// screen idle
extern view_dynamic_t dyn_view_idle;
extern view_screen_t scr_idle;
extern void scr_idle_handle(ak_msg_t* msg);

#endif //__SCREENS_H__