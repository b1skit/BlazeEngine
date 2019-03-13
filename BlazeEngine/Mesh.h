#pragma once

#include "Material.h"

#include <glm.hpp>
#include <GL/glew.h>

//using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::vec2;


namespace BlazeEngine
{
	struct Vertex
	{
	public:
		// TO DO: Figure out an elegant way to initialize vertices with all of their attributes...
		Vertex()
		{
			position = vec3(0.0f, 0.0f, 0.0f); 
			normal = vec3(0.0f, 0.0f, 0.0f);
			color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			uv0 = vec2(0.0f, 0.0f);
		}

		 //Explicit constructor:
		Vertex(const vec3& position, const vec3& normal, const vec4& color, const vec2& uv0)
		{
			this->position = position;
			this->normal = normal;
			this->color = color;
			this->uv0 = uv0;
		}

		// Copy constructor:
		Vertex(const Vertex& vertex) = default;
		/*{
			this->position = vertex.position;
		}*/

		vec3 position;
		vec3 normal;
		vec4 color;
		vec2 uv0;
	protected:


	private:


	};


	class Mesh
	{
	public:
		Mesh(Vertex* vertices, unsigned int numVerts, GLubyte* indices, unsigned int numIndices, Material* material);
		/*Mesh(Vertex* vertices, unsigned int numVerts, Material* material);*/
		~Mesh();

		// Copy constructor:
		Mesh(const Mesh& mesh) = default;

		// Getters/Setters:
		inline Vertex* Vertices() { return vertices; }
		inline unsigned int NumVerts() { return this->numVerts; }
		inline Material* GetMaterial() { return material; }
		inline GLubyte* Indices() { return indices; }
		inline unsigned int NumIndices() { return numIndices; }


	protected:


	private:
		Vertex* vertices = nullptr;		// Deallocated in SceneManager.Shutdown()
		unsigned int numVerts;

		GLubyte* indices = nullptr;
		unsigned int numIndices;

		Material* material = nullptr;	// Points to scene manager's statically allocated list of materials
	};
}



