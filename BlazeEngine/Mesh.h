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
			
			uv0 = uv1 = uv2 = uv3 = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		 //Explicit constructor:
		Vertex(const vec3& position, const vec3& normal, const vec3 tangent, const vec3 bitangent, const vec4& color, const vec4& uv0)
		{
			this->position = position;
			this->color = color;

			this->normal = normal;
			this->tangent = tangent;
			this->bitangent = bitangent;
			
			this->uv0 = uv0;

			// Just set these to 0 for now...
			uv1 = uv2 = uv3 = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		// Copy constructor:
		Vertex(const Vertex& vertex) = default;


		vec3 position;
		vec4 color;
		
		vec3 normal;
		vec3 tangent;
		vec3 bitangent;
		
		vec4 uv0;
		vec4 uv1;
		vec4 uv2;
		vec4 uv3;

	protected:


	private:


	};


	// Bounds of a mesh, scene, etc
	struct Bounds
	{
		float xMin = std::numeric_limits<float>::max();
		float xMax = std::numeric_limits<float>::min();

		float yMin = std::numeric_limits<float>::max();
		float yMax = std::numeric_limits<float>::min();

		float zMin = std::numeric_limits<float>::max();
		float zMax = std::numeric_limits<float>::min();
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
		VERTEX_UV1			= 6,
		VERTEX_UV2			= 7,
		VERTEX_UV3			= 8,

		VERTEX_ATTRIBUTES_COUNT	// RESERVED: The total number of vertex attributes
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

		// Mesh bounds, in local space
		Bounds bounds;

	protected:


	private:
		Vertex* vertices		= nullptr;		// Deallocated in SceneManager.Shutdown()
		unsigned int numVerts	= 0;

		GLuint* indices			= nullptr;		// Deallocated in SceneManager.Shutdown()
		unsigned int numIndices = 0;

		GLuint meshVAO			= 0;
		GLuint meshVBOs[BUFFER_COUNT];			// Buffer objects that hold vertices in GPU memory

		int materialIndex		= -1;

		Transform transform;
		string meshName			= "UNNAMED_MESH";

		// Computes mesh bounds, in local space
		void ComputeBounds();
	};
}



