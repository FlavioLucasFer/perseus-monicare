#include <inttypes.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "structs.h";
#include "function_prototypes.h";

device_state_t device_state = device_state_t::stand_by;

void setup (void) 
{
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
			show_menu();
			break;
		
		case device_state_t::alarming:
			alarm();
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
			break;
	}
}

// TODO
void stand_by (void)
{
	// turn LCD off
	// checking o'clock to alarm user
}

// TODO
void login (void) 
{
	// user login function
}

// TODO
void show_menu (void) 
{
	// function to show application menu
}

// TODO
void alarm (void)
{
	// function to play buzzer to remind the user 
	// to take your measurements
}

// TODO
void get_body_temperature (void) 
{
	// function to get user's body temperature 
	// and save it in the database
}

// TODO
void get_blood_oxygenation (void) 
{
	// function to get user's blood oxygenation 
	// and save it in the database
}

// TODO
void get_heart_rate (void)
{
	// function to get user's heart rate 
	// and save it in the database
}