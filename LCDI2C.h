#ifndef __perseus_monicare_lcdi2c__h_
#define __perseus_monicare_lcdi2c__h_

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "inttypes.h"
#include "TimerWD.h"

/*
Class to display a message at LCD display of type I2C.
This class extends LiquidCrystal_I2C library and depends
of Wire library too.
*/

typedef uint8_t timeout_index;

class LCDI2C : public LiquidCrystal_I2C {
	public:
		~LCDI2C (void);
		LCDI2C (uint8_t address, uint8_t columns, uint8_t rows);
		void begin (void);
		uint8_t get_address (void);
		uint8_t get_columns (void);
		uint8_t get_rows (void);
		void scroll_horizontal (uint8_t row, String message, uint16_t delay_time);
		void scroll_vertical (uint8_t column, String message, uint16_t delay_time);
		void print_message(uint8_t column, uint8_t row, String message);

	private:
		uint8_t address;
		uint8_t columns = 20;
		uint8_t rows = 4;
		int8_t column = -1;
		uint8_t row = 0;
		TimerWD timerwd;
		timeout_index timeout_sv = timerwd.set_timeout(0);
		timeout_index timeout_sh = timerwd.set_timeout(0);
};

#endif
