#include "Mesh.h"

namespace BlazeEngine
{
	// Vertex functions:


	// Mesh functions:
	Mesh::Mesh(Vertex* vertices, unsigned int numVerts)
	{
		drawCount = numVerts;

		glGenVertexArrays(1, &vertexArrayObject); // Size, target
		glBindVertexArray(vertexArrayObject);

		glGenBuffers(VERTEX_BUFFER_SIZE, vertexArrayBuffers); // Allocate buffer on the GPU
		glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[VERTEX_BUFFER_POSITION]); // Tell OpenGl to interpret buffer as an array
		glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(vertices[0]), vertices, GL_STATIC_DRAW); // Put data into the buffer

		// Tell OpenGL how to interpet the data we've put on the GPU:
		glEnableVertexAttribArray(0); // Treat data as an array
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Tell it how to read the array: attribute array, count (3 = 3 elements in vec3), data type, normalize?, space between steps(?), start offset

		glBindVertexArray(0); // Prevent further vertex array object operations affecting our vertex array object
	}


	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
	}
}


