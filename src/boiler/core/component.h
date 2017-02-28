#ifndef COMPONENT_H
#define COMPONENT_H

#define COMPONENT_POSITION 1

class Component
{
	const int id;
public:
	Component(int id) : id(id) { }
	virtual ~Component() { }

	int getId() const { return id; }
};

#endif /* COMPONENT_H */
