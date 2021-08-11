enum class device_state_t
{
	stand_by,
	logging_in,
	showing_menu,
	getting_body_temperature,
	getting_blood_oxygenation,
	getting_heart_rate,
};

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