/**
 ******************************************************************************
 * @author: GaoKong
 * @date:   13/08/2016
 ******************************************************************************
**/
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <forward_list>
#include <functional>
#include <iterator>
#include <vector>
#include <deque>
#include <queue>
#include <array>
#include <map>

/* kernel include */
#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

/* driver include */
#include "led.h"
#include "button.h"
#include "flash.h"

/* app include */
#include "app.h"
#include "app_dbg.h"
#include "app_bsp.h"
#include "app_flash.h"
#include "app_non_clear_ram.h"

#include "task_list.h"
#include "task_shell.h"
#include "task_life.h"
#include "task_if.h"
#include "task_uart_if.h"
#include "task_display.h"

/* sys include */
#include "sys_boot.h"
#include "sys_irq.h"
#include "sys_io.h"
#include "sys_ctrl.h"
#include "sys_dbg.h"

/* arduino include */
#include "SPI.h"
#include "WString.h"
#include "HardwareSerial.h"
#include "ArduinoJson.h"

/* common include */
#include "screen_manager.h"

/* ----------------------- Platform includes --------------------------------*/
#include "mw24_nwk.h"
#include "mw24_io_cfg.h"
#include "mw24_eeprom.h"

#include "buzzer.h"

#include "eeprom.h"

/* ----------------------- Json includes ------------------------------------*/
//#include "json.hpp"

using namespace std;

#define MBM_SERIAL_PORT           ( 2 )
#define MBM_SERIAL_BAUDRATE       ( 9600 )

#if defined(RELEASE)
const char* app_run_mode = "RELEASE";
#else
static const char* app_run_mode = "DEBUG";
#endif

const app_info_t app_info { \
	APP_MAGIC_NUMBER, \
			APP_VER, \
};

static boot_app_share_data_t boot_app_share_data;

static void app_power_on_reset();
static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();
static void app_modbus_wireless_init();

/*****************************************************************************/
/* app main function.
 */
/*****************************************************************************/
int main_app() {
	APP_PRINT("App run mode: %s, App version: %d.%d.%d.%d\n" \
			, app_run_mode	\
			, app_info.version[0]	\
			, app_info.version[1]	\
			, app_info.version[2]	\
			, app_info.version[3]);

	sys_soft_reboot_counter++;

	/******************************************************************************
	* init active kernel
	*******************************************************************************/
	ENTRY_CRITICAL();
	task_init();
	task_create((task_t*)app_task_table);
	task_polling_create((task_polling_t*)app_task_polling_table);
	EXIT_CRITICAL();

	/******************************************************************************
	* init applications
	*******************************************************************************/
	/*********************
	* hardware configure *
	**********************/
	/* init watch dog timer */
	sys_ctrl_independent_watchdog_init();	/* 32s */
	sys_ctrl_soft_watchdog_init(200);		/* 20s */

	/* init modbus wireless timer */
	mw24_timer_init(); /* 3ms */

	SPI.begin();

	/* adc peripheral configure */
	io_cfg_adc1();			/* configure adc for thermistor and CT sensor */

	/* adc configure for ct sensor */
	adc_bat_io_cfg();

	/* flash io init */
	flash_io_ctrl_init();

	/*********************
	* software configure *
	**********************/
	/* initial boot object */
	sys_boot_init();

	/* life led init */
	led_init(&led_life, led_life_init, led_life_on, led_life_off);

	ring_buffer_char_init(&ring_buffer_console_rev, buffer_console_rev, BUFFER_CONSOLE_REV_SIZE);

	/* button init */
	button_init(&btn_mode,	10,	BUTTON_MODE_ID,	io_button_mode_init,	io_button_mode_read,	btn_mode_callback);
	button_init(&btn_up,	10,	BUTTON_UP_ID,	io_button_up_init,		io_button_up_read,		btn_up_callback);
	button_init(&btn_down,	10,	BUTTON_DOWN_ID,	io_button_down_init,	io_button_down_read,	btn_down_callback);

	button_enable(&btn_mode);
	button_enable(&btn_up);
	button_enable(&btn_down);

	/* siren init */
	BUZZER_Init();
	BUZZER_PlayTones(tones_startup);

	/* init modbus wireless io debug */
	mw24_io_debug_init();

	/* get boot share data */
	flash_read(APP_FLASH_INTTERNAL_SHARE_DATA_SECTOR_1, reinterpret_cast<uint8_t*>(&boot_app_share_data), sizeof(boot_app_share_data_t));
	if (boot_app_share_data.is_power_on_reset == SYS_POWER_ON_RESET) {
		app_power_on_reset();
	}

	/* increase start time */
	fatal_log_t app_fatal_log;
	flash_read(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, reinterpret_cast<uint8_t*>(&app_fatal_log), sizeof(fatal_log_t));
	app_fatal_log.restart_times ++;
	flash_erase_sector(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR);
	flash_write(APP_FLASH_AK_DBG_FATAL_LOG_SECTOR, reinterpret_cast<uint8_t*>(&app_fatal_log), sizeof(fatal_log_t));

	EXIT_CRITICAL();

	/* start timer for application */
	app_init_state_machine();
	app_start_timer();

	/******************************************************************************
	* app task initial
	*******************************************************************************/
	app_task_init();

	/******************************************************************************
	* app modbus wireless
	*******************************************************************************/
	app_modbus_wireless_init();

	/******************************************************************************
	* run applications
	*******************************************************************************/
#if !defined(IF_LINK_UART_EN)
	sys_ctrl_shell_sw_to_nonblock();
#endif

	return task_run();
}

