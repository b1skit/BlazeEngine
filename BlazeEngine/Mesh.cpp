#include "Mesh.h"

namespace BlazeEngine
{
	// Vertex functions:


	// Mesh functions:
	Mesh::Mesh(Vertex* vertices, unsigned int numVerts, Material* material)
	{
		this->vertices = vertices;
		this->numVerts = numVerts;

		this->material = material;
	}

	Mesh::~Mesh()
	{

	}

}


