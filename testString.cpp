#include <iostream>
#include <sstream>

const std::string test_string(void);

int main(void)
{
	std::cout << test_string() << std::endl;
	return (0);
}

const std::string test_string(void)
{
	std::string string = "";
	std::string firstPart = "";
	std::string lastPart = "";
	std::stringstream ss;
	int nb = 10;
	
	firstPart += "first";
	lastPart += "last";
	
	ss << nb;
	string += firstPart + " "+ ss.str() + " "  + lastPart;
	return (string);
}
