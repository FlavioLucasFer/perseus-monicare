#include <inttypes.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ** STRUCTS **

// Struct to port that have state
struct statefull_port_t {
	const uint8_t PORT;
	bool state;
};

// Struct to sensor port
struct sensor_port_t {
	const uint8_t PORT;
	float value;
};

// ** PORTS **

// Button to start reading the DS18B20 sensor temperature
statefull_port_t ds18b20_button = { 14, false }; 
// DS18B20 sensor
sensor_port_t ds18b20_sensor = { 26, 0 };

// ** GENERAL PURPOSE VARIABLES **

// OneWire's instance of DS18B20 sensor, used to get sensor address
OneWire one_wire_ds18b20(ds18b20_sensor.PORT);
// DallasTemperature's instance of DS18B20 sensor, used to get sensor temperature
DallasTemperature dallas_ds18b20(&one_wire_ds18b20);
// DS18B20's address
DeviceAddress ds18b20_address;

// ** TASK HANDLERS **

// Handler to task that get DS18B20's temperature
TaskHandle_t get_ds18b20_temp_task_handle;

// ** FUNCTIONS PROTOTYPES **

void verify_ds18b20_sensor (DeviceAddress sensor_address);
void handle_ds18b20_button_irs (void);
void get_ds18b20_temp_task (void);

void setup (void) 
{
	Serial.begin(115200);
	dallas_ds18b20.begin(); // Initialize sensors

	verify_ds18b20_sensor(ds18b20_address);

	pinMode(ds18b20_button.PORT, INPUT_PULLUP);

	// Attach interruput of SD18B20's button on change
	attachInterrupt(ds18b20_button.PORT, handle_ds18b20_button_irs, CHANGE); 
}

void loop (void) 
{
}

// Verify DS18B20 sensor, looking for and printing it address
void verify_ds18b20_sensor (DeviceAddress sensor_address)
{
	Serial.println("Looking for DS18B20 sensors...");
	Serial.print("Found ");
	Serial.print(dallas_ds18b20.getDeviceCount(), DEC);
	Serial.println(" sensors.");

	if (!dallas_ds18b20.getAddress(ds18b20_address, 0))
		Serial.println("Sensors not found!");

	Serial.print("Sensor address: ");

	for (uint8_t i = 0; i < 8; i++) {
		if (sensor_address[i] < 16)
			Serial.print("0");
		Serial.print(sensor_address[i], HEX);
	}

	Serial.println("\n");
}

// Interrupt routine of DS18B20's button
void handle_ds18b20_button_irs (void) 
{
	Serial.println("handle_ds18b20_button_irs");

	if (digitalRead(ds18b20_button.PORT) == LOW) {
		Serial.println("on low");
		while (digitalRead(ds18b20_button.PORT) == LOW) {}

		// Create task on core 0
		xTaskCreatePinnedToCore(
			get_ds18b20_temp_task,
			"get_ds18b20_temp_task",
			10000,
			NULL,
			0,
			&get_ds18b20_temp_task_handle,
			0);
	}
}

// Task to get DS18B20's temperature
void get_ds18b20_temp_task (void * parameter)
{
	uint16_t previus_millis = millis();
	float temp_c = 0;
	
	Serial.printf("\nds18b20_temp_task() running on core: %d\n", xPortGetCoreID());
	Serial.println("Getting DS18B20's celsius temperature");

	// Get DS18B20's temperature for 2 minutes
	while (millis() - previus_millis < 120000) {
		Serial.print(".");
		dallas_ds18b20.requestTemperatures();
		temp_c = dallas_ds18b20.getTempC(ds18b20_address);
	}

	// Set collected temperature to value of ds18b20_sensor
	ds18b20_sensor.value = temp_c;

	Serial.println("\nTemp C: ");
	Serial.print(ds18b20_sensor.value);

	// Delete task
	vTaskDelete(NULL);
}