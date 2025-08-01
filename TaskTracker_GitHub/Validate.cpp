#include "Validate.h"

void Validate::ValidateInput(int& inputNumber, int min, int max) {
	while (inputNumber < min || inputNumber > max) {
		std::cout << "Invalid option. Please select a valid option (" << min << "-" << max << "): ";
		std::cin >> inputNumber;
	}
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