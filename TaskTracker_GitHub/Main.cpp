#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
	size_t totalSize = size * nmemb;
	s->append((char*)contents, totalSize);
	return totalSize;
}

int main()
{
	std::string input;
	std::string responseString;
	std::cout << "Welcome to the Task Tracker!" << std::endl;
	std::cout << "Please input your GitHub username: ";
	std::getline(std::cin, input);
	CURL* curl;
	curl = curl_easy_init();
	nlohmann::json jsonData;
	if (curl) {
		std::string url = "https://api.github.com/users/" + input + "/events";
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "TaskTracker/1.0");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else {
			std::cout << "Successfully fetched repositories for user: " << input << std::endl;
			jsonData = nlohmann::json::parse(responseString);
		}

	}
	else {
		std::cerr << "Failed to initialize CURL." << std::endl;
	}
	try
	{
		for (const auto& data : jsonData)
		{
			std::string type = data["type"];
			std::string repo = data["repo"]["name"];
			if (type == "PushEvent") {

				int commits = data["payload"]["commits"].size();
				std::cout << "- Pushed " << commits << " commits to " << repo << "\n";

				for (const auto& commit : data["payload"]["commits"]) {
					std::string message = commit["message"];
					std::string url = commit["url"];
					std::string sha = url.substr(url.find_last_of('/') + 1); // Extract SHA from URL
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
			std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Sleep for 1 second to avoid overwhelming the output
		}
	}
	catch (std::exception& e) {
		std::cerr << "Failed to parse JSON: " << e.what() << "\n";
	}
}

