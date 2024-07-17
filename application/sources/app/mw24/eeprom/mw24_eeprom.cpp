#include "mw24_eeprom.h"

static uint8_t number_address_rf24	  = MW24_ADDRESS_RF24_MIN;
static uint8_t address_rf24[]		  = {0x3c, 0x3c, 0x3c, 0, 0};
static uint8_t channel_rf24			  = MW24_CHANNEL_RF24_MIN;
static uint8_t number_baudrate_modbus = (uint8_t)MB_BAUDRATE_2400;
static uint32_t baudrate_modbus		  = mw24_modbus_baudrate_array[(uint8_t)MB_BAUDRATE_2400];

uint8_t caculate_checksum(mw24_data_cfg_t *data) {
	uint8_t checksum	= 0;
	uint8_t *data_array = reinterpret_cast<uint8_t *>(data);

	for (size_t i = 0; i < sizeof(mw24_data_cfg_t); i++) {
		checksum += data_array[i];
	}

	return checksum;
}

bool mw24_set_data_cfg(mw24_data_cfg_t *data) {
	/****************************
 * Address nRF24l01*
 ****************************/
	/* check */
	if (data->address < MW24_ADDRESS_RF24_MIN || data->address > MW24_ADDRESS_RF24_MAX) {
		data->address = MW24_ADDRESS_RF24_MIN;
		// FATAL("RF", 0x01);
		return false;
	}
	/* set*/
	number_address_rf24 = data->address;
	address_rf24[3]		= ((data->address << 8) & 0xFF);
	address_rf24[4]		= (data->address & 0xFF);

	/****************************
 * Channel nRF24l01*
 ****************************/
	/* check */
	if (data->channel < MW24_CHANNEL_RF24_MIN || data->channel > MW24_CHANNEL_RF24_MAX) {
		data->channel = MW24_CHANNEL_RF24_MIN;
		// FATAL("RF", 0x02);
		return false;
	}
	/* set */
	channel_rf24 = data->channel;
	/****************************
 * Baudrate modbus *
 ****************************/
	/* check */
	if (data->number_baudrate < MW24_NUMBER_BAUDRATE_MODBUS_MIN || data->number_baudrate > MW24_NUMBER_BAUDRATE_MODBUS_MAX) {
		data->number_baudrate = MW24_NUMBER_BAUDRATE_MODBUS_MIN;
		// FATAL("RF", 0x03);
		return false;
	}
	/* set */
	number_baudrate_modbus = data->number_baudrate;
	baudrate_modbus = mw24_modbus_baudrate_array[(uint8_t)number_baudrate_modbus];

	return true;
}

uint8_t mw24_eeprom_read_data_cfg(mw24_data_cfg_t *data) {
	mw24_eeprom_data_t data_config;

	uint8_t read_data[SIZE_DATA_CONFIG];
	eeprom_read(MW24_EEPROM_DATA_CONFIG_ADDR, (uint8_t *)&read_data, SIZE_DATA_CONFIG);

	memcpy(&data_config, read_data, SIZE_DATA_CONFIG);

	if (data_config.magic_number != MW24_MAGIC_NUMBER_CFG_DATA) {
		return MW24_EEPROM_READ_DATA_FAIL;
	}

	if (data_config.checksum == caculate_checksum(&data_config.data)) {
		if(!mw24_set_data_cfg(&data_config.data)) {
			return MW24_EEPROM_READ_DATA_ERROR;
		}
		memcpy(data, &data_config.data, sizeof(mw24_data_cfg_t));
		return MW24_EEPROM_READ_DATA_SUCCESS;
	}
	return MW24_EEPROM_READ_DATA_ERROR;
}

uint8_t mw24_eeprom_write_data_cfg(mw24_data_cfg_t *data) {
	mw24_eeprom_data_t save_data;
	save_data.data		   = *data;
	save_data.magic_number = MW24_MAGIC_NUMBER_CFG_DATA;
	save_data.checksum	   = caculate_checksum(data);

	uint8_t byte[SIZE_DATA_CONFIG];
	memcpy(&byte, &save_data, SIZE_DATA_CONFIG);
	if (eeprom_write(MW24_EEPROM_DATA_CONFIG_ADDR, (uint8_t *)&byte, SIZE_DATA_CONFIG) == 0) {
		return MW24_EEPROM_WRITE_DATA_SUCCESS;
	}
	return MW24_EEPROM_WRITE_DATA_FAIL;
}

uint8_t mw24_get_number_address_nrf() {
	return number_address_rf24;
}

uint8_t *mw24_get_address_nrf() {
	return (uint8_t*)&address_rf24;
}

uint8_t mw24_get_channel_nrf() {
	return channel_rf24;
}

uint8_t mw24_get_number_baudrate_modbus() {
	return number_baudrate_modbus;
}

uint32_t mw24_get_baudrate_modbus() {
	return baudrate_modbus;
}
