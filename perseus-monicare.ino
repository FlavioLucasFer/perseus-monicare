#include "perseus-monicare.h"

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
	// function to get user's body temperature
	// and save it in the database
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
