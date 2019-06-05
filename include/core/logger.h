#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>

namespace Boiler
{

class Logger
{
	static std::vector<std::string> buffer;
	std::string name;
public:
    Logger(std::string name) { this->name = name; }

    void log(std::string message) const
	{
		std::string entry = "[" + name + "] " + message;
		std::cout << entry << std::endl;
	}

	void log(double value) const
	{
		log(std::to_string(value));
	}

    void error(std::string message) const
	{
		std::string entry = "[ERROR ->" + name + "] " + message;
		std::cerr << entry << std::endl;
	}
};

}

#endif /* LOGGER_H */
