#ifndef __perseus_monicare_api_client__h_
#define __perseus_monicare_api_client__h_

#include "WiFi.h"
#include "inttypes.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include "Arduino.h"

enum class HTTPProtocol 
{
	http,
	https,
};

struct user_t 
{
	String username;
	String password;

	inline user_t (void) {}

	inline user_t (String username, String password)
	{
		this->username = username;
		this->password = password;
	}
};

struct patient_t
{
	int id;
	String name;
	String email;
	String phone;

	inline patient_t(void) {}

	inline patient_t(int id, String name, String email, String phone)
	{
		this->id = id;
		this->name = name;
		this->email = email;
		this->phone = phone;
	}
};

struct measurement_t 
{
	float value;
	String measured_at;
	uint8_t measurement_type_id;

	inline measurement_t (void) {}

	inline measurement_t (float value, uint8_t measurement_type_id)
	{
		this->value = value;
		this->measurement_type_id = measurement_type_id;
	}

	inline measurement_t (float value, String measured_at, uint8_t measurement_type_id)
	{
		this->value = value;
		this->measured_at = measured_at;
		this->measurement_type_id = measurement_type_id;
	}
};

class APIClient 
{
	private:
		enum class Routes
		{
			login,
			logout,
			me,
			refresh_token,
			measurement_types,
			patient_measurement,
		};
		struct auth_t
		{
			int id;
			String token;

			inline auth_t (void) {}
		};

	private:
		String ip;
		uint8_t port;
		HTTPClient http;
		HTTPProtocol http_protocol = HTTPProtocol::http;
		auth_t auth = auth_t();
		patient_t patient = patient_t();

	private:
		String getHTTPProtocolString (void);
		String getRouteString (Routes route);
		DynamicJsonDocument postRequest (int capacity, Routes r, String json);
		DynamicJsonDocument postRequest (int capacity, String route, String json);
		DynamicJsonDocument getRequest (int capacity, Routes r);
		DynamicJsonDocument getRequest (int capacity, String route);
		JsonObject getLoggedPatient (void);

	private:
		inline void isPatientLogged()
		{
			if (!this->auth.id)
				throw "No patient logged in";
		}

		inline void requestErrorHandler(uint16_t statusCode)
		{
			if (statusCode <= 0)
				return throw "Unexpected internal error";
		}

		inline void requestErrorHandler(uint16_t statusCode, DynamicJsonDocument doc)
		{
			if (statusCode >= 400)
				return throw doc;
		}

	public:
		inline APIClient (String server_ip, uint8_t server_port)
		{
			this->ip = server_ip;
			this->port = server_port;
		}

		inline APIClient (String server_ip, uint8_t server_port, HTTPProtocol http_protocol)
		{
			this->ip = server_ip;
			this->port = server_port;
			this->http_protocol = HTTPProtocol(http_protocol);
		}

		inline ~APIClient () {}

	public: 
		uint32_t login (user_t user);
		uint32_t login (String username, String password);
		void logout (void);
		uint32_t refreshToken (void);
		JsonArray getMeasurementTypes (void);
		JsonObject storeMeasurement (measurement_t measurement);
		JsonObject storeMeasurement (float value, String measured_at, uint8_t measurement_type_id);

	public:
		inline String getServerIp (void)
		{
			return this->ip;
		}

		inline uint8_t getServerPort (void)
		{
			return this->port;
		}

		inline HTTPProtocol getHTTPProtocol (void)
		{
			return this->http_protocol;
		}

		inline String getServerAddress (void)
		{
			return this->getHTTPProtocolString() + "://" + this->ip + ":" + this->port + "/api";
		}

		inline patient_t getPatient (void)
		{
			return this->patient;
		}
};

#endif