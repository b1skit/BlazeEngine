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
		Vertex(const glm::vec3& position)
		{
			this->position = position;
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


		// Temp: Make these public for now TODO: Getters/setters!!!!!!!!!!!!!!!!!!!!
		enum
		{
			VERTEX_BUFFER_POSITION,

			VERTEX_BUFFER_SIZE, // Reserved: Number of elements in our buffer
		};

		GLuint vertexArrayObject;
		GLuint vertexArrayBuffers[VERTEX_BUFFER_SIZE];
		unsigned int drawCount;

	protected:


	private:
		Vertex* vertices;
		int numVerts;

		Material* material;
		
	};
}



