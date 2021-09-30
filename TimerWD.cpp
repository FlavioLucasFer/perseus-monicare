#include "TimerWD.h"

/*
Destructor
*/
TimerWD::~TimerWD (void) {}

/*
Default constructor
*/
TimerWD::TimerWD (void) {}

/*
Set timeout

@param 16 bits unsigned int {timeout} -> Timeout to be setted

Returns: 8 bits unsigned int timeout index
*/
index_t TimerWD::set_timeout(timeout_t timeout)
{
	this->timeout_arr.push_back(timeout);
	this->previous_millis_arr.push_back(0);
	
	return this->previous_millis_arr.size() - 1;
}

void TimerWD::change_timeout (index_t timeout_index, timeout_t timeout)
{
	this->timeout_arr.at(timeout_index) = timeout;
}

/*
Check timeout

@param 8 bits unsigned int {timeout} -> timeout index

Returns: boolean -> true if is timeout else false
*/
bool TimerWD::is_timeout (index_t timeout_index)
{
	millis_t current_millis = millis();
	millis_t previous_millis = this->previous_millis_arr.at(timeout_index);
	timeout_t timeout = this->timeout_arr.at(timeout_index);

	if ((current_millis - previous_millis) > timeout) {
		this->previous_millis_arr.at(timeout_index) = current_millis;

		return true;
	}

	return false;
}