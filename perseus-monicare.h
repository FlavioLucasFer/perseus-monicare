#ifndef __PERSEUS_MONICARE__
#define __PERSEUS_MONICARE__

// Headers
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <inttypes.h>
#include <OneWire.h>
#include <Keypad.h>
#include <WiFi.h>
#include "lib.h"
#include "api_client.h"
#include "LCDI2C.h"

enum class device_state_t
{
	STAND_BY,
	CONNECTING_TO_WIFI,
	LOGGING_IN,
	SHOWING_MENU,
	GETTING_BODY_TEMPERATURE,
	GETTING_BLOOD_OXYGENATION_AND_HEART_RATE,
	PANIC_DS,

#define STAND_BY_DS device_state_t::STAND_BY
#define CONNECTING_TO_WIFI_DS device_state_t::CONNECTING_TO_WIFI
#define LOGGING_IN_DS device_state_t::LOGGING_IN
#define SHOWING_MENU_DS device_state_t::SHOWING_MENU
#define GETTING_BODY_TEMPERATURE_DS device_state_t::GETTING_BODY_TEMPERATURE
#define GETTING_BLOOD_OXYGENATION_AND_HEART_RATE_DS device_state_t::GETTING_BLOOD_OXYGENATION_AND_HEART_RATE
#define PANIC_DS device_state_t::PANIC_DS
};

enum class panic_state_t
{
	DS18B20_NOT_FOUND,
	GYMAX30102_NOT_FOUND,

#define DS18B20_NOT_FOUND panic_state_t::DS18B20_NOT_FOUND
#define GYMAX30102_NOT_FOUND panic_state_t::GYMAX30102_NOT_FOUND
};

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#define KEYPAD_ROW_PINS (byte[]) { 19, 18, 5, 17 }
#define KEYPAD_COL_PINS (byte[]) { 16, 4, 12, 15 }

#define DEGREE_SYMBOL 0xDF
#define CLOCK_SYMBOL (uint8_t[]) { 0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0 }
#define HEART_SYMBOL (uint8_t[]) { 0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0 }

#define LCD_ADR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

extern device_state_t device_state;
extern panic_state_t panic_state;
extern uint64_t last_user_interaction;

// Sensors
  // DS18B20
extern OneWire one_wire_DS18B20;
extern DallasTemperature dallas_DS18B20;
extern sensor_port_t DS18B20;
extern DeviceAddress DS18B20_addr;

// Keypad
extern Keypad keypad;

// LCD
extern bool lcd_is_on;
extern const byte lcd_add;
extern const uint8_t lcd_column;
extern const uint8_t lcd_row;
extern LCDI2C lcd;

// LCD symbols
extern const byte degree_symbol;
extern const uint8_t clock_symbol[8];
extern const uint8_t heart_symbol[8];

// standby
extern bool standby_indicator_led_state;

// WiFi Connection
extern WiFiClient wifi; 
extern const char *ssid;
extern const char *password;

// API
extern api_client_t api_client;

#endif
