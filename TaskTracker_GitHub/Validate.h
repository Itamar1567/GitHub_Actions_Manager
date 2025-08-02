
#include <iostream>
#include <nlohmann/json.hpp>

class Validate {

private:

public:
	static void ClearInputBufferIfNeeded();
	static void AskForInput(const std::string&, std::string&);
	static int TryParseInteger(std::string&);
	static bool ValidateInRange(std::string&, int, int);
	static bool ValidateJson(const nlohmann::json&);
};
