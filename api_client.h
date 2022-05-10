#ifndef __PERSEUS_MONICARE_API_CLIENT__
#define __PERSEUS_MONICARE_API_CLIENT__

#include <functional>
#include <string>
#include <inttypes.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "lib.h"

using json_capacity_t = uint16_t;
using server_ip_t = String;
using server_port_t = uint16_t;
using id_t = uint64_t;

enum class http_protocol_t 
{
	http,
	https,

#define HTTP_PROTOCOL http_protocol_t::http
#define HTTPS_PROTOCOL http_protocol_t::https
};

enum class route_t
{
	login,
	logout,
	me,
	refresh_token,
	measurement_types,
	patient_measurement,

#define LOGIN_ROUTE route_t::login
#define LOGOUT_ROUTE route_t::logout
#define ME_ROUTE route_t::me
#define REFRESH_TOKEN_ROUTE route_t::refresh_token
#define MEASUREMENT_TYPES_ROUTE route_t::measurement_types
#define PATIENT_MEASUREMENT_ROUTE route_t::patient_measurement
};

struct user_t 
{
	String username;
	String password;

	user_t () {}
	user_t (String username, String password)
	{
		this->username = username;
		this->password = password;
	}
};

struct patient_t
{
	id_t id;
	String name;
	String email;
	String phone;

	patient_t () {}
	patient_t (id_t id, String name, String email, String phone)
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
	id_t measurement_type_id;

	measurement_t () {}
	measurement_t (float value, id_t measurement_type_id)
	{
		this->value = value;
		this->measurement_type_id = measurement_type_id;
	}

	measurement_t (float value, String measured_at, id_t measurement_type_id)
	{
		this->value = value;
		this->measured_at = measured_at;
		this->measurement_type_id = measurement_type_id;
	}
};

class auth_t
{
	OO_ENCAPSULATE(id_t, id);
	OO_ENCAPSULATE(String, token);
	OO_ENCAPSULATE_RO(uint64_t, logged_at);
	OO_ENCAPSULATE_RO(uint32_t, expires_in);

public:
	auth_t() {}

public:
	inline void set_expires_in (uint32_t expires_in)
	{
		this->expires_in = expires_in;
		this->logged_at = millis();
	}

	inline bool is_expired()
	{
		return (this->logged_at - this->expires_in) <= 0;
	}

	inline bool is_logged()
	{
		return !is_expired() && this->token;
	}
};

class api_client_t 
{
	OO_ENCAPSULATE_RO(server_ip_t, ip);
	OO_ENCAPSULATE_RO(server_port_t, port);
	OO_ENCAPSULATE_RO_DV(http_protocol_t, http_protocol, HTTP_PROTOCOL);
	OO_ENCAPSULATE_RO_DV(auth_t, auth, auth_t());
	OO_ENCAPSULATE_RO_DV(patient_t, patient, patient_t());

	private:
		String stringify_http_protocol ();
		String stringify_route (route_t route);
		DynamicJsonDocument post_request (json_capacity_t capacity, route_t r, String json);
		DynamicJsonDocument post_request (json_capacity_t capacity, String route, String json);
		DynamicJsonDocument get_request (json_capacity_t capacity, route_t r);
		DynamicJsonDocument get_request (json_capacity_t capacity, String route);
		JsonObject get_logged_patient ();
	
	private:
		template<typename Fn>
		void http_request(String route, Fn request)
		{
			WiFiClient client;
			HTTPClient http;

			http.useHTTP10(true);
			http.begin(client, route.c_str());
			http.addHeader("Content-Type", "application/json");

			if (this->auth.get_token())
				http.addHeader("Authorization", "Bearer" + this->auth.get_token());

			request(http);

			http.end();
		}

	private:
		inline void remove_utf8_bom_from_json_str(String &json)
		{
			if (json.length() > 3)
			{
				if (json[0] == char(0xEF) && json[1] == char(0xBB) && json[2] == char(0xBF))
				{
					json = json.substring(3);
				}
			}
		}

	public:
		api_client_t (server_ip_t server_ip, server_port_t server_port)
		{
			this->ip = server_ip;
			this->port = server_port;
		}

		api_client_t (server_ip_t server_ip, server_port_t server_port, http_protocol_t http_protocol)
		{
			this->ip = server_ip;
			this->port = server_port;
			this->http_protocol = http_protocol;
		}

	public:
		uint32_t login (user_t user);
		uint32_t login (String username, String password);
		void logout ();
		uint32_t refresh_token ();
		JsonArray get_measurement_types ();
		JsonObject store_measurement (measurement_t measurement);
		JsonObject store_measurement (float value, String measured_at, id_t measurement_type_id);

	public:
		inline String get_server_addr ()
		{
			return this->stringify_http_protocol() + "://" + this->ip + ":" + this->port + "/api";
		}
};

#endif