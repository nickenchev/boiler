#ifndef PART_H
#define PART_H

class Engine;

class Part
{
protected:
    Engine *engine;

public:
    Part(Engine *engine) : engine(engine) { }
    virtual void start() = 0;
    virtual void update(const float delta) = 0;
    virtual void render() = 0;

    const Engine *getEngine() const { return engine; }
};

#endif /* PART_H */
