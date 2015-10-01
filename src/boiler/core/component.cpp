#include <iostream>
#include "component.h"

void Component::log(std::string message) const
{
    std::cout << "[" << name << "] " << message << std::endl;
}

void Component::error(std::string message) const
{
    std::cerr << "[" << name << " ERROR] " << message << std::endl;
}
