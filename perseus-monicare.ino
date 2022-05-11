#include "perseus-monicare.h"

static void lcd_clear_line(uint8_t line)
{
	for (uint8_t n = 0; n < LCD_COLS; n++) {
		lcd.setCursor(n, line);
		lcd.print(" ");
	}
}

static void look_for_I2C_devices ()
{
	byte error; 
	byte address;
	uint8_t n_devices = 0;

	DPRINTLN_L("Looking for I2C devices...");
	for (address = 1; address < 127; address++) {
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		
		if (error == 0) {
			#if DEBUG
				DPRINT_L("I2C device found at address 0x");

				if (address < 16)
					DPRINT_L("0");

				DPRINTLN(address, HEX);
			#endif
			n_devices++;
		}

		#if DEBUG
			else if (error == 4) {
				DPRINT_L("Unknow error at address 0x");
				
				if (address < 16) 
					DPRINT_L("0");

				DPRINTLN(address, HEX);
			}
		#endif
	}
	
	if (n_devices == 0) {
		DPRINTLN_L("No I2C devices found\n");
		set_panic(I2C_DEVICES_NOT_FOUND);
	}
}

static void look_for_DS18B20_sensors (DallasTemperature dallas_instance, DeviceAddress sensor_addr)
{
	if (!dallas_instance.getAddress(sensor_addr, 0)) {
		DPRINTLN_L("Sensors not found!");
		set_panic(DS18B20_NOT_FOUND);
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
	lcd_print("Monicare", 6);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	DPRINT_L("\nTrying to connect to WiFi network: ");
	DPRINT(ssid);

	lcd_print("Conectando ao WiFi", 1, 2);
	#if !DEBUG
		while (WiFi.status() != WL_CONNECTED);
	#else
		unsigned long previous_millis = millis();
		while (WiFi.status() != WL_CONNECTED && millis() - previous_millis >= 500) {
			DPRINT('.');
			previous_millis = millis();
		}
	#endif

	configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

	DPRINT_L("Local IP: ");
	DPRINTLN(WiFi.localIP());
	DPRINT_L("Mac address: ");
	DPRINTLN(WiFi.macAddress());

	DPRINTLN_L("You're connected to the WiFi network\n");

	delay(2000);
	lcd_clear_line(2);
	lcd_print("Connectado a rede", 0, 2);
	lcd_print(ssid, 0, 3);
	delay(3000);

	lcd.clear();
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
	while (true) {
		char key = keypad.getKey();

		if (key != NO_KEY) {
			PRINT("[KEY]: ");
			PRINTLN(key);

			if (key == '#')
				break;
			else if (key == '*') {
				var.remove(var.length() - 1);
			}
			else
				var += key;

			PRINT_L("VAR: ");
			PRINTLN(var);

			lcd_print(var, 0, 3);
		}
	}
}

// TODO
static void login ()
{
	lcd_print("Monicare - Login", 2);

	String username;
	String password;
	
	DPRINTLN_L("Username: ");
	lcd_print("Usuario:", 0, 2);
	wait_login_input(username);

	lcd_clear_line(2);
	lcd_clear_line(3);
	lcd_print("Senha:", 0, 2);
	DPRINTLN_L("Password: ");
	wait_login_input(password);

	DPRINT_L("\n[USERNAME]: ");
	DPRINTLN(username);
	DPRINT_L("[PASSWORD]: ");
	DPRINTLN(password);

	lcd_clear_line(2);
	lcd_clear_line(3);
	lcd_print("Efetuando login...", 0, 2);

	api_client.login(username, password);
	DPRINTLN_L("SUCCESSFULLY LOGGED IN");

	lcd_clear_line(2);
	lcd_print("Login efetuado", 3, 2);
	lcd_print("com sucesso!", 4, 3);
	delay(2000);

	lcd_clear_line(2);
	lcd_clear_line(3);
	lcd_print("Entrando...", 4, 2);
	delay(3000);

	lcd.clear();
	device_state = GETTING_BODY_TEMPERATURE_DS;
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

static String get_timestamp ()
{
	tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		return "Failed to obtain timestamp";
	}
	char time_str_buffer[50];
	strftime(time_str_buffer, sizeof(time_str_buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
	return time_str_buffer;
}

static void get_body_temperature ()
{
	uint32_t previous_millis = millis();
	measurement_t measurement;

	DPRINTLN_L("Getting DS18B20's celsius temperature");

	while (millis() - previous_millis < 120000) {
		dallas_DS18B20.requestTemperatures();
		measurement.value = dallas_DS18B20.getTempC(DS18B20_addr);
		DPRINTLN(measurement.value);

		delay(50);
	}

	measurement.value += 2;
	measurement.measured_at = get_timestamp();
	measurement.measurement_type_id = BODY_TEMPERATURE_MTID;

	DS18B20.value = measurement.value;

	DPRINTLN_L("\nTemp C: ");
	DPRINTLN(measurement.value);
 
	api_client.store_measurement(measurement);

	DPRINTLN_L("BODY TEMPERATURE SUCCESSFULLY REGISTERED");
}

// TODO
static void get_blood_oxygenation_and_heart_rate ()
{
	// function to get user's blood oxygenation
	// and save it in the database
}

static void set_panic (panic_state_t panic)
{
	device_state = PANIC_DS;
	panic_state = panic;
}

static void panic () 
{
	String message;
	String recommendation = "Check wire connections and restart device...";

	switch (panic_state) {
		case DS18B20_NOT_FOUND:
			message = "DS18B20 SENSOR NOT FOUND";
			break;
		
		case GYMAX30102_NOT_FOUND:
			message = "GY-MAX30102 SENSOR NOT FOUND";
			break;
		
		case I2C_DEVICES_NOT_FOUND:
			message = "I2C DEVICES NOT FOUND";
			break;

		default:
			message = "UNEXPECTED ERROR";
			recommendation = "(1) Check all wire connections\n(2) Check code, recompile and up the sketch"; 
			break;
	}

	lcd.clear();
	lcd_print("[PANIC]", 7);
	lcd_print(message, 0, 2);

	DPRINTLN_L("[PANIC]");
	DPRINTLN_L(message.c_str());
	DPRINTLN_L(recommendation.c_str());

	delay(10000);
}

void setup () 
{
	#if DEBUG
		Serial.begin(115200);

		while (!Serial);
	#endif

	lcd.init();
	lcd.backlight();
	Wire.begin();
	dallas_DS18B20.begin();

	connect_to_network(SSID, NETWORK_PASSWORD);

	look_for_DS18B20_sensors(dallas_DS18B20, DS18B20_addr);
	look_for_I2C_devices();
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
