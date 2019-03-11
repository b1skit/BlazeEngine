#include "Mesh.h"

namespace BlazeEngine
{
	// Vertex functions:


	// Mesh functions:
	/*Mesh::Mesh(Vertex* vertices, unsigned int numVerts, GLubyte* indices, unsigned int numIndices, Material* material)*/
	Mesh::Mesh(Vertex* vertices, unsigned int numVerts, Material* material)
	{
		this->vertices = vertices;
		this->numVerts = numVerts;

		//this->indices = indices;
		//this->numIndices = numIndices;

		this->material = material;
	}

	Mesh::~Mesh()
	{

	}

}


