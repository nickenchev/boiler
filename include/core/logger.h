#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace Boiler
{

class Logger
{
	static vector<string> buffer;
	string name;
public:
    Logger(std::string name) { this->name = name; }

    void log(std::string message) const
	{
		string entry = "[" + name + "]" + message;
		cout << entry << endl;
	}
    void error(std::string message) const
	{
		string entry = "[ERROR ->" + name + "]" + message;
		cerr << entry << endl;
	}
};

}

#endif /* LOGGER_H */
