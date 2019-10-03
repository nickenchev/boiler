#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <fmt/format.h>

namespace Boiler
{

class Logger
{
	static std::vector<std::string> buffer;
	std::string name;

	void printInfo(const std::string &message) const
	{
		std::string entry = "[" + name + "] " + message;
		std::cout << entry << std::endl;
	}

	void printError(const std::string &message) const
	{
		std::string entry = "[" + name + "] ** ERROR: " + message;
		std::cerr << entry << std::endl;
	}

public:
    Logger(std::string name) { this->name = name; }

    void log(std::string message) const
	{
		printInfo(message);
	}

	template<typename... Args>
	void log(std::string formatString, Args&&... args)
	{
		printInfo(fmt::format(formatString, args...));
	}

	void log(double value) const
	{
		log(std::to_string(value));
	}

    void error(std::string message) const
	{
		printError(message);
	}
};

}

#endif /* LOGGER_H */
