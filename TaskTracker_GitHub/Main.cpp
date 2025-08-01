#include "CurlSetup.h"

int main()
{
	CurlSetup* curlSetup = new CurlSetup();

	//Token for GitHub API authentication
	std::string token;

	int inputNumber;

	//request user input for GitHub username
	std::string github_username;
	std::string requestedRepoName;
	std::string responseString;
	while (true)
	{

		std::cout << "Welcome to the Mini Task Manager!" << std::endl;
		std::cout << "What would you like to do?" << std::endl << "Options:" << std::endl;
		std::cout << "1. View recent GitHub activity" << std::endl;
		std::cout << "2. Change a repositories view status" << std::endl;
		std::cout << "3. Browse a users repositories" << std::endl;
		std::cout << "4. Exit" << std::endl;
		std::cout << "Please select an option (1-3): ";
		std::cin >> inputNumber;

		while (inputNumber > 3 && inputNumber < 1)
		{
			std::cout << "Invalid option. Please select a valid option (1-3): ";
			std::cin >> inputNumber;
		}
		if (inputNumber == 1)
		{
			std::cin.ignore(); // Clear the newline character from the input buffer
			system("cls");
			std::cout << "**********************************" << std::endl;
			std::cout << "*******User Recent Activity*******" << std::endl;
			std::cout << "**********************************" << std::endl;
			std::cout << "Please input your GitHub username: ";
			std::getline(std::cin, github_username);

			if (curlSetup->GetCurlHandle()) {
				// Set the URL for the request and add username to the input
				std::string url = "https://api.github.com/users/" + github_username + "/events";
				curlSetup->Setup(url);
				curlSetup->CurlCheckJsonSet();
			}
			else {
				std::cerr << "Failed to initialize CURL." << std::endl;
			}
			try
			{
				// Iterate through the JSON data and print the relevant information
				for (const auto& data : curlSetup->GetJsonData())
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
					
					std::cout << std::endl; // Print a newline for better readability
					//Wait for a second before printing the next event to avoid overwhelming the output
					std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Sleep for 1 second to avoid overwhelming the output
				}
			}
			catch (std::exception& e) {
				std::cerr << "Failed to parse JSON: " << e.what() << "\n";
			}

			curlSetup->CleanUp(); // Clean up the CURL resources after use
			std::cout << "Press Enter to continue..." << std::endl;
			std::cin.get(); // Wait for user input before clearing the screen
			system("cls");
		}
		else if (inputNumber == 2)
		{
			bool isFoundRepo = false;
			struct curl_slist* headers = nullptr;
			std::cin.ignore();
			system("cls");

			std::cout << "**************************************" << std::endl;
			std::cout << "*******Change Repository Status*******" << std::endl;
			std::cout << "**************************************" << std::endl;
			while (isFoundRepo == false)
			{
				headers = nullptr; // Reset headers for each iteration

				std::cout << "Please input your GitHub token: ";
				std::getline(std::cin, token);
				std::cout << "Please input your GitHub username: ";
				std::getline(std::cin, github_username);
				std::cout << "Please input the name of the repository you want to change the status of: ";
				std::getline(std::cin, requestedRepoName);

				if (curlSetup->GetCurlHandle())
				{

					std::string url = "https://api.github.com/repos/" + github_username + "/" + requestedRepoName;
					curlSetup->HeaderSetup(token);
					curlSetup->Setup(url);
					curlSetup->CurlCheckJsonSet();

					if (curlSetup->GetJsonData()["message"] == "Not Found" || curlSetup->GetJsonData()["status"] == "401")
					{
						std::cout << "\nRepository not found" << std::endl;
						curlSetup->CleanUp();
					}
					else
					{
						isFoundRepo = true; // Set the flag to true if the repository is found
					}
				}

			}

			std::cout << "Successfully fetched repository status for user: " << github_username << std::endl;
			// Print the repository status
			std::cout << "Repository: " << curlSetup->GetJsonData()["name"] << "\n";

			bool isPrivate = curlSetup->GetJsonData()["private"];
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
				if (inputNumber == 3)
				{
					std::cout << "Please Wait..." << std::endl;
				}
				else
				{
					if (inputNumber == 1)
					{
						payload = "{\"private\": true}";
					}
					else if (inputNumber == 2)
					{
						payload = "{\"private\": false}";
					}

					curlSetup->PerformPatchRequest(payload); // Perform the PATCH request to change the repository status
					std::cout << "Successfully changed the status of the repository: " << requestedRepoName << std::endl;

				}
				
				

				
				curlSetup->CleanUp(); // Clean up the CURL resources after use
				std::cout << "Press Enter to continue..." << std::endl;
				std::cin.ignore(); // Wait for user input before clearing the screen
				std::cin.get();
				system("cls");

			}
			else if (inputNumber == 2)
			{
				std::cout << "Please Wait..." << std::endl;
				curlSetup->CleanUp(); // Clean up the CURL resources after use
				std::cout << "Press Enter to continue..." << std::endl;
				std::cin.ignore(); // Wait for user input before clearing the screen
				std::cin.get();
				system("cls");
			}
		}
		else if (inputNumber == 3)
		{

			if (curlSetup->GetCurlHandle())
			{
				std::cout << "Enter GitHub username: ";
				std::cin.ignore(); // Clear the newline character from the input buffer
				std::getline(std::cin, github_username);

				std::cout << "Would you like to access private repositories? (Requieres a token): ";
				std::cout << "1: Access Private and Public Repositories";
				std::cout << "2: Access Only public Repositories";
				std::cout << "3: Exit";

				std::cin >> inputNumber;

				//if(inputNumber == 0)
				std::string url = "https://api.github.com/users/" + github_username + "/repos";
				curlSetup->Setup(url);
				curlSetup->CurlCheckJsonSet();
				std::cout << "Repositories for user: " << github_username << std::endl << std::endl;
				for (const auto& data : curlSetup->GetJsonData())
				{
					std::string repoName = data["name"];
					std::cout << repoName << std::endl;
				}
			}
		}
		else if (inputNumber == 4)
		{
			std::cout << "Exiting the Mini Task Manager. Goodbye!" << std::endl;
			return 0;
		}

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