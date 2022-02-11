#include "APIClient.h"

String APIClient::getHTTPProtocolString (void)
{
	switch (this->http_protocol) {
		case HTTPProtocol::http:
			return "http";
		case HTTPProtocol::https:
			return "https";
	}
}

String APIClient::getRouteString (Routes route)
{
	switch (route) {
		case Routes::login:
			return "/auth/login";
		case Routes::logout:
			return "/auth/logout";
		case Routes::me:
			return "/auth/me";
		case Routes::refresh_token:
			return "/auth/refresh";
		case Routes::measurement_types:
			return "/measurement-types";
		case Routes::patient_measurement:
			return "/patient-measurements";
	}
}

DynamicJsonDocument APIClient::getRequest (int capacity, String route)
{
	try {
		WiFiClient wifiClient;
		DynamicJsonDocument doc(capacity);

		this->http.useHTTP10(true);
		this->http.begin(wifiClient, route);

		this->http.addHeader("Content-Type", "application/json");

		if (this->auth.token)
			this->http.addHeader("Authorization", "Bearer" + this->auth.token);

		uint16_t responseCode = this->http.GET();

		this->requestErrorHandler(responseCode);

		deserializeJson(doc, this->http.getStream());

		this->requestErrorHandler(responseCode, doc);

		this->http.end();

		return doc;
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception& e) {
		throw e;
	}
}

DynamicJsonDocument APIClient::getRequest (int capacity, Routes route)
{
	try {
		return this->getRequest(
			capacity,
			this->getServerAddress() + this->getRouteString(route)
		);
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception& e) {
		throw e;
	}
}

DynamicJsonDocument APIClient::postRequest (int capacity, String route, String body)
{
	try {
		WiFiClient wifiClient;
		DynamicJsonDocument doc(capacity);

		this->http.useHTTP10(true);
		this->http.begin(wifiClient, route);

		this->http.addHeader("Content-Type", "application/json");

		if (this->auth.token)
			this->http.addHeader("Authorization", "Bearer" + this->auth.token);

		uint16_t responseCode = this->http.POST(body);

		this->requestErrorHandler(responseCode);

		deserializeJson(doc, this->http.getStream());

		this->requestErrorHandler(responseCode, doc);

		this->http.end();

		return doc;
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception& e) {
		throw e;
	}
}

DynamicJsonDocument APIClient::postRequest (int capacity, Routes route, String body)
{
	try {
		return this->postRequest(
			capacity,
			this->getServerAddress() + this->getRouteString(route),
			body
		);
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception& e) {
		throw e;
	}
}

JsonObject APIClient::getLoggedPatient (void)
{
	try {
		this->isPatientLogged();
		return this->postRequest(384, Routes::me, "")["data"];
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}

uint32_t APIClient::login (String username, String password)
{
	try {
		StaticJsonDocument<96> doc;

		doc["login"] = username;
		doc["password"] = password;

		String body;

		serializeJson(doc, body);

		auto responseDoc = this->postRequest(768, Routes::login, body);
		JsonObject responseData = responseDoc["data"];

		this->auth.token = responseData["accessToken"].as<String>();

		auto loggedPatient = this->getLoggedPatient();

		JsonObject user = loggedPatient["user"];

		this->auth.id = loggedPatient["id"].as<int>();

		this->patient = patient_t(
			this->auth.id,
			user["name"].as<String>(),
			loggedPatient["email"].as<String>(),
			loggedPatient["phone"].as<String>()
		);

		return responseDoc["data"]["expiresIn"];
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}

uint32_t APIClient::login (user_t user)
{
	try {
		return this->login(user.username, user.password);
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}

void APIClient::logout (void) 
{
	try {
		this->isPatientLogged();
		auto responseDoc = this->postRequest(128, Routes::logout, "");

		this->auth = auth_t();
		this->patient = patient_t();
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}

uint32_t APIClient::refreshToken (void) 
{
	try {
		this->isPatientLogged();
		auto responseDoc = this->postRequest(768, Routes::refresh_token, "");

		JsonObject responseData = responseDoc["data"];
		this->auth.token = responseData["accessToken"].as<String>();

		return responseData["expiresIn"];
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}

JsonArray APIClient::getMeasurementTypes (void)
{
	try {
		this->isPatientLogged();
		return this->getRequest(1024, Routes::measurement_types)["data"];
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}

JsonObject APIClient::storeMeasurement (float value, String measured_at, uint8_t measurement_type_id)
{
	try {
		this->isPatientLogged();
		StaticJsonDocument<128> doc;

		doc["value"] = value;
		doc["measuredAt"] = measured_at;
		doc["measurementTypeId"] = measurement_type_id;

		String body;

		serializeJson(doc, body);

		auto route = this->getServerAddress() + this->getRouteString(Routes::patient_measurement) + "/" + this->auth.id;
		auto responseDoc = this->postRequest(768, route, body);

		return responseDoc["data"];
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}

JsonObject APIClient::storeMeasurement (measurement_t measurement)
{
	try {
		return this->storeMeasurement(
			measurement.value, 
			measurement.measured_at, 
			measurement.measurement_type_id
		);
	}
	catch (DynamicJsonDocument e) {
		throw e;
	}
	catch (String &e) {
		throw e;
	}
	catch (const std::exception &e) {
		throw e;
	}
}
