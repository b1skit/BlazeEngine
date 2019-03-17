#include "Mesh.h"

namespace BlazeEngine
{
	// Vertex functions:


	// Mesh functions:
	Mesh::Mesh(Vertex* vertices, unsigned int numVerts, GLubyte* indices, unsigned int numIndices, unsigned int materialIndex)
	{
		this->vertices = vertices;
		this->numVerts = numVerts;

		this->indices = indices;
		this->numIndices = numIndices;

		this->materialIndex = materialIndex;

		this->transform = nullptr; // Must be initialized by calling SetTransform();
	}

	Mesh::~Mesh()
	{

	}

}


