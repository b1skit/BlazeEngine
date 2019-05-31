#pragma once

#include "Transform.h"

#include <glm.hpp>
#include <GL/glew.h>

#include <string>


using glm::vec2;
using glm::vec3;
using glm::vec4;


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
		Bounds() {};
		Bounds(Bounds const& rhs) = default;

		float xMin = std::numeric_limits<float>::max();
		float xMax = -std::numeric_limits<float>::max(); // Note: -max is the furthest away from max

		float yMin = std::numeric_limits<float>::max();
		float yMax = -std::numeric_limits<float>::max();

		float zMin = std::numeric_limits<float>::max();
		float zMax = -std::numeric_limits<float>::max();
		// TODO: Create a setter/getter for these values, ensuring they're 3D (ie. ?min != ?max)


		// Returns a Bounds, transformed from local space using transform
		Bounds GetTransformedBounds(mat4 const& transform)
		{
			// Temp: Ensure the bounds are 3D here, before we do any calculations (TODO: Getters/setters that enforce 3D bounds)
			Make3Dimensional();

			Bounds result;

			vec4 points[8];
			points[0]			= vec4(xMin, yMax, zMin, 1.0f);		// Left		top		front ("front" == forward == Z-)
			points[1]			= vec4(xMax, yMax, zMin, 1.0f);		// Right	top		front
			points[2]			= vec4(xMin, yMin, zMin, 1.0f);		// Left		bot		front
			points[3]			= vec4(xMax, yMin, zMin, 1.0f);		// Right	bot		front

			points[4]			= vec4(xMin, yMax, zMax, 1.0f);		// Left		top		back
			points[5]			= vec4(xMax, yMax, zMax, 1.0f);		// Right	top		back
			points[6]			= vec4(xMin, yMin, zMax, 1.0f);		// Left		bot		back
			points[7]			= vec4(xMax, yMin, zMax, 1.0f);		// Right	bot		back

			for (int i = 0; i < 8; i++)
			{
				points[i] = transform * points[i];

				if (points[i].x < result.xMin)
				{
					result.xMin = points[i].x;
				}
				if (points[i].x > result.xMax)
				{
					result.xMax = points[i].x;
				}
				
				if (points[i].y < result.yMin)
				{
					result.yMin = points[i].y;
				}
				if (points[i].y > result.yMax)
				{
					result.yMax = points[i].y;
				}
				
				if (points[i].z < result.zMin)
				{
					result.zMin = points[i].z;
				}
				if (points[i].z > result.zMax)
				{
					result.zMax = points[i].z;
				}
			}

			return result;
		}

		void Make3Dimensional()
		{
			if (glm::abs(xMax - xMin) < 0.01)
			{
				xMax += 0.01f;
				xMin -= 0.01f;
			}

			if (glm::abs(yMax - yMin) < 0.01)
			{
				yMax += 0.01f;
				yMin -= 0.01f;
			}

			if (glm::abs(zMax - zMin) < 0.01)
			{
				zMax += 0.01f;
				zMin -= 0.01f;
			}
		}
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
		inline string const&	Name()							{ return meshName; }

		inline Vertex*			Vertices()						{ return vertices; }
		inline unsigned int		NumVerts()						{ return this->numVerts; }
		
		inline int&				MaterialIndex()					{ return materialIndex; }
		
		inline GLuint*			Indices() { return indices; }
		inline unsigned int		NumIndices()					{ return numIndices; }


		inline Transform&		GetTransform()					{ return transform; }

		inline GLuint const&	VAO() { return meshVAO; }
		inline GLuint const&	VBO(VERTEX_BUFFER_OBJECT index)	{ return meshVBOs[index]; }
		
		// Deallocate and unbind this mesh object
		void DestroyMesh();


		// Static functions:
		//------------------

		// Create a simple cube mesh aligned to +/-1 on all axis'
		static Mesh CreateCube();

		// Mesh localBounds, in local space
		Bounds localBounds;

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

		// Computes mesh localBounds, in local space
		void ComputeBounds();
	};
}



