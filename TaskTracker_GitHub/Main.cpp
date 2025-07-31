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
	//create a JSON object to store the response
	nlohmann::json jsonData;

	//Token for GitHub API authentication
	std::string token;

	//Creates a CURL object to perform the request
	CURL* curl;
	// Initialize CURL
	curl = curl_easy_init();

	int inputNumber;

	//request user input for GitHub username
	std::string github_username;
	std::string requestedRepoName;
	std::string responseString;

	std::cout << "Welcome to the Mini Task Manager!" << std::endl;
	std::cout << "What would you like to do?" << std::endl << "Options:" << std::endl;
	std::cout << "1. View recent GitHub activity" << std::endl;
	std::cout << "2. Change a repositories view status" << std::endl;
	std::cout << "3. Exit" << std::endl;
	std::cout << "Please select an option (1-3): ";
	std::cin >> inputNumber;

	while (inputNumber > 3 && inputNumber < 1)
	{
		std::cout << "Invalid option. Please select a valid option (1-3): ";
		std::cin >> inputNumber;
	}
	if (inputNumber == 1)
	{
		std::cout << "**********************************" << std::endl;
		std::cout << "*******User Recent Activity*******" << std::endl;
		std::cout << "**********************************" << std::endl;

		std::cout << "Please input your GitHub username: ";
		std::cin.ignore(); // Clear the newline character left in the input buffer
		std::getline(std::cin, github_username);

		if (curl) {
			// Set the URL for the request and add username to the input
			std::string url = "https://api.github.com/users/" + github_username + "/events";
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
				std::cout << "Successfully fetched repositories for user: " << github_username << std::endl;
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
	if (inputNumber == 2)
	{
		std::cout << "**************************************" << std::endl;
		std::cout << "*******Change Repository Status*******" << std::endl;
		std::cout << "**************************************" << std::endl;

		std::cout << "Please input your GitHub username: ";
		std::cin.ignore();
		std::getline(std::cin, github_username);
		std::cout << "Please input the name of the repository you want to change the status of: ";
		std::getline(std::cin, requestedRepoName);

		if (curl)
		{
			
			curl_easy_setopt(curl, CURLOPT_URL, ("https://api.github.com/repos/" + github_username + "/" + requestedRepoName).c_str());
			std::cout << "https://api.github.com/repos/" + github_username + "/" + requestedRepoName;
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET"); // or omit this for GET
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "TaskTracker_GitHub/1.0");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

			// 3) Perform the GET request
			CURLcode res = curl_easy_perform(curl);
			if (res != CURLE_OK) {
				std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
				return 0;
			}
			try {
				jsonData = nlohmann::json::parse(responseString);
			}
			catch (const nlohmann::json::parse_error& e) {
				std::cerr << "JSON parse error: " << e.what() << std::endl;
				return 0;
			}
			if (jsonData["message"] == "Not Found")
			{
				return 0;
			}
				std::cout << "Successfully fetched repository status for user: " << github_username << std::endl;
				// Print the repository status
				std::cout << "Repository: " << jsonData["name"] << "\n";

				bool isPrivate = jsonData["private"];
				if (isPrivate) { std::cout << "Status: " << "Private" << std::endl; }
				else { std::cout << "Status: " << "Public" << std::endl; }

				std::cout << "1.Change the status of this repository: " << std::endl;
				std::cout << "2. Exit" << std::endl;
				std::cin >> inputNumber;
				while (inputNumber > 2 || inputNumber < 1)
				{
					std::cout << "Invalid option. Please select a valid option (1-3): " << std::endl;
					std::cin.ignore();
					std::cin >> inputNumber;
				}
				if (inputNumber == 1)
				{

					std::cout << "Please input your GitHub token: ";
					std::cin.ignore();
					std::getline(std::cin, token);

					struct curl_slist* headers = nullptr;
					headers = curl_slist_append(headers, ("Authorization: token " + token).c_str());
					headers = curl_slist_append(headers, "Accept: application/vnd.github+json");
					headers = curl_slist_append(headers, "Content-Type: application/json");
					curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


					std::cout << "1. Make it private" << std::endl;
					std::cout << "2. Make it public" << std::endl;
					std::cout << "3. Exit" << std::endl;
					std::cout << "Please select an option (1-3): " << std::endl;

					std::cin >> inputNumber;
					while (inputNumber > 3 || inputNumber < 1)
					{
						std::cout << "Invalid option. Please select a valid option (1-3): ";
						std::cin.ignore();
						std::cin >> inputNumber;
					}

					std::string payload;
						if (inputNumber == 1)
						{
							payload = "{\"private\": true}";
						}
						else if (inputNumber == 2)
						{

							payload = "{\"private\": false}";
						}
						else if (inputNumber == 3)
						{
							return 0;
						}

						curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
						CURLcode res = curl_easy_perform(curl);
						if (res != CURLE_OK)
						{
							std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
						}
						else
						{ 
							std::cout << "Successfully changed the status of the repository: " << requestedRepoName << std::endl;
							curl_slist_free_all(headers);

						}
				}
				else if (inputNumber == 2)
				{
					return 0;
				}

				

				

		}

	}
	if (inputNumber == 3)
	{
		std::cout << "Exiting the Mini Task Manager. Goodbye!" << std::endl;
		return 0;
	}
	return 0;
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