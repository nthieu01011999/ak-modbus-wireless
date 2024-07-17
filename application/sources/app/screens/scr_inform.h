#ifndef __SCR_INFORM_H__
#define __SCR_INFORM_H__

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

#include "app_eeprom.h"
#include "eeprom.h"
#include "mw24_eeprom.h"

#include "screens.h"

extern view_dynamic_t dyn_view_inform;

extern view_screen_t scr_inform;
extern void scr_inform_handle(ak_msg_t* msg);

#endif // __SCR_INFORM_H__