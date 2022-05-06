#include "api_client.h"

String api_client_t::stringify_http_protocol ()
{
	switch (this->http_protocol) {
		case HTTP_PROTOCOL:
			return "http";
		case HTTPS_PROTOCOL:
			return "https";
	}
}

String api_client_t::stringify_route (route_t route)
{
	switch (route) {
		case LOGIN_ROUTE:
			return "/auth/login";
		case LOGOUT_ROUTE:
			return "/auth/logout";
		case ME_ROUTE:
			return "/auth/me";
		case REFRESH_TOKEN_ROUTE:
			return "/auth/refresh";
		case MEASUREMENT_TYPES_ROUTE:
			return "/measurement-types";
		case PATIENT_MEASUREMENT_ROUTE:
			return "/patient-measurements";
	}
}

DynamicJsonDocument api_client_t::get_request (json_capacity_t capacity, String route)
{
	DynamicJsonDocument doc(capacity);

	this->http_request(route, [&] (HTTPClient http) {
		uint16_t responseCode = http.GET();
		String payload = http.getString();
		remove_utf8_bom_from_json_str(payload);
		deserializeJson(doc, payload);
	});

	return doc;
}

DynamicJsonDocument api_client_t::get_request (json_capacity_t capacity, route_t route)
{
	return this->get_request(
		capacity,
		this->get_server_addr() + this->stringify_route(route)
	);
}

DynamicJsonDocument api_client_t::post_request (json_capacity_t capacity, String route, String body)
{
	DynamicJsonDocument doc(capacity);

	this->http_request(route, [&] (HTTPClient http) {
		uint16_t responseCode = http.POST(body);
		String payload = http.getString();
		remove_utf8_bom_from_json_str(payload);
		deserializeJson(doc, payload);
	});

	return doc;
}

DynamicJsonDocument api_client_t::post_request (json_capacity_t capacity, route_t route, String body)
{
	return this->post_request(
		capacity,
		this->get_server_addr() + this->stringify_route(route),
		body
	);
}

JsonObject api_client_t::get_logged_patient ()
{
	if (this->auth.is_logged())
		return this->post_request(384, ME_ROUTE, "")["data"];
}

uint32_t api_client_t::login (String username, String password)
{
	StaticJsonDocument<96> doc;

	doc["login"] = username;
	doc["password"] = password;

	String body;

	serializeJson(doc, body);

	auto responseDoc = this->post_request(768, LOGIN_ROUTE, body);

	JsonObject responseData = responseDoc["data"];
	this->auth.set_token(responseData["accessToken"].as<String>());

	auto loggedPatient = this->get_logged_patient();
	JsonObject user = loggedPatient["user"];

	this->auth.set_id(loggedPatient["id"].as<id_t>());

	this->patient = patient_t(
		this->auth.get_id(),
		user["name"].as<String>(),
		loggedPatient["email"].as<String>(),
		loggedPatient["phone"].as<String>()
	);
	PRINTLN_L("here6");

	return responseDoc["data"]["expiresIn"];
}

uint32_t api_client_t::login (user_t user)
{
	return this->login(user.username, user.password);
}

void api_client_t::logout () 
{
	if (this->auth.is_logged()) {
		auto responseDoc = this->post_request(128, LOGOUT_ROUTE, "");

		this->auth = auth_t();
		this->patient = patient_t();
	}
}

uint32_t api_client_t::refresh_token () 
{
	if (this->auth.is_logged()) {
		auto responseDoc = this->post_request(768, REFRESH_TOKEN_ROUTE, "");

		JsonObject responseData = responseDoc["data"];
		this->auth.set_token(responseData["accessToken"].as<String>());

		return responseData["expiresIn"];
	}
}

JsonArray api_client_t::get_measurement_types ()
{
	if (this->auth.is_logged())
		return this->get_request(1024, MEASUREMENT_TYPES_ROUTE)["data"];
}

JsonObject api_client_t::store_measurement (float value, String measured_at, id_t measurement_type_id)
{
	if (this->auth.is_logged()) {
		StaticJsonDocument<128> doc;

		doc["value"] = value;
		doc["measuredAt"] = measured_at;
		doc["measurementTypeId"] = measurement_type_id;

		String body;

		serializeJson(doc, body);

		auto route = this->get_server_addr() + this->stringify_route(PATIENT_MEASUREMENT_ROUTE) + "/" + (const char*) this->auth.get_id();
		auto responseDoc = this->post_request(768, route, body);

		return responseDoc["data"];
	}
}

JsonObject api_client_t::store_measurement (measurement_t measurement)
{
	return this->store_measurement(
		measurement.value, 
		measurement.measured_at, 
		measurement.measurement_type_id
	);
}
