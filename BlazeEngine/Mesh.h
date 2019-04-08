#pragma once

#include <glm.hpp>
#include <GL/glew.h>

//using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::vec2;


namespace BlazeEngine
{
	// Pre-declarations:
	class Transform;


	// Vertex structure:
	struct Vertex
	{
	public:
		// TO DO: Figure out an elegant way to initialize vertices with all of their attributes...
		Vertex()
		{
			position = vec3(0.0f, 0.0f, 0.0f); 
			normal = vec3(0.0f, 0.0f, 0.0f);
			color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
			uv = vec2(0.0f, 0.0f);
		}

		 //Explicit constructor:
		Vertex(const vec3& position, const vec3& normal, const vec4& color, const vec2& uv)
		{
			this->position = position;
			this->normal = normal;
			this->color = color;
			this->uv = uv;
		}

		// Copy constructor:
		Vertex(const Vertex& vertex) = default;


		vec3 position;
		vec3 normal;
		vec4 color;
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
		VERTEX_POSITION,
		VERTEX_NORMAL,
		VERTEX_COLOR,
		VERTEX_UV, // TO DO: Implement multipl UV channels?

		VERTEX_NUM_ATTRIBUTES	// RESERVED: The total number of vertex attributes
	};


	class Mesh
	{
	public:
		Mesh(Vertex* vertices, unsigned int numVerts, GLuint* indices, unsigned int numIndices, int materialIndex = -1);
		/*~Mesh();*/

		// Copy constructor:
		Mesh(const Mesh& mesh) = default;

		// Getters/Setters:
		inline Vertex* Vertices() { return vertices; }
		inline unsigned int NumVerts() { return this->numVerts; }
		
		inline int& MaterialIndex() { return materialIndex; }
		
		inline GLuint* Indices() { return indices; }
		inline unsigned int NumIndices() { return numIndices; }
		
		inline Transform* GetTransform() { return transform; }
		inline void SetTransform(Transform* transform) { this->transform = transform; }

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

		Transform* transform	= nullptr;		// Pointer to statically allocated Transform held by SceneObject superclass
	};
}



