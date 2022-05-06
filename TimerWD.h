#ifndef __TIMERWD__
#define __TIMERWD__

#include <Arduino.h>
#include <inttypes.h>
#include <iostream>
#include <vector>

/*
Class to control time without using Arduino's delay function.
*/

// type used to index of vector
typedef uint8_t index_t; // max value = 256

// type used to timeouts
typedef uint16_t timeout_t; // max value = 65536

// type used to millis
typedef uint64_t millis_t; // max value = 1.8446744e+19

class TimerWD {
	public:
		~TimerWD (void);
		TimerWD (void);
		index_t set_timeout (timeout_t timeout);
		bool is_timeout (index_t timeout_index);
		void change_timeout (index_t timeout_index, timeout_t timeout);

	private:
		std::vector<timeout_t> timeout_arr;
		std::vector<millis_t> previous_millis_arr;
};

#endif