#ifndef __perseus_monicare__h_
#define __perseus_monicare__h_

// Headers
#include "inttypes.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "inttypes.h"
#include "LCDI2C.h"
#include "structs.h"

extern device_state_t device_state;
extern uint64_t last_user_interaction;
// LCD
extern bool lcd_is_on;
extern const byte lcd_add;
extern const uint8_t lcd_column;
extern const uint8_t lcd_row;
extern LCDI2C lcd;
// LCD symbols
extern const byte degree_symbol;
extern const uint8_t clock_symbol[8];
extern const uint8_t heart_symbol[8];
// standby
extern bool standby_indicator_led_state;

#endif
