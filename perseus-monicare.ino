#include "perseus-monicare.h"

static void connect_to_network ()
{
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		Serial.print("Connection to WiFi network: ");
		Serial.println(ssid);

		delay(500);
	}

	Serial.print("Local IP: ");
	Serial.println(WiFi.localIP());
	Serial.print("Mac address: ");
	Serial.println(WiFi.macAddress());

	Serial.println("You're connected to the WiFi network");
}

void look_for_DS18B20_sensors (DallasTemperature dallas_instance, DeviceAddress sensor_addr)
{
	Serial.println("Looking for DS18B20 sensors...");
	Serial.print("Found ");
	Serial.print(dallas_instance.getDeviceCount(), DEC);
	Serial.println(" sensors.");

	if (!dallas_instance.getAddress(sensor_addr, 0))
		Serial.println("Sensors not found!");

	Serial.print("Sensor address: ");

	for (uint8_t i = 0; i < 8; i++) {
		if (sensor_addr[i] < 16)
			Serial.print("0");
		Serial.print(sensor_addr[i], HEX);
	}

	Serial.println("\n");
}

static bool user_is_inactive ()
{
	if ((millis() - last_user_interaction) < 10000) {
		return true;
	}

	return false;
}

static void stand_by (void)
{
	lcd.noBacklight();
	standby_indicator_led_state = true;

	// TODO
	// get o'clock from web
	// if ()
	// alarm();
}

// TODO
static void login (void)
{
	// user login function
}

// TODO
static void menu (void)
{
	// function to show application menu
}

// TODO
static void alarm (void)
{
	// function to play buzzer to remind the user
	// to take your measurements
}

// TODO
static void get_body_temperature (void)
{
	uint32_t previous_millis = millis();
	float temp_c = 0;

	Serial.println("Getting DS18B20's celsius temperature");

	while (millis() - previous_millis < 120000)
	{
		
		dallas_DS18B20.requestTemperatures();
		temp_c = dallas_DS18B20.getTempC(DS18B20_addr);
		Serial.println(temp_c);

		delay(50);
	}

	temp_c += 2;

	DS18B20.value = temp_c;

	Serial.println("\nTemp C: ");
	Serial.println(temp_c);
}

// TODO
static void get_blood_oxygenation (void)
{
	// function to get user's blood oxygenation
	// and save it in the database
}

// TODO
static void get_heart_rate (void)
{
	// function to get user's heart rate
	// and save it in the database
}

static void panic (void) {
	// show panic error to user
}

void setup (void) 
{
	Serial.begin(115200);
	dallas_DS18B20.begin();

	connect_to_network();

	look_for_DS18B20_sensors(dallas_DS18B20, DS18B20_addr);
}

void loop (void) 
{
	switch (device_state) {
		case device_state_t::stand_by:
			stand_by();
			break;

		case device_state_t::logging_in:
			login();
			break;
		
		case device_state_t::showing_menu:
			menu();
			break;

		case device_state_t::getting_body_temperature:
			get_body_temperature();
			break;

		case device_state_t::getting_blood_oxygenation:
			get_blood_oxygenation();
			break;

		case device_state_t::getting_heart_rate:
			get_heart_rate();
			break;

		default:
			panic();
			break;
	}
}
