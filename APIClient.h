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

class APIClient {
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
		struct patient_t
		{
			int id;
			String name;
			String token;

			patient_t (void) {
				this->id = 0;
				this->name = "";
				this->token = "";
			}
		};

	private:
		String ip;
		uint8_t port;
		HTTPClient http;
		HTTPProtocol http_protocol = HTTPProtocol::http;
		patient_t patient = patient_t();

	private:
		String getHTTPProtocolString (void);
		String getRouteString (Routes route);
		DynamicJsonDocument postRequest (int capacity, Routes r, String json);
		DynamicJsonDocument postRequest (int capacity, String route, String json);
		DynamicJsonDocument getRequest (int capacity, Routes r);
		DynamicJsonDocument getRequest (int capacity, String route);
		DynamicJsonDocument getLoggedUser (void);

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
		DynamicJsonDocument login (String username, String password);
		DynamicJsonDocument logout (void);
		DynamicJsonDocument refreshToken (void);
		DynamicJsonDocument getMeasurementTypes (void);
		DynamicJsonDocument setMeasurement (float value, String measured_at, int measurement_type_id);

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

		inline String getServerAddress (void) {
			return this->getHTTPProtocolString() + "://" + this->ip + ":" + this->port + "/api";
		}
};

#endif