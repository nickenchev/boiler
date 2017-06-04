#include <iostream>
#include <string>
#include <boiler.h>
#include "blankpart.h"

int main(int argc, char *argv[])
{
	Boiler::getInstance().initialize(std::make_unique<OpenGLRenderer>(false), 512, 512);

	auto part = std::make_shared<BlankPart>();
	Boiler::getInstance().start(part);

	return 0;
}
