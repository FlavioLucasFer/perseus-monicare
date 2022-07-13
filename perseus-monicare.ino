#include "perseus-monicare.h"

static void lcd_clear_row (uint8_t row)
{
	for (uint8_t n = 0; n < LCD_COLS; n++) {
		lcd.setCursor(n, row);
		lcd.print(" ");
	}
}

static void lcd_clear_column (uint8_t col, uint8_t row)
{
	lcd.setCursor(col, row);
	lcd.print(" ");
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
	lcd_clear_row(2);
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

static void line_input(String& var)
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
				lcd_clear_column(var.length(), 3);
			}
			else {
				var += key;
				lcd_print(var, 0, 3);
			}

			PRINT_L("VAR: ");
			PRINTLN(var);
		}
	}
}

static void login ()
{
	lcd_print("Monicare - Login", 2);

	String username;
	String password;
	
	DPRINTLN_L("Username: ");
	lcd_print("Usuario:", 0, 2);
	line_input(username);

	lcd_clear_row(2);
	lcd_clear_row(3);
	lcd_print("Senha:", 0, 2);
	DPRINTLN_L("Password: ");
	line_input(password);

	DPRINT_L("\n[USERNAME]: ");
	DPRINTLN(username);
	DPRINT_L("[PASSWORD]: ");
	DPRINTLN(password);

	lcd_clear_row(2);
	lcd_clear_row(3);
	lcd_print("Efetuando login...", 0, 2);

	api_client.login(username, password);
	DPRINTLN_L("SUCCESSFULLY LOGGED IN");

	lcd_clear_row(2);
	lcd_print("Login efetuado", 3, 2);
	lcd_print("com sucesso!", 4, 3);
	delay(2000);

	lcd_clear_row(2);
	lcd_clear_row(3);
	lcd_print("Entrando...", 4, 2);
	delay(3000);

	lcd.clear();
	device_state = SHOWING_MENU_DS;
}

static void menu ()
{
	while (device_state == SHOWING_MENU_DS) {
		lcd_print("Monicare - Menu", 3);
		lcd_print("1) Medir temp. corp.", 0, 1);
		lcd_print("2) Medir freq. cardi", 0, 2);
		lcd_print("aca e oxi. sanguinea", 0, 3);

		char menu_choice = keypad.waitForKey();

		switch (menu_choice) {
			case '1':
				device_state = GETTING_BODY_TEMPERATURE_DS;
				break;
			case '2':
				device_state = GETTING_BLOOD_OXYGENATION_AND_HEART_RATE_DS;
				break;
			default:
				lcd.clear();
				lcd_print("Opcao invalida!", 2, 1);
				break;
		}

		lcd.clear();
	}
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
	measurement_t measurement;
	measurement.measured_at = get_timestamp();
	measurement.measurement_type_id = BODY_TEMPERATURE_MTID;

	lcd_print("Por favor, coloque o");
	lcd_print("termometro embaixo", 1, 1);
	lcd_print("do braco...", 4, 2);

	float initial_temp;
	float control_temp;
	DPRINTLN_L("Waiting for the user to place the thermometer under the arm");
	uint8_t stab_count = 5;
	do {
		dallas_DS18B20.requestTemperatures();
		control_temp = dallas_DS18B20.getTempC(DS18B20_addr);

		if (stab_count > 0) {
			initial_temp = control_temp;
			DPRINT_L("Inicial temperature: ");
			DPRINTLN(initial_temp);
			stab_count--;
		} 

		DPRINTLN(control_temp);
	} while ((control_temp - initial_temp) < 0.5);

	lcd.clear();
	lcd_print("Coletando temperatu-");
	lcd_print("ra corporal", 0, 1);
	lcd_print("AGUARDE...", 5, 3);

	DPRINTLN_L("Getting DS18B20's celsius temperature");
	uint32_t previous_millis = millis();
	while (millis() - previous_millis < DS18B20_STAB_TIME) {
		dallas_DS18B20.requestTemperatures();
		measurement.value = dallas_DS18B20.getTempC(DS18B20_addr);
		DPRINTLN(measurement.value);

		delay(50);
	}

	measurement.value += 2;
	DS18B20.value = measurement.value;

	lcd.clear();
	lcd_print("Temperatura corporal");
	lcd_print(ceil(measurement.value * 100) / 100, 7, 1);
	lcd_print(DEGREE_SYMBOL, 12, 1);
	lcd_print("Salvando...", 4, 3);

	DPRINTLN_L("\nTemp C: ");
	DPRINTLN(measurement.value);

	delay(5000);
 
	api_client.store_measurement(measurement);

	DPRINTLN_L("BODY TEMPERATURE SUCCESSFULLY REGISTERED");
	
	delay(3000);
	lcd.clear();
	device_state = SHOWING_MENU_DS;
}

