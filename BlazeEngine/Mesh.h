#pragma once

#include "Transform.h"

#include <glm.hpp>
#include <GL/glew.h>

#include <string>

//using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::vec2;

using std::string;


namespace BlazeEngine
{
	//// Pre-declarations:
	//class Transform;


	// Vertex structure:
	struct Vertex
	{
	public:
		Vertex()
		{
			position = normal = tangent = bitangent = vec3(0.0f, 0.0f, 0.0f);
			
			color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			uv = vec2(0.0f, 0.0f);
			// TO DO: Make this a vec4!
		}

		 //Explicit constructor:
		Vertex(const vec3& position, const vec3& normal, const vec3 tangent, const vec3 bitangent, const vec4& color, const vec2& uv)
		{
			this->position = position;
			this->normal = normal;
			this->tangent = tangent;
			this->bitangent = bitangent;
			this->color = color;
			this->uv = uv;
		}

		// Copy constructor:
		Vertex(const Vertex& vertex) = default;


		vec3 position;
		vec4 color;
		
		vec3 normal;
		vec3 tangent;
		vec3 bitangent;
		
		vec2 uv;

	protected:


	private:


	};


	enum VERTEX_BUFFER_OBJECT
	{
		BUFFER_VERTICES,
		BUFFER_INDEXES,

		BUFFER_COUNT, // Reserved: Number of buffers to allocate
	};


	enum VERTEX_ATTRIBUTE
	{
		VERTEX_POSITION		= 0,
		VERTEX_COLOR		= 1,
		
		VERTEX_NORMAL		= 2,
		VERTEX_TANGENT		= 3,
		VERTEX_BITANGENT	= 4,
		
		VERTEX_UV0			= 5, // TO DO: Implement multipl UV channels?
		//VERTEX_UV1			= 6,
		//VERTEX_UV2			= 7,
		//VERTEX_UV3			= 8,

		VERTEX_NUM_ATTRIBUTES	// RESERVED: The total number of vertex attributes
	};


	class Mesh
	{
	public:
		Mesh(string name, Vertex* vertices, unsigned int numVerts, GLuint* indices, unsigned int numIndices, int materialIndex = -1);
		/*~Mesh();*/

		// Copy constructor:
		Mesh(const Mesh& mesh) = default;

		// Getters/Setters:
		inline string const& Name() { return meshName; }

		inline Vertex* Vertices() { return vertices; }
		inline unsigned int NumVerts() { return this->numVerts; }
		
		inline int& MaterialIndex() { return materialIndex; }
		
		inline GLuint* Indices() { return indices; }
		inline unsigned int NumIndices() { return numIndices; }


		inline Transform& GetTransform() { return transform; }

		inline GLuint const& VAO() { return meshVAO; }
		inline GLuint const& VBO(VERTEX_BUFFER_OBJECT index) { return meshVBOs[index]; }
		
		// Deallocate and unbind this mesh object
		void DestroyMesh();


		// Static functions:
		//------------------

		// Create a simple cube mesh aligned to +/-1 on all axis'
		static Mesh CreateCube();


	protected:


	private:
		Vertex* vertices		= nullptr;		// Deallocated in SceneManager.Shutdown()
		unsigned int numVerts	= -1;

		GLuint* indices			= nullptr;		// Deallocated in SceneManager.Shutdown()
		unsigned int numIndices = -1;

		GLuint meshVAO			= 0;
		GLuint meshVBOs[BUFFER_COUNT];			// Buffer objects that hold vertices in GPU memory

		int materialIndex		= -1;

		Transform transform;
		string meshName			= "UNNAMED";
	};
}



