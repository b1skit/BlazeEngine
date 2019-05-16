#include "Mesh.h"

namespace BlazeEngine
{
	// Vertex functions:


	// Mesh functions:
	Mesh::Mesh(string name, Vertex* vertices, unsigned int numVerts, GLuint* indices, unsigned int numIndices, int materialIndex /*= -1*/)
	{
		this->meshName = name;

		this->vertices = vertices;
		this->numVerts = numVerts;

		this->indices = indices;
		this->numIndices = numIndices;

		this->materialIndex = materialIndex;

		//this->transform = nullptr; // Must be set via SetTransform();


		// Create and bind our Vertex Array Object:
		glGenVertexArrays(1, &meshVAO);
		glBindVertexArray(meshVAO);

		// Create and bind a vertex buffer:
		glGenBuffers(1, &meshVBOs[BUFFER_VERTICES]);
		glBindBuffer(GL_ARRAY_BUFFER, meshVBOs[BUFFER_VERTICES]);

		// Create and bind an index buffer:
		glGenBuffers(1, &meshVBOs[BUFFER_INDEXES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshVBOs[BUFFER_INDEXES]);


		// Position:
		glEnableVertexAttribArray(VERTEX_POSITION);
		glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); // Define array of vertex attribute data: index, number of components (3 = 3 elements in vec3), type, should data be normalized?, stride, offset from start to 1st component

		// Color buffer:
		glEnableVertexAttribArray(VERTEX_COLOR);
		glVertexAttribPointer(VERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		// Normals:
		glEnableVertexAttribArray(VERTEX_NORMAL);
		glVertexAttribPointer(VERTEX_NORMAL, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Tangents:
		glEnableVertexAttribArray(VERTEX_TANGENT);
		glVertexAttribPointer(VERTEX_TANGENT, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

		// Bitangents:
		glEnableVertexAttribArray(VERTEX_BITANGENT);
		glVertexAttribPointer(VERTEX_BITANGENT, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

		// UV's:
		glEnableVertexAttribArray(VERTEX_UV0);
		glVertexAttribPointer(VERTEX_UV0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv0));

		glEnableVertexAttribArray(VERTEX_UV1);
		glVertexAttribPointer(VERTEX_UV1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv1));
		
		glEnableVertexAttribArray(VERTEX_UV2);
		glVertexAttribPointer(VERTEX_UV2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv2));
		
		glEnableVertexAttribArray(VERTEX_UV3);
		glVertexAttribPointer(VERTEX_UV3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv3));


		// Buffer data:
		glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(Vertex), &vertices[0].position.x, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);


		// Cleanup:
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//Mesh::~Mesh()
	//{
	//	// Cleanup should be handled by whatever owns the mesh, by calling DestroyMesh()
	//}

	void Mesh::DestroyMesh()
	{
		#if defined(DEBUG_LOG_OUTPUT)
			name = name + "_DELETED"; // Safety...
		#endif

		if (vertices)
		{
			delete[] vertices;
			vertices = nullptr;
			numVerts = -1;
		}
		if (indices)
		{
			delete[] indices;
			indices = nullptr;
			numIndices = -1;
		}

		glDeleteVertexArrays(1, &meshVAO);
		glDeleteBuffers(BUFFER_COUNT, meshVBOs);

		materialIndex = -1;		// Note: Material MUST be cleaned up elsewhere!
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

		vec4 uvs[4]
		{
			vec4(0.0f, 0.0f, 0.0f, 0.0f),
			vec4(0.0f, 1.0f, 0.0f, 0.0f),
			vec4(1.0f, 0.0f, 0.0f, 0.0f),
			vec4(1.0f, 1.0f, 0.0f, 0.0f),
		};

		int numVerts = 24;
		Vertex* cubeVerts = new Vertex[numVerts]
		{
			// TO DO: Implement hard-coded tangent & bitangents instead of empty vec3's...

			// Front face:
			Vertex(positions[0], normals[0], vec3(), vec3(), colors[0], uvs[1]), // HINT: position index should = color index
			Vertex(positions[1], normals[0], vec3(), vec3(), colors[1], uvs[0]), // All UV's should be used once per face
			Vertex(positions[2], normals[0], vec3(), vec3(), colors[2], uvs[2]), //2
			Vertex(positions[3], normals[0], vec3(), vec3(), colors[3], uvs[3]), //3

			// Left face:
			Vertex(positions[4], normals[2], vec3(), vec3(), colors[4], uvs[1]), //4
			Vertex(positions[5], normals[2], vec3(), vec3(), colors[5], uvs[0]),
			Vertex(positions[1], normals[2], vec3(), vec3(), colors[1], uvs[2]),
			Vertex(positions[0], normals[2], vec3(), vec3(), colors[0], uvs[3]), //7

			// Right face:
			Vertex(positions[3], normals[3], vec3(), vec3(), colors[3], uvs[1]), //8
			Vertex(positions[2], normals[3], vec3(), vec3(), colors[2], uvs[0]),
			Vertex(positions[6], normals[3], vec3(), vec3(), colors[6], uvs[2]),
			Vertex(positions[7], normals[3], vec3(), vec3(), colors[7], uvs[3]), //11

			// Top face:
			Vertex(positions[4], normals[4], vec3(), vec3(), colors[4], uvs[1]), //12
			Vertex(positions[0], normals[4], vec3(), vec3(), colors[0], uvs[0]),
			Vertex(positions[3], normals[4], vec3(), vec3(), colors[3], uvs[2]),
			Vertex(positions[7], normals[4], vec3(), vec3(), colors[7], uvs[3]), //15

			// Bottom face:
			Vertex(positions[1], normals[5], vec3(), vec3(), colors[1], uvs[1]), //16
			Vertex(positions[5], normals[5], vec3(), vec3(), colors[5], uvs[0]),
			Vertex(positions[6], normals[5], vec3(), vec3(), colors[6], uvs[2]),
			Vertex(positions[2], normals[5], vec3(), vec3(), colors[2], uvs[3]), //19

			// Back face:
			Vertex(positions[7], normals[1], vec3(), vec3(), colors[7], uvs[1]), //20
			Vertex(positions[6], normals[1], vec3(), vec3(), colors[6], uvs[0]),
			Vertex(positions[5], normals[1], vec3(), vec3(), colors[5], uvs[2]),
			Vertex(positions[4], normals[1], vec3(), vec3(), colors[4], uvs[3]), //23
		};

		int numIndices = 36;
		GLuint* cubeIndices = new GLuint[numIndices] // 6 faces * 2 tris * 3 indices 
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

		return Mesh("cube", cubeVerts, numVerts, cubeIndices, numIndices);
	}

}


