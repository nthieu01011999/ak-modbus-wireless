#ifndef __MW24_EEPROM_H__
#define __MW24_EEPROM_H__

#include <stdint.h>
#include "sys_dbg.h"
#include "eeprom.h"
#include "mw24_config.h"

/***********************************************************/
/* Read / write data */
/************************************************************/
#define MW24_MAGIC_NUMBER_CFG_DATA	 (0xAABBCCDD)
#define MW24_EEPROM_DATA_CONFIG_ADDR (0x0100)

#define SIZE_DATA_CONFIG			 (sizeof(mw24_eeprom_data_t))

typedef struct {
	uint8_t address;
	uint8_t channel;
	uint8_t number_baudrate;
	uint8_t null;
} mw24_data_cfg_t;

typedef struct {
	uint32_t magic_number;
	mw24_data_cfg_t data;
	uint8_t checksum;
} mw24_eeprom_data_t;

typedef enum mw24_modbus_baudrate_e {
	MB_BAUDRATE_2400,
	MB_BAUDRATE_4800,
	MB_BAUDRATE_9600,
	MB_BAUDRATE_14400,
	MB_BAUDRATE_19200,
	MB_BAUDRATE_28800,
	MB_BAUDRATE_38400,
	MB_BAUDRATE_57600,
	MB_BAUDRATE_76800,
	MB_BAUDRATE_115200
} mw24_modbus_baudrate_t;

const uint32_t mw24_modbus_baudrate_array[] = {2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200};

typedef enum mw24_eeprom_states_e {
	MW24_EEPROM_READ_DATA_SUCCESS,
	MW24_EEPROM_READ_DATA_FAIL,		// Wrong magic number
	MW24_EEPROM_READ_DATA_ERROR,	// Wrong checksum
	MW24_EEPROM_WRITE_DATA_SUCCESS,
	MW24_EEPROM_WRITE_DATA_FAIL
} mw24_eeprom_states_t;

extern uint8_t mw24_eeprom_write_data_cfg(mw24_data_cfg_t *data);
extern uint8_t mw24_eeprom_read_data_cfg(mw24_data_cfg_t *data);
extern bool mw24_set_data_cfg(mw24_data_cfg_t *data);

extern uint8_t mw24_get_number_address_nrf();
extern uint8_t *mw24_get_address_nrf();
extern uint8_t mw24_get_channel_nrf();
extern uint8_t mw24_get_number_baudrate_modbus();
extern uint32_t mw24_get_baudrate_modbus();

#endif //__MW24_EEPROM_H__
