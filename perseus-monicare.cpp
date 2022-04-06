#include "perseus-monicare.h"

// Variables
device_state_t device_state = device_state_t::stand_by;
uint64_t last_user_interaction = 0;

// Sensors
sensor_port_t DS18B20 = { 27, 0 };
OneWire one_wire_DS18B20(DS18B20.port);
DallasTemperature dallas_DS18B20(&one_wire_DS18B20);
DeviceAddress DS18B20_addr;

// LCD
bool lcd_is_on = true;
const byte lcd_addr = 0x27;
const uint8_t lcd_columns = 20;
const uint8_t lcd_rows = 4;
LCDI2C lcd(lcd_addr, lcd_columns, lcd_rows);

// LCD symbols
const byte degree_symbol = 0xDF;
const uint8_t clock_symbol[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
const uint8_t heart_symbol[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};

// standby
bool standby_indicator_led_state = false;

// WiFi Connection
WiFiClient wifi_client;
const char* ssid = "";
const char* password = "";

// API
APIClient api_client("", 80);
