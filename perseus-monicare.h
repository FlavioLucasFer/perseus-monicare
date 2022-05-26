#ifndef __PERSEUS_MONICARE__
#define __PERSEUS_MONICARE__

// Headers
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <inttypes.h>
#include <OneWire.h>
#include <Keypad.h>
#include <WiFi.h>
#include <Wire.h>
#include <time.h>
#include "lib.h"
#include "env.h"
#include "api_client.h"

enum class device_state_t
{
	stand_by,
	connecting_to_wifi,
	logging_in,
	showing_menu,
	getting_body_temperature,
	getting_blood_oxygenation_and_heart_rate,
	panic,

#define STAND_BY_DS device_state_t::stand_by
#define CONNECTING_TO_WIFI_DS device_state_t::connecting_to_wifi
#define LOGGING_IN_DS device_state_t::logging_in
#define SHOWING_MENU_DS device_state_t::showing_menu
#define GETTING_BODY_TEMPERATURE_DS device_state_t::getting_body_temperature
#define GETTING_BLOOD_OXYGENATION_AND_HEART_RATE_DS device_state_t::getting_blood_oxygenation_and_heart_rate
#define PANIC_DS device_state_t::panic
};

enum class panic_state_t
{
	ds18b20_not_found,
	gymax30102_not_found,
	i2c_devices_not_found,

#define DS18B20_NOT_FOUND panic_state_t::ds18b20_not_found
#define GYMAX30102_NOT_FOUND panic_state_t::gymax30102_not_found
#define I2C_DEVICES_NOT_FOUND panic_state_t::i2c_devices_not_found
};

#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC -4 * 60 * 60
#define DAYLIGHT_OFFSET_SEC 3600

extern device_state_t device_state;
extern panic_state_t panic_state;
extern uint64_t last_user_interaction;

// Sensors
  // DS18B20
#define DS18B20_STAB_TIME 120000
extern OneWire one_wire_DS18B20;
extern DallasTemperature dallas_DS18B20;
extern sensor_port_t DS18B20;
extern DeviceAddress DS18B20_addr;

// Keypad
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
extern Keypad keypad;
extern byte keypad_row_pins[KEYPAD_ROWS];
extern byte keypad_col_pins[KEYPAD_COLS];

// LCD
#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4
extern bool lcd_is_on;
extern LiquidCrystal_I2C lcd;

template <typename T>
void lcd_print (T message, uint8_t col = 0, uint8_t row = 0)
{
	lcd.setCursor(col, row);
	lcd.print(message);
}

// LCD symbols
#define DEGREE_SYMBOL (char)223
extern const uint8_t clock_symbol[8];
extern const uint8_t heart_symbol[8];
// standby
extern bool standby_indicator_led_state;

// WiFi Connection
extern WiFiClient wifi; 

// API
extern api_client_t api_client;

#endif
