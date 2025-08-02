#define CURL_STATICLIB
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>

class CurlSetup {

private:

	CURL* curl; // CURL handle for performing requests
	std::string responseString; // String to store the response from the server
	nlohmann::json jsonData; // JSON object to store the response
	struct curl_slist* headers = nullptr;


public:

	CurlSetup();
	static size_t WriteCallback(void*, size_t, size_t, std::string*);
	void Setup(std::string&);
	void CleanUp();
	void HeaderSetup(std::string&);
	void PerformPatchRequest(std::string&);
	int CurlCheckJsonSet();
	int SetupJson();
	nlohmann::json GetJsonData();
	CURL* GetCurlHandle();

};