#include "CurlSetup.h"
#include "Validate.h"

int main()
{
	CurlSetup* curlSetup = new CurlSetup();
	int a;
	//Token for GitHub API authentication
	std::string token;

	std::string userInput; 
	int numericalInput; 
	//request user input for GitHub username
	std::string github_username;
	std::string requestedRepoName;
	while (true)
	{
		system("cls"); // Clear the console screen
		std::cout << "Welcome to the GitHub Actions Manager!" << std::endl;
		std::cout << "What would you like to do?" << std::endl << "Options:" << std::endl;
		std::cout << "1. View recent GitHub activity" << std::endl;
		std::cout << "2. Change a repositories view status" << std::endl;
		std::cout << "3. Browse a users repositories" << std::endl;
		std::cout << "4. Exit" << std::endl;

		Validate::AskForInput("Response: ", userInput); // Ask for user input
		if (Validate::ValidateInRange(userInput, 1, 4)) { numericalInput = Validate::TryParseInteger(userInput); }
		else { return 0; }

		if (numericalInput == 1)
		{
			system("cls");
			std::cout << "**********************************" << std::endl;
			std::cout << "*******User Recent Activity*******" << std::endl;
			std::cout << "**********************************" << std::endl;
			Validate::AskForInput("Please input your GitHub username: ", github_username); // Ask for user input

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
		else if (numericalInput == 2)
		{
			bool isFoundRepo = false;
			struct curl_slist* headers = nullptr;
			system("cls");

			std::cout << "**************************************" << std::endl;
			std::cout << "*******Change Repository Status*******" << std::endl;
			std::cout << "**************************************" << std::endl;
			while (isFoundRepo == false)
			{
				curlSetup->CleanUp();
				headers = nullptr; // Reset headers for each iteration

				Validate::AskForInput("Please input your GitHub Access token: ", token); // Ask for user input
				Validate::AskForInput("Please input your GitHub username: ", github_username); // Ask for user input
				Validate::AskForInput("Please input the name of the repository you want to change the status of: ", requestedRepoName); // Ask for user input

				if (curlSetup->GetCurlHandle())
				{
					std::string url = "https://api.github.com/repos/" + github_username + "/" + requestedRepoName;
					curlSetup->HeaderSetup(token);
					curlSetup->Setup(url);
					curlSetup->CurlCheckJsonSet();

					isFoundRepo = Validate::ValidateJson(curlSetup->GetJsonData()); // Set the flag to true if the repository is found
				}
				if (isFoundRepo == false)
				{

					std::cout << "1.Continue" << std::endl;
					std::cout << "2.Exit" << std::endl;
					Validate::AskForInput("Response: ", userInput); // Ask for user input
					if (Validate::ValidateInRange(userInput, 1, 2)) { numericalInput = Validate::TryParseInteger(userInput); }
					if (numericalInput == 1)
					{
						continue; // Continue the loop to ask for the repository name again
					}
					else if (numericalInput == 2)
					{
						return 0; // Exit the program if the user chooses to exit
					}

				}
			}

			std::cout << "Successfully fetched repository status for user: " << github_username << std::endl;

			std::cout << "Repository: " << curlSetup->GetJsonData()["name"] << "\n"; 

			bool isPrivate = curlSetup->GetJsonData()["private"]; // Print the repository status

			if (isPrivate) { std::cout << "Status: " << "Private" << std::endl; }
			else { std::cout << "Status: " << "Public" << std::endl; }

			std::cout << "1.Change the status of this repository: " << std::endl;
			std::cout << "2. Exit" << std::endl;

			Validate::AskForInput("Response: ", userInput); // Ask for user input
			if (Validate::ValidateInRange(userInput, 1, 4)) { numericalInput = Validate::TryParseInteger(userInput); }

			if (numericalInput == 1)
			{
				std::cout << "1. Make it private" << std::endl;
				std::cout << "2. Make it public" << std::endl;
				std::cout << "3. Exit" << std::endl;
				Validate::AskForInput("Response: ", userInput); // Ask for user input
				if (Validate::ValidateInRange(userInput, 1, 3)) { numericalInput = Validate::TryParseInteger(userInput); }

				std::string payload;

				if (numericalInput == 3){ std::cout << "Please Wait..." << std::endl; }
				else
				{
					// Prepare the payload for the PATCH request based on user input
					if (numericalInput == 1){ payload = "{\"private\": true}"; }
					else if (numericalInput == 2){ payload = "{\"private\": false}"; }

					curlSetup->PerformPatchRequest(payload); // Perform the PATCH request to change the repository status
					std::cout << "Successfully changed the status of the repository: " << requestedRepoName << std::endl;
				}
				
				curlSetup->CleanUp(); // Clean up the CURL resources after use
				std::cout << "Press Enter to continue..." << std::endl;
				std::cin.get();
				system("cls");

			}
			else if (numericalInput == 2)
			{
				std::cout << "Please Wait..." << std::endl;
				curlSetup->CleanUp(); // Clean up the CURL resources after use
				std::cout << "Press Enter to continue..." << std::endl;
				std::cin.get();
				system("cls");
			}
		}
		else if (numericalInput == 3)
		{
			if (curlSetup->GetCurlHandle())
			{
				bool isFoundRepo = false;

				std::string url;
				while (isFoundRepo == false)
				{

					curlSetup->CleanUp(); // Clean up the CURL resources before each new request
					system("cls");

					std::cout << "Would you like to access private repositories? (Requieres a token): " << std::endl;
					std::cout << "1: Access Private and Public Repositories" << std::endl;;
					std::cout << "2: Access Only Public Repositories" << std::endl;;
					std::cout << "3: Exit" << std::endl;

					Validate::AskForInput("Response: ", userInput); // Ask for user input
					if (Validate::ValidateInRange(userInput, 1, 3)) { numericalInput = Validate::TryParseInteger(userInput); }

					if (numericalInput == 1)
					{
						url = "https://api.github.com/user/repos";
						Validate::AskForInput("Enter Access Token: ", token); // Ask for user input
						curlSetup->HeaderSetup(token);
					}
					if (numericalInput == 2)
					{
						Validate::AskForInput("Enter GitHub username: ", github_username); // Ask for user input
						url = "https://api.github.com/users/" + github_username + "/repos";
					}
					if (numericalInput == 3)
					{
						curlSetup->CleanUp(); // Clean up the CURL resources before exiting
						std::cout << "Exiting to main menu..." << std::endl;
						std::cout << "Press Enter to continue..." << std::endl;
						std::cin.get(); // Wait for user input before clearing the screen
						continue; // Exit the loop if the user chooses to exit
					}

					curlSetup->Setup(url);
					curlSetup->CurlCheckJsonSet();

					isFoundRepo = Validate::ValidateJson(curlSetup->GetJsonData()); // Set the flag to true if the repository is found
					
					if (isFoundRepo == false)
					{

						std::cout << "1.Continue" << std::endl;
						std::cout << "2.Exit" << std::endl;
						Validate::AskForInput("Response: ", userInput); // Ask for user input
						if (Validate::ValidateInRange(userInput, 1, 2)) { numericalInput = Validate::TryParseInteger(userInput); }
						if (numericalInput == 1)
						{
							continue; // Continue the loop to ask for the repository name again
						}
						else if (numericalInput == 2)
						{
							return 0; // Exit the program if the user chooses to exit
						}

					}

				}
				int i = 1;
				for (const auto& data : curlSetup->GetJsonData())
				{
					std::string repoName = data["name"];
					std::cout << std::to_string(i) + ": " + repoName << std::endl;
					i++;
				}

				curlSetup->CleanUp(); // Clean up the CURL resources before exiting
				std::cout << "Press Enter to continue..." << std::endl;
				std::cin.get(); // Wait for user input before clearing the screen
				continue; // Exit the loop if the user chooses to exit
			}
		}
		else if (numericalInput == 4){
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