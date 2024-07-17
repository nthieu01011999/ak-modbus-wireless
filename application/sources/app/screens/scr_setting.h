#ifndef __SCR_SETTING_H__
#define __SCR_SETTING_H__

#include "fsm.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "sys_ctrl.h"
#include "sys_dbg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"
#include "view_render.h"

#include "buzzer.h"

#include "screens.h"

#include "app_eeprom.h"
#include "eeprom.h"
#include "mw24_eeprom.h"

#include "mw24_io_cfg.h"

extern view_dynamic_t dyn_view_setting;

extern view_screen_t scr_setting;
extern void scr_setting_handle(ak_msg_t* msg);

#endif // __SCR_SETTING_H__