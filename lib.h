#ifndef __ESP_ARDUINO_LIB__
#define __ESP_ARDUINO_LIB__

#define OO_ENCAPSULATE(TYPE, VAR) \
    private: \
        TYPE VAR; \
    public: \
        inline TYPE get_##VAR () \
        { \
            return this->VAR; \
        } \
        inline void set_##VAR (TYPE VAR) \
        { \
            this->VAR = VAR; \
        }

#define OO_ENCAPSULATE_DV(TYPE, VAR, DEFAULT_VALUE) \
    private: \
        TYPE VAR = DEFAULT_VALUE; \
    public: \
        inline TYPE get_##VAR () \
        { \
            return this->VAR; \
        } \
        inline void set_##VAR (TYPE VAR) \
        { \
            this->VAR = VAR; \
        }

#define OO_ENCAPSULATE_RO(TYPE, VAR) \
    private: \
        TYPE VAR; \
    public: \
        inline TYPE get_##VAR () \
        { \
            return this->VAR; \
        }

#define OO_ENCAPSULATE_RO_DV(TYPE, VAR, DEFAULT_VALUE) \
    private: \
        TYPE VAR = DEFAULT_VALUE; \
    public: \
        inline TYPE get_##VAR () \
        { \
            return this->VAR; \
        }

#define PRINT(...) Serial.print(__VA_ARGS__)
#define PRINT_L(s) Serial.print(F(s)) // macro for literal strings
#define PRINTLN(...) Serial.println(__VA_ARGS__)
#define PRINTLN_L(s) Serial.println(F(s)) // nacro for literal strings

#define DEBUG 1

#if DEBUG
    #define DPRINT(...) PRINT(__VA_ARGS__)
    #define DPRINT_L(s) PRINT_L(s) // macro for literal strings
    #define DPRINTLN(...) PRINTLN(__VA_ARGS__)
    #define DPRINTLN_L(s) PRINTLN_L(s) // nacro for literal strings
#else
    #define DPRINT(...) 
    #define DPRINT_L(s)
    #define DPRINTLN(...)
    #define DPRINTLN_L(s)
#endif


struct statefull_port_t
{
    const uint8_t port;
    bool state;
};

struct sensor_port_t
{
    const uint8_t port;
    float value;
};

#endif
