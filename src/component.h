#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>

class Engine;

class Component
{
    std::string name;
    const Engine &engine;
protected:
    const Engine &getEngine() const { return engine; }
public:
    Component(std::string name, const Engine &engine) : name(name), engine(engine) { }

    void log(std::string message) const;
    void error(std::string message) const;
};


#endif /* COMPONENT_H */