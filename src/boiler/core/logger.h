#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>

class Logger
{
	std::string name;
public:
    Logger(std::string name) { this->name = name; }

    void log(std::string message) const
	{
		std::cout << "[" << name << "] " << message << std::endl;
	}
    void error(std::string message) const
	{
		std::cerr << "[" << name << " ERROR] " << message << std::endl;
	}
};


#endif /* LOGGER_H */
