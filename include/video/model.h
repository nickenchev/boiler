#ifndef MODEL_H
#define MODEL_H

namespace Boiler
{

class Model
{
    unsigned int vertexCount;
    unsigned int indexCount;

public:
    Model(unsigned int vertexCount, unsigned int indexCount);

	unsigned int getVertexCount() const { return vertexCount; }
	unsigned int getIndexCount() const { return indexCount; }
};

}
#endif /* MODEL_H */
