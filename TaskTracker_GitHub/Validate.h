#include <iostream>
#include <nlohmann/json.hpp>

class Validate {

private:

public:

	void ValidateInput(int&, int, int);
	bool ValidateJson(const nlohmann::json&);



};