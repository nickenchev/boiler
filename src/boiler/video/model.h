#ifndef MODEL_H
#define MODEL_H

class Model
{
    int numVertices;

public:
    Model(int numVertices);

    int getNumVertices() const { return numVertices; }
};


#endif /* MODEL_H */