/*****************************************************************************/
/* polling task can be pushed right here
 * when all ak message queue empty, task_polling_xxx() will be called.
 */
/*****************************************************************************/

void task_polling_console() {
	volatile uint8_t c = 0;

	while (ring_buffer_char_is_empty(&ring_buffer_console_rev) == false) {

		ENTRY_CRITICAL();
		c = ring_buffer_char_get(&ring_buffer_console_rev);
		EXIT_CRITICAL();

#if defined (IF_LINK_UART_EN)
		if (plink_hal_rev_byte(c) == LINK_HAL_IGNORED) {
#endif
			if (shell.index < SHELL_BUFFER_LENGHT - 1) {

				if (c == '\r' || c == '\n') { /* linefeed */

					xputc('\r');
					xputc('\n');

					shell.data[shell.index] = c;
					shell.data[shell.index + 1] = 0;
					task_post_common_msg(AC_TASK_SHELL_ID, AC_SHELL_LOGIN_CMD, (uint8_t*)&shell.data[0], shell.index + 2);

					shell.index = 0;
				}
				else {

					xputc(c);

					if (c == 8 && shell.index) { /* backspace */
						shell.index--;
					}
					else {
						shell.data[shell.index++] = c;
					}
				}
			}
			else {
				LOGIN_PRINT("\nerror: cmd too long, cmd size: %d, try again !\n", SHELL_BUFFER_LENGHT);
				shell.index = 0;
			}
#if defined (IF_LINK_UART_EN)
		}
#endif
	}
}

/*****************************************************************************/
/* app initial function.
 */
/*****************************************************************************/

/* start software timer for application
 * used for app tasks
 */
void app_start_timer() {
	/* start timer to toggle life led */
	timer_set(	AC_TASK_LIFE_ID, \
				AC_LIFE_SYSTEM_CHECK, \
				AC_LIFE_TASK_TIMER_LED_LIFE_INTERVAL, \
				TIMER_PERIODIC);
	timer_set(	AC_TASK_FW_ID, \
				FW_CHECKING_REQ, \
				FW_UPDATE_REQ_INTERVAL, \
				TIMER_ONE_SHOT);
	timer_set(	AC_TASK_DISPLAY_ID, \
				AC_DISPLAY_INITIAL, \
				AC_DISPLAY_INITIAL_INTERVAL, \
				TIMER_ONE_SHOT);
}

/* init state machine for tasks
 * used for app tasks
 */
void app_init_state_machine() {

}

/* send first message to trigger start tasks
 * used for app tasks
 */
void app_task_init() {
	SCREEN_CTOR(&scr_mng_app, scr_startup_handle, &scr_startup);
	task_post_pure_msg(AC_TASK_UART_IF_ID, AC_UART_IF_INIT);
}

void app_modbus_wireless_init() {
	mw24_data_cfg_t data;
	xprintf("Read data %d", mw24_eeprom_read_data_cfg(&data));

	if (mw24_eeprom_read_data_cfg(&data) != MW24_EEPROM_READ_DATA_SUCCESS) {
		data.address = MW24_ADDRESS_RF24_MIN;
		data.number_baudrate = MW24_NUMBER_BAUDRATE_MODBUS_MIN;
		data.channel = MW24_CHANNEL_RF24_MIN;
		mw24_eeprom_write_data_cfg(&data);
	}

	mw24_set_data_cfg(&data);

	mw24_nrf_init();
	mw24_rs485_uart2_init();
}

/*****************************************************************************/
/* app common function
 */
/*****************************************************************************/

/* hardware timer interrupt 10ms
 * used for led, button polling
 */
void sys_irq_timer_10ms() {
	button_timer_polling(&btn_mode);
	button_timer_polling(&btn_up);
	button_timer_polling(&btn_down);
}

/* init non-clear RAM objects
 */
void app_power_on_reset() {
	sys_soft_reboot_counter = 0;
}

void* app_get_boot_share_data() {
	return static_cast<void*>(&boot_app_share_data);
}
