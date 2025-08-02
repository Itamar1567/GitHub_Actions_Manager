#include "Validate.h"


void Validate::ClearInputBufferIfNeeded() {

	//.rdbuf()->in_avail() checks if there is any input left in the buffer
	if (std::cin.rdbuf()->in_avail() > 0) {
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

void Validate::AskForInput(const std::string& message, std::string& input)
{
	std::cout << message;
	ClearInputBufferIfNeeded(); // Clear the input buffer if needed
	std::getline(std::cin, input); // Get the input from the user
}

int Validate::TryParseInteger(std::string& input)
{
	// Try to convert the input string to an integer
	while (true)
	{
		try
		{
			return std::stoi(input);
		}
		catch (const std::exception&)
		{
			std::cout << "Invalid input. Please enter a number." << std::endl;
			std::cin >> input;
		}
	}
}

bool Validate::ValidateInRange(std::string& inputNumber, int min, int max) {

	int numericalInput = TryParseInteger(inputNumber); // Convert the input to an integer
	while (numericalInput < min || numericalInput > max) {
		std::cout << "Invalid option. Please select a valid option (" << min << "-" << max << "): ";
		std::cin >> inputNumber;
		numericalInput = TryParseInteger(inputNumber); // Keep prompting until a valid input is entered
	}

	ClearInputBufferIfNeeded();
	return true;
}

bool Validate::ValidateJson(const nlohmann::json& json)
{
	if ((json.contains("message") && json["message"] == "null") || (json.contains("status") && json["status"] == "401"))
	{
		std::cout << "\nRepository not found" << std::endl;
		std::cout << "Press Enter to continue..." << std::endl;
		std::cin.get();
		return false; // Return false if the JSON contains an error message
	}
	else
	{
		return true; // Return true if the JSON is valid
	}
}

