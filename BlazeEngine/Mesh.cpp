#include "Mesh.h"

namespace BlazeEngine
{
	// Vertex functions:


	// Mesh functions:
	Mesh::Mesh(Vertex* vertices, unsigned int numVerts)
	{
		this->vertices = vertices;
		this->numVerts = numVerts;
	}

	Mesh::~Mesh()
	{
		/*if (vertices)
		{
			delete vertices;
		}*/
	}

}


