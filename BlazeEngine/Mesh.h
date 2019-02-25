#pragma once

#include "Material.h"

#include <glm.hpp>
#include <GL/glew.h>

//using glm::vec2;
using glm::vec3;
using glm::vec4;


namespace BlazeEngine
{
	struct Vertex
	{
	public:
		// TO DO: Figure out an elegant way to initialize vertices with all of their attributes...
		Vertex()
		{
			position = vec3(0, 0, 0); 
		}

		// Explicit constructor:
		/*Vertex(const glm::vec3& position)*/
		Vertex(glm::vec3 const& position)
		{
			this->position = position;
			
		}

		// Copy constructor:
		Vertex(const Vertex& vertex)
		{
			this->position = vertex.position;

		}

	protected:


	private:
		vec3 position;
		/*vec4 color;*/
		/*vec2 UV;*/

	};


	class Mesh
	{
	public:
		Mesh(Vertex* vertices, unsigned int numVerts);
		~Mesh();

		// Copy constructor:
		Mesh(const Mesh& mesh)
		{
			this->vertices = mesh.vertices;
			this->numVerts = mesh.numVerts;

			this->material = mesh.material;
		}

		// Getters/Setters:
		inline Vertex* Vertices() { return vertices; }
		inline unsigned int NumVerts() { return this->numVerts; }


	protected:


	private:
		Vertex* vertices;
		unsigned int numVerts;

		Material* material;
	};
}



