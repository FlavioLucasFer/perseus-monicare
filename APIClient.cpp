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

DynamicJsonDocument APIClient::getRequest (int capacity, Routes r)
{
	WiFiClient wifiClient;
	DynamicJsonDocument doc(capacity);
	auto route = this->getServerAddress() + this->getRouteString(r);

	this->http.useHTTP10(true);
	this->http.begin(wifiClient, route);

	this->http.addHeader("Content-Type", "application/json");

	if (this->patient.token)
		this->http.addHeader("Authorization", "Bearer" + this->patient.token);

	auto responseCode = this->http.GET();

	if (responseCode <= 0)
		throw "Unexpected error";

	deserializeJson(doc, this->http.getStream());

	this->http.end();

	return doc;
}

DynamicJsonDocument APIClient::getRequest (int capacity, String route)
{
	WiFiClient wifiClient;
	DynamicJsonDocument doc(capacity);

	this->http.useHTTP10(true);
	this->http.begin(wifiClient, route);

	this->http.addHeader("Content-Type", "application/json");

	if (this->patient.token)
		this->http.addHeader("Authorization", "Bearer" + this->patient.token);

	auto responseCode = this->http.GET();

	if (responseCode <= 0)
		throw "Unexpected error";

	deserializeJson(doc, this->http.getStream());

	this->http.end();

	return doc;
}

DynamicJsonDocument APIClient::postRequest (int capacity, Routes r, String body)
{
	WiFiClient wifiClient;
	DynamicJsonDocument doc(capacity);
	auto route = this->getServerAddress() + this->getRouteString(r);

	this->http.useHTTP10(true);
	this->http.begin(wifiClient, route);

	this->http.addHeader("Content-Type", "application/json");
	
	if (this->patient.token)
		this->http.addHeader("Authorization", "Bearer" + this->patient.token);

	auto responseCode = this->http.POST(body);

	if (responseCode <= 0)
		throw "Unexpected error";

	deserializeJson(doc, this->http.getStream());

	this->http.end();

	return doc;
}

DynamicJsonDocument APIClient::postRequest (int capacity, String route, String body)
{
	WiFiClient wifiClient;
	DynamicJsonDocument doc(capacity);

	this->http.useHTTP10(true);
	this->http.begin(wifiClient, route);

	this->http.addHeader("Content-Type", "application/json");

	if (this->patient.token)
		this->http.addHeader("Authorization", "Bearer" + this->patient.token);

	auto responseCode = this->http.POST(body);

	if (responseCode <= 0)
		throw "Unexpected error";

	deserializeJson(doc, this->http.getStream());

	this->http.end();

	return doc;
}

DynamicJsonDocument APIClient::getLoggedUser (void)
{
	return this->postRequest(384, Routes::me, "");
}

DynamicJsonDocument APIClient::login (String username, String password)
{
	StaticJsonDocument<96> doc;

	doc["login"] = username;
	doc["password"] = password;

	String body;

	serializeJson(doc, body);

	auto responseDoc = this->postRequest(768, Routes::login, body);
	JsonObject responseData = responseDoc["data"];

	this->patient.token = responseData["accessToken"].as<String>();

	auto loggedUser = this->getLoggedUser();

	JsonObject loggedUserData = loggedUser["data"];
	JsonObject user = loggedUserData["user"];

	this->patient.id = loggedUserData["id"].as<int>();
	this->patient.name = user["name"].as<String>();

	return responseDoc;
}

DynamicJsonDocument APIClient::logout (void) 
{
	auto responseDoc = this->postRequest(128, Routes::logout, "");

	this->patient = patient_t();

	return responseDoc;
}

DynamicJsonDocument APIClient::refreshToken (void) 
{
	auto responseDoc = this->postRequest(768, Routes::refresh_token, "");

	JsonObject responseData = responseDoc["data"];
	this->patient.token = responseData["accessToken"].as<String>();

	return responseDoc;
}

DynamicJsonDocument APIClient::getMeasurementTypes (void)
{
	return this->getRequest(1024, Routes::measurement_types);
}

DynamicJsonDocument APIClient::setMeasurement (float value, String measured_at, int measurement_type_id)
{
	StaticJsonDocument<128> doc;

	doc["value"] = value;
	doc["measuredAt"] = measured_at;
	doc["measurementTypeId"] = measurement_type_id;

	String body;

	serializeJson(doc, body);

	auto route = this->getServerAddress() + this->getRouteString(Routes::patient_measurement) + "/" + this->patient.id;
	auto responseDoc = this->postRequest(768, route, body);

	return responseDoc;
}
