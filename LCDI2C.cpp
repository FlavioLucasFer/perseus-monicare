#include "LCDI2C.h"

/*
**Destructor**
*/
LCDI2C::~LCDI2C (void) {}

/*
**Constructor**

Constructor method that recieve LCD display address, number of columns and
rows.

@param int {address} -> LCD display address
@param int {columns} -> Columns number of display
@param int {rows} -> Rows number of display

Returns: void
*/
LCDI2C::LCDI2C (uint8_t address, uint8_t columns, uint8_t rows)
	: LiquidCrystal_I2C(address, columns, rows)
{
	this->address = address;
	this->columns = columns;
	this->rows = rows;
	this->column = columns;
	this->row = 0;
}


/*
Method to initialize LiquidCrystalk_I2C, its a mandatory method 
and needs to be called into Arduino's setup function.

@param void

Returns: void
*/
void LCDI2C::begin (void) 
{
	this->init();
	this->backlight();
}

/*
**Getter**

Getter to address attribute.

@param void

Returns: int this->address
*/
uint8_t LCDI2C::get_address (void) 
{
	return this->address;
}

/*
**Getter**

Getter to columns attribute.

@param void

Returns: int this->columns
*/
uint8_t LCDI2C::get_columns (void) 
{
	return this->columns;
}

/*
**Getter**

Getter to rows attribute.

@param void

Returns: int this->rows
*/
uint8_t LCDI2C::get_rows (void) 
{
	return this->rows;
}

/*
Method to scroll message horizontally from right to left.

@param int {row} -> The row where message will be displayed
@param String {message} -> The message to display
@param int {delay_time} -> the delay time of scroll

Returns: void
*/
void LCDI2C::scroll_horizontal (uint8_t row, String message, uint16_t delay_time)
{
	this->timerwd.change_timeout(this->timeout_sh, delay_time);

	this->setCursor(this->column >= 0 ? this->column : 0, row);

	if (this->column > -1)
		this->print(message.substring(0, this->columns - this->column));
	else {
		uint16_t pos = abs(this->column);
		uint16_t len = abs(this->column - message.length());

		if (len > this->columns)
			len = this->columns + pos;

		this->print(message.substring(pos, len));
	}

	if (this->timerwd.is_timeout(this->timeout_sh)) {
		this->clear();

		if (this->column > -abs(message.length()))
			this->column--;
		else
			this->column = this->columns;
	}
}

/*
Method to scroll message vertically from top to bottom.

@param int {column} -> The column where the first character of message will be displayed
@param String {message} -> The message to display
@param int {delay_time} -> the delay time of scroll

Returns: void
*/
void LCDI2C::scroll_vertical (uint8_t column, String message, uint16_t delay_time)
{
	this->timerwd.change_timeout(this->timeout_sv, delay_time);
	
	this->setCursor(column, this->row);
	this->print(message);

	if (this->timerwd.is_timeout(this->timeout_sv)) {
		this->clear();

		if (this->row < this->rows - 1)
			this->row++;
		else
			this->row = 0;
	}
}

/*
Method to display a simple message.

@param int {column} -> The column where the first character of the message will be displayed
@param int {row} -> The row where the message will be displayed
@param String {message} -> The message to display

Returns: void
*/

void LCDI2C::print_message (uint8_t column, uint8_t row, String message)
{
	this->setCursor(column, row);
	this->print(message);
}
