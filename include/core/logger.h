#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <fmt/core.h>

namespace Boiler
{

class Logger
{
	const std::string name;

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
    Logger(std::string name) : name(name) { }

    void log(std::string message) const
	{
		printInfo(message);
	}

	template<typename... Args>
	constexpr void log(fmt::format_string<Args...> fmtString, Args&&... args) const
	{
		printInfo(fmt::format(fmtString, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void error(fmt::format_string<Args...> fmtString, Args&&... args) const
	{
		printError(fmt::format(fmtString, std::forward<Args>(args)...));
	}

    void error(std::string message) const
	{
		printError(message);
	}
};

}

#endif /* LOGGER_H */
