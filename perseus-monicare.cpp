#include "perseus-monicare.h"

// Variables
device_state_t device_state = CONNECTING_TO_WIFI_DS;
panic_state_t panic_state;
uint64_t last_user_interaction = 0;

// Sensors
sensor_port_t DS18B20 = { 27, 0 };
OneWire one_wire_DS18B20(DS18B20.port);
DallasTemperature dallas_DS18B20(&one_wire_DS18B20);
DeviceAddress DS18B20_addr;

// Matrix Keypad
char keypad_keys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};

Keypad keypad = Keypad(makeKeymap(keypad_keys), KEYPAD_ROW_PINS, KEYPAD_COL_PINS, KEYPAD_ROWS, KEYPAD_COLS);

// LCD
bool lcd_is_on = true;
LCDI2C lcd(LCD_ADR, LCD_COLS, LCD_ROWS);

// standby
bool standby_indicator_led_state = false;

// WiFi Connection
WiFiClient wifi_client;
const char* ssid = "FLAVIO";
const char* password = "jogadordelol";

// API
api_client_t api_client(SERVER_IP, SERVER_PORT);
