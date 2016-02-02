#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

class Component
{
    std::string name;

public:
    Component(std::string name) : name(name) { }

    void log(std::string message) const;
    void error(std::string message) const;
};


#endif /* COMPONENT_H */
