#ifndef COMPONENT_H
#define COMPONENT_H

class Engine;

class Component
{
    const Engine &engine;
protected:
    const Engine &getEngine() const { return engine; }
public:
    Component(const Engine &engine) : engine(engine) { }
};


#endif /* COMPONENT_H */