// TODO
static void get_blood_oxygenation_and_heart_rate ()
{
	measurement_t spo2_measurement;
	spo2_measurement.measured_at = get_timestamp();
	spo2_measurement.measurement_type_id = BLOOD_OXYGENATION_MTID;
	
	measurement_t heart_rate_measurement;
	heart_rate_measurement.measured_at = get_timestamp();
	heart_rate_measurement.measurement_type_id = HEART_RATE_MTID;

	lcd.clear();
	lcd_print("Por favor, coloque o");
	lcd_print("dedo sob o LED.", 0, 1);

	while (oximeter.getIR() < 5000);

	lcd.clear();
	lcd_print("Coletando dados", 2);
	lcd_print("AGUARDE...", 5, 3);

	for (byte i = 0; i < OXI_BUFFER_LENGTH; i++) {
		while (!oximeter.available())
			oximeter.check();

		red_buffer[i] = oximeter.getIR();
		ir_buffer[i] = oximeter.getRed();
		oximeter.nextSample();

		DPRINT_L("red: ");
		DPRINT(red_buffer[i], DEC);
		DPRINT_L("\t ir: ");
		DPRINT(ir_buffer[i], DEC);
	}

	maxim_heart_rate_and_oxygen_saturation(
		ir_buffer, 
		OXI_BUFFER_LENGTH, 
		red_buffer, 
		&spo2, 
		&valid_spo2, 
		&heart_rate, 
		&valid_heart_rate
	);

	lcd_print("BPM = ", 0, 1);
	lcd_print("SPO2 = ", 0, 2);

	uint64_t previous_millis = millis();

	while ((millis() - previous_millis) < OXI_STAB_TIME) {
		for (byte i = 25; i < 100; i++) {
			red_buffer[i - 25] = red_buffer[i];
			ir_buffer[i - 25] = ir_buffer[i];
		}

		for (byte i = 75; i < 100; i++) {
			while (!oximeter.available())
				oximeter.check();

			red_buffer[i] = oximeter.getRed();
			ir_buffer[i] = oximeter.getIR();
			oximeter.nextSample();
			
			DPRINT_L("red: ");
			DPRINT(red_buffer[i], DEC);
			DPRINT_L("\t ir: ");
			DPRINT(ir_buffer[i], DEC);

			DPRINT_L("\t HR=");
			DPRINT(heart_rate, DEC);
			DPRINT_L("\t AVG=");
			DPRINT(beat_avg, DEC);
			DPRINT_L("\t valid=");
			DPRINT(valid_heart_rate, DEC);

			DPRINT_L("\t SPO2=");
			DPRINT(spo2, DEC);
			DPRINT_L("\t AVG=");
			DPRINT(spo2_avg, DEC);
			DPRINT_L("\t valid=");
			DPRINT(valid_spo2, DEC);

			long ir_value = ir_buffer[i];

			if (checkForBeat(ir_value)) {
				long delta = millis() - last_beat;
				last_beat = millis();

				beats_per_minute = 60 / (delta / 1000.0);
				beat_avg = (beat_avg + beats_per_minute) / 2;
			}
			if (millis() - last_beat > 10000) {
				beats_per_minute = 0;
				beat_avg = (beat_avg + beats_per_minute) / 2;
			}
		}

		maxim_heart_rate_and_oxygen_saturation(
			ir_buffer,
			OXI_BUFFER_LENGTH,
			red_buffer,
			&spo2,
			&valid_spo2,
			&heart_rate,
			&valid_heart_rate
		);

		DPRINT_L("\t HR=");
		DPRINT(heart_rate, DEC);
		DPRINT_L("\t AVG=");
		DPRINT(beat_avg, DEC);
		DPRINT_L("\t valid=");
		DPRINT(valid_heart_rate, DEC);

		DPRINT_L("\t SPO2=");
		DPRINT(spo2, DEC);
		DPRINT_L("\t AVG=");
		DPRINT(spo2_avg, DEC);
		DPRINT_L("\t valid=");
		DPRINT(valid_spo2, DEC);

		if (valid_spo2 == 1 && spo2 < 100 && spo2 > 0)
			spo2_avg = (spo2_avg + spo2) / 2;
		else {
			spo2 = 0;
			spo2_avg = (spo2_avg + spo2) / 2;
		}

		lcd_print(beat_avg, 8, 1);
		lcd_print(spo2_avg, 9, 2);
	}

	lcd.clear();
	lcd_print("Dados coletados!", 3);
	lcd_print("BPM = ", 0, 1);
	lcd_print("SPO2 = ", 0, 2);
	lcd_print(beat_avg, 8, 1);
	lcd_print(spo2_avg, 9, 2);
	lcd_print("Salvando...", 4, 3);

	DPRINTLN_L("\nBPM AVG: ");
	DPRINTLN(beat_avg);
	DPRINTLN_L("\nSPO2 AVG: ");
	DPRINTLN(spo2_avg);

	delay(5000);

	heart_rate_measurement.value = beat_avg;
	spo2_measurement.value = spo2_avg;

	api_client.store_measurement(heart_rate_measurement);
	api_client.store_measurement(spo2_measurement);

	DPRINTLN_L("HEART RATE AND SPO2 SUCCESSFULLY REGISTERED");

	delay(3000);
	lcd.clear();
	device_state = SHOWING_MENU_DS;
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
			message = "DS18B20 (TEMPERATURE SENSOR) SENSOR NOT FOUND";
			break;
		
		case GYMAX30102_NOT_FOUND:
			message = "GY-MAX30102 (PULSE OXIMETER) SENSOR NOT FOUND";
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

	DPRINTLN_L("Initializing Pulse Oximeter...");
	if (!oximeter.begin(Wire, I2C_SPEED_FAST))
		set_panic(GYMAX30102_NOT_FOUND);

	oximeter.setup(
		OXI_LED_BRIGHTNESS, 
		OXI_SAMPLE_AVERAGE, 
		OXI_LED_MODE, 
		OXI_SAMPLE_RATE, 
		OXI_PULSE_WIDTH, 
		OXI_ADC_RANGE
	);

	DPRINTLN_L("Initializing LCD Display...");
	lcd.init();
	lcd.backlight();

	DPRINTLN_L("Initializing DS18B20 (temperature sensor)...");
	Wire.begin();
	dallas_DS18B20.begin();

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
