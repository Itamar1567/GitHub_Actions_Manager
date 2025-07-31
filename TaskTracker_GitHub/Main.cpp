#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>

//Point to the content, size of data chunck recieved, amount of data received, point to string to store the response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
	//total bytes of data received
	size_t totalSize = size * nmemb;
	//Adds resault to the string
	s->append((char*)contents, totalSize);
	// returns the total size of the data received so libcul doesent think something went wrong
	return totalSize;
}

int main()
{
	//request user input for GitHub username
	std::string input;
	std::string responseString;
	std::cout << "Welcome to the Task Tracker!" << std::endl;
	std::cout << "Please input your GitHub username: ";
	std::getline(std::cin, input);
	//Creates a CURL object to perform the request
	CURL* curl;
	// Initialize CURL
	curl = curl_easy_init();
	//create a JSON object to store the response
	nlohmann::json jsonData;
	if (curl) {
		// Set the URL for the request and add username to the input
		std::string url = "https://api.github.com/users/" + input + "/events";
		// Give the URL to CURL
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		// Set the ser-Agent header to avoid 403 Forbidden error
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "TaskTracker/1.0");
		// Set the write function to handle the response
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		// Set the write data to store the response in a string
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else {
			std::cout << "Successfully fetched repositories for user: " << input << std::endl;
			// Parse the response string into a JSON object
			jsonData = nlohmann::json::parse(responseString);
		}

	}
	else {
		std::cerr << "Failed to initialize CURL." << std::endl;

	}

	// Clean up CURL resources since we are done with the request
	curl_easy_cleanup(curl);

	try
	{
		// Iterate through the JSON data and print the relevant information
		for (const auto& data : jsonData)
		{
			//Get the type of event in order to get the array of commits, messages, etc.
			std::string type = data["type"];
			std::string repo = data["repo"]["name"];
			if (type == "PushEvent") {

				//Get commit amount from the payload
				int commits = data["payload"]["commits"].size();
				std::cout << "- Pushed " << commits << " commits to " << repo << "\n";

				//Iterate through the commits and print the commit message and URL
				for (const auto& commit : data["payload"]["commits"]) {
					std::string message = commit["message"];
					std::string url = commit["url"];
					//Get the commit SHA from the URL
					std::string sha = url.substr(url.find_last_of('/') + 1); // Extract SHA from URL
					// Construct the GitHub URL for the commit
					std::string gitHubUrl = "https://github.com/" + repo + "/commit/" + sha;
					std::cout << "- Commit message: " << message << "\n";
					std::cout << "- Commit URL: " << gitHubUrl << "\n";
				}
				
			}
			else if (type == "IssuesEvent") {
				std::string action = data["payload"]["action"];
				std::cout << "- " << action << " an issue in " << repo << "\n";
			}
			else if (type == "WatchEvent") {
				std::cout << "- Starred " << repo << "\n";
			}
			else {
				std::cout << "- " << type << " in " << repo << "\n";
			}
			//Wait for a second before printing the next event to avoid overwhelming the output
			std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Sleep for 1 second to avoid overwhelming the output
		}
	}
	catch (std::exception& e) {
		std::cerr << "Failed to parse JSON: " << e.what() << "\n";
	}
}

//JSON Schema for the response

/*
* 
{
	"type": "array",
		"items" : {
		"title": "Event",
			"description" : "Event",
			"type" : "object",
			"properties" : {
			"id": {
				"type": "string"
			},
				"type" : {
				"type": [
					"string",
					"null"
				]
			},
}

*/