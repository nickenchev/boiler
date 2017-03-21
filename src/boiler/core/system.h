#ifndef SYSTEM_H
#define SYSTEM_H

class System
{
public:
    System();
    virtual ~System();

	virtual void execute() = 0;
};

#endif /* SYSTEM_H */
