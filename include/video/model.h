#ifndef MODEL_H
#define MODEL_H

namespace Boiler
{

class Model
{
    int numVertices;
    int numIndices;

public:
    Model(int numVertices, int numIndices);

    int getNumVertices() const { return numVertices; }
    int getNumIndices() const { return numIndices; }
};

}
#endif /* MODEL_H */
