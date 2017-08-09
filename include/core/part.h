#ifndef PART_H
#define PART_H

class Part
{
public:
	Part() { }
    // disable copying
    Part(const Part &part) = delete;
    Part &operator=(const Part &part) = delete;

    virtual void onStart() = 0;
};

#endif /* PART_H */