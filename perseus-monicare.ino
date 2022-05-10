#include "perseus-monicare.h"

static void look_for_DS18B20_sensors (DallasTemperature dallas_instance, DeviceAddress sensor_addr)
{
	if (!dallas_instance.getAddress(sensor_addr, 0)) {
		DPRINTLN_L("Sensors not found!");
		panic_state = DS18B20_NOT_FOUND;
		device_state = PANIC_DS;
	}
	
	#if DEBUG
		DPRINTLN_L("Looking for DS18B20 sensors...");
		DPRINT_L("Found ");
		DPRINT(dallas_instance.getDeviceCount(), DEC);
		DPRINTLN_L(" sensors.");
		DPRINT_L("Sensor address: ");

		for (uint8_t i = 0; i < 8; i++) {
			if (sensor_addr[i] < 16)
				DPRINT_L("0");
			DPRINT(sensor_addr[i], HEX);
		}
		
		DPRINTLN_L("\n");
	#endif
}

static bool user_is_inactive ()
{
	if ((millis() - last_user_interaction) < 10000) {
		return true;
	}

	return false;
}

static void connect_to_network(const char *ssid, const char *password)
{
	unsigned long previous_millis = millis();
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	DPRINT_L("\nTrying to connect to WiFi network: ");
	DPRINT(ssid);

	while (WiFi.status() != WL_CONNECTED && millis() - previous_millis >= 500)
	{
		DPRINT('.');
		previous_millis = millis();
	}

	DPRINT_L("Local IP: ");
	DPRINTLN(WiFi.localIP());
	DPRINT_L("Mac address: ");
	DPRINTLN(WiFi.macAddress());

	DPRINTLN_L("You're connected to the WiFi network\n");

	device_state = LOGGING_IN_DS;
}

static void stand_by ()
{
	lcd.noBacklight();
	standby_indicator_led_state = true;

	// TODO
	// get o'clock from web
	// if ()
	// alarm();
}

static void wait_login_input(String& var)
{
	while (true)
	{
		char key = keypad.getKey();

		if (key != NO_KEY)
		{
			PRINT("[KEY]: ");
			PRINTLN(key);

			if (key == '#')
				break;
			else if (key == '*')
			{
				var.remove(var.length() - 1);
			}
			else
				var += key;

			PRINT_L("VAR: ");
			PRINTLN(var);
		}
	}
}

// TODO
static void login ()
{
	String username;
	String password;
	
	DPRINTLN_L("Username: ");
	wait_login_input(username);
	DPRINTLN_L("Password: ");
	wait_login_input(password);

	DPRINT_L("\n[USERNAME]: ");
	DPRINTLN(username);
	DPRINT_L("[PASSWORD]: ");
	DPRINTLN(password);

	uint32_t expiresIn = api_client.login(username, password);
	DPRINT("expiresIn:");
	DPRINTLN(expiresIn);
}

// TODO
static void menu ()
{
	// function to show application menu
}

// TODO
static void alarm ()
{
	// function to play buzzer to remind the user
	// to take your measurements
}

static void get_body_temperature ()
{
	uint32_t previous_millis = millis();
	float temp_c = 0;

	PRINTLN_L("Getting DS18B20's celsius temperature");

	while (millis() - previous_millis < 120000)
	{
		
		dallas_DS18B20.requestTemperatures();
		temp_c = dallas_DS18B20.getTempC(DS18B20_addr);
		PRINTLN(temp_c);

		delay(50);
	}

	temp_c += 2;

	DS18B20.value = temp_c;

	PRINTLN_L("\nTemp C: ");
	PRINTLN(temp_c);
}

// TODO
static void get_blood_oxygenation_and_heart_rate ()
{
	// function to get user's blood oxygenation
	// and save it in the database
}

static void panic () {
	String panic_message = "[PANIC]\n";

	switch (panic_state) {
		case DS18B20_NOT_FOUND:
			panic_message += "DS18B20 sensor not found!\nCheck wire connections and restart device...";
			break;
		
		case GYMAX30102_NOT_FOUND:
			panic_message += "GY-MAX30102 sensor not found!\nCheck wire connections and restart device...";
			break;

		default:
			panic_message = "Unexpected error ocurred!\n\n(1) Check all wire connections\n(2) Check code, recompile and up the sketch";
			break;
	}

	DPRINT_L(panic_message.c_str());

	delay(10000);
}

void setup () 
{
	#if DEBUG
		Serial.begin(115200);

		while (!Serial);
	#endif

	dallas_DS18B20.begin();
	look_for_DS18B20_sensors(dallas_DS18B20, DS18B20_addr);
}

void loop () 
{
	switch (device_state) {
		case STAND_BY_DS:
			stand_by();
			break;

		case CONNECTING_TO_WIFI_DS:
			connect_to_network(SSID, NETWORK_PASSWORD);
			break;

		case LOGGING_IN_DS:
			login();
			break;
		
		case SHOWING_MENU_DS:
			menu();
			break;

		case GETTING_BODY_TEMPERATURE_DS:
			get_body_temperature();
			break;

		case GETTING_BLOOD_OXYGENATION_AND_HEART_RATE_DS:
			get_blood_oxygenation_and_heart_rate();
			break;

		default:
			panic();
			break;
	}
}

