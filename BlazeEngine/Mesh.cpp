#include "Mesh.h"

namespace BlazeEngine
{
	// Vertex functions:


	// Mesh functions:
	Mesh::Mesh(Vertex* vertices, unsigned int numVerts, GLubyte* indices, unsigned int numIndices, int materialIndex)
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


	// Static functions:
	//-------------------

	
	Mesh Mesh::CreateCube()
	{
		// Note: BlazeEngine uses a RHCS in all cases
		vec3 positions[8];
		positions[0] = vec3(-1.0f, 1.0f, 1.0f); // "Front" side
		positions[1] = vec3(-1.0f, -1.0f, 1.0f);
		positions[2] = vec3(1.0f, -1.0f, 1.0f);
		positions[3] = vec3(1.0f, 1.0f, 1.0f);
		
		positions[4] = vec3(-1.0f, 1.0f, -1.0f); // "Back" side
		positions[5] = vec3(-1.0f, -1.0f, -1.0f);
		positions[6] = vec3(1.0f, -1.0f, -1.0f);
		positions[7] = vec3(1.0f, 1.0f, -1.0f);

		vec3 normals[6]
		{
			vec3(0.0f, 0.0f, 1.0f),		// Front = 0
			vec3(0.0f, 0.0f, -1.0f),	// Back = 1
			vec3(-1.0f, 0.0f, 0.0f),	// Left = 2
			vec3(1.0f, 0.0f, 0.0f),		// Right = 3
			vec3(0.0f, 1.0f, 0.0f),		// Up = 4
			vec3(0.0f, -1.0f, 0.0f),	// Down = 5
		};

		vec4 colors[8]
		{
			vec4(0.0f, 0.0f, 0.0f, 1.0f),
			vec4(0.0f, 0.0f, 1.0f, 1.0f),
			vec4(0.0f, 1.0f, 0.0f, 1.0f),
			vec4(0.0f, 1.0f, 1.0f, 1.0f),
			vec4(1.0f, 0.0f, 0.0f, 1.0f),
			vec4(1.0f, 0.0f, 1.0f, 1.0f),
			vec4(1.0f, 1.0f, 0.0f, 1.0f),
			vec4(1.0f, 1.0f, 1.0f, 1.0f),
		};

		vec2 uvs[4]
		{
			vec2(0.0f, 0.0f),
			vec2(0.0f, 1.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
		};

		int numVerts = 24;
		Vertex* cubeVerts = new Vertex[numVerts]
		{
			// Front face:
			Vertex(positions[0], normals[0], colors[0], uvs[1]), // HINT: position index should = color index
			Vertex(positions[1], normals[0], colors[1], uvs[0]), // All UV's should be used once per face
			Vertex(positions[2], normals[0], colors[2], uvs[2]), //2
			Vertex(positions[3], normals[0], colors[3], uvs[3]), //3

			// Left face:
			Vertex(positions[4], normals[2], colors[4], uvs[1]), //4
			Vertex(positions[5], normals[2], colors[5], uvs[0]),
			Vertex(positions[1], normals[2], colors[1], uvs[2]),
			Vertex(positions[0], normals[2], colors[0], uvs[3]), //7

			// Right face:
			Vertex(positions[3], normals[3], colors[3], uvs[1]), //8
			Vertex(positions[2], normals[3], colors[2], uvs[0]),
			Vertex(positions[6], normals[3], colors[6], uvs[2]),
			Vertex(positions[7], normals[3], colors[7], uvs[3]), //11

			// Top face:
			Vertex(positions[4], normals[4], colors[4], uvs[1]), //12
			Vertex(positions[0], normals[4], colors[0], uvs[0]),
			Vertex(positions[3], normals[4], colors[3], uvs[2]),
			Vertex(positions[7], normals[4], colors[7], uvs[3]), //15

			// Bottom face:
			Vertex(positions[1], normals[5], colors[1], uvs[1]), //16
			Vertex(positions[5], normals[5], colors[5], uvs[0]),
			Vertex(positions[6], normals[5], colors[6], uvs[2]),
			Vertex(positions[2], normals[5], colors[2], uvs[3]), //19

			// Back face:
			Vertex(positions[7], normals[1], colors[7], uvs[1]), //20
			Vertex(positions[6], normals[1], colors[6], uvs[0]),
			Vertex(positions[5], normals[1], colors[5], uvs[2]),
			Vertex(positions[4], normals[1], colors[4], uvs[3]), //23
		};

		int numIndices = 36;
		GLubyte* cubeIndices = new GLubyte[numIndices] // 6 faces * 2 tris * 3 indices 
		{
			// Front face:
			0, 1, 3,
			1, 2, 3,

			// Left face:
			4, 5, 7,
			7, 5, 6,

			// Right face:
			8, 9, 11,
			9, 10, 11,

			// Top face:
			12, 13, 15,
			13, 14, 15,

			// Bottom face:
			16, 17, 19,
			17, 18, 19,

			// Back face:
			20, 21, 23,
			21, 22, 23,
		};

		return Mesh(cubeVerts, numVerts, cubeIndices, numIndices);
	}

}


