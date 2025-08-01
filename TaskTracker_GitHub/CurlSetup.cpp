#include "CurlSetup.h"

CurlSetup::CurlSetup(){

	curl = curl_easy_init(); // Initialize CURL
	responseString.clear(); // Clear the response string
	jsonData = nlohmann::json::object(); // Clear the JSON data
	headers = nullptr; // Initialize headers to nullptr


	if (!curl) {
		std::cerr << "Failed to initialize CURL" << std::endl;
		exit(EXIT_FAILURE);
	}
}

size_t CurlSetup::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
	//total bytes of data received
	size_t totalSize = size * nmemb;
	//Adds resault to the string
	s->append((char*)contents, totalSize);
	// returns the total size of the data received so libcul doesent think something went wrong
	return totalSize;
}

void CurlSetup::Setup(std::string& urlRef)
{
	curl_easy_setopt(curl, CURLOPT_URL, urlRef.c_str());

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	// Set the ser-Agent header to avoid 403 Forbidden error
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "TaskTracker/1.0");
	// Set the write function to handle the response
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	// Set the write data to store the response in a string
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
}

void CurlSetup::CleanUp()
{
	if (curl) {
		curl_easy_cleanup(curl); // Clean up the CURL handle
		curl = curl_easy_init();
	}
	if (headers)
	{
		curl_slist_free_all(headers); // Free the headers list
		headers = nullptr; // Reset headers to nullptr
	}
	responseString.clear(); // Clear the response string for the next request
	jsonData.clear(); // Clear the JSON data for the next request
}

void CurlSetup::HeaderSetup(std::string& token)
{
	headers = curl_slist_append(headers, ("Authorization: token " + token).c_str()); // Add the Authorization header with the token
	headers = curl_slist_append(headers, "Accept: application/vnd.github+json"); // Add the Accept header to specify the response format
	headers = curl_slist_append(headers, "Content-Type: application/json"); // Add the Content-Type header to specify the request format
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // Set the headers for the CURL request
}

void CurlSetup::PerformPatchRequest(std::string& payload)
{
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH"); // Set the request method to PATCH
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str()); //Pass the requested payload to the server
	curl_easy_perform(curl);
}

int CurlSetup::CurlCheckJsonSet()
{
	CURLcode res = curl_easy_perform(curl);

	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		return 0; // Return 0 to indicate failure
	}
	else {
		SetupJson(); // Call SetupJson to parse the response string into jsonData
		return 1;
	}
}

int CurlSetup::SetupJson() {
	
	try {
		jsonData = nlohmann::json::parse(responseString);
	}
	catch (const nlohmann::json::parse_error& e) {
		std::cerr << "JSON parse error: " << e.what() << std::endl;
		return 0; // Return 0 to indicate failure
	}
}

nlohmann::json CurlSetup::GetJsonData() {
	return jsonData; // Return the parsed JSON data
}

CURL* CurlSetup::GetCurlHandle() {
	return curl; // Return the CURL handle
}