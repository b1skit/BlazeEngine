#include "Mesh.h"

#include "BuildConfiguration.h"

#include "glm.hpp"
#include "gtc/constants.hpp"
using glm::pi;


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

		// Once we've stored our properties locally, we can compute the localBounds:
		ComputeBounds();


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


	void Mesh::Destroy()
	{
		#if defined(DEBUG_LOG_OUTPUT)
			meshName = meshName + "_DELETED"; // Safety...
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
			// TODO: Implement hard-coded tangent & bitangents instead of empty vec3's...

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

	Mesh Mesh::CreateQuad(vec3 tl /*= vec3(-0.5f, 0.5f, 0.0f)*/, vec3 tr /*= vec3(0.5f, 0.5f, 0.0f)*/, vec3 bl /*= vec3(-0.5f, -0.5f, 0.0f)*/, vec3 br /*= vec3(0.5f, -0.5f, 0.0f)*/)
	{
		vec3 tangent	= normalize(vec3(br - bl));
		vec3 bitangent	= normalize(vec3(tl - bl));
		vec3 quadNormal = normalize(cross(tangent, bitangent));
		vec4 redColor	= vec4(1, 0, 0, 1); // Assign a bright red color by default...

		vec4 uvs[4]
		{
			vec4(0, 1, 0, 0), // tl
			vec4(0, 0, 0, 0), // bl
			vec4(1, 1, 0, 0), // tr
			vec4(1, 0, 0, 0)  // br
		};	

		int numVerts = 4;
		Vertex* quadVerts = new Vertex[numVerts]
		{
			// tl
			Vertex(tl, quadNormal, tangent, bitangent, redColor, uvs[0]),
			
			// bl
			Vertex(bl, quadNormal, tangent, bitangent, redColor, uvs[1]),

			// tr
			Vertex(tr, quadNormal, tangent, bitangent, redColor, uvs[2]),

			// br
			Vertex(br, quadNormal, tangent, bitangent, redColor, uvs[3])
		};

		int numIndices = 6;
		GLuint* quadIndices = new GLuint[numIndices]
		{
			// TL face:
			0, 1, 2,
			// BR face:
			2, 1, 3
		}; // Note: CCW winding

		return Mesh("quad", quadVerts, numVerts, quadIndices, numIndices);
	}


	Mesh Mesh::CreateSphere(float radius /*= 0.5f*/, int numLatSlices /*= 16*/, int numLongSlices /*= 16*/)
	{
		// NOTE: Currently, this function does not generate valid tangents for any verts. Some UV's are distorted, as we're using merged vertices. TODO: Fix this

		// Note: Latitude = horizontal lines about Y
		//		Longitude = vertical lines about sphere
		//		numLatSlices = horizontal segments
		//		numLongSlices = vertical segments

		int numVerts		= numLatSlices * numLongSlices + 2; // + 2 for end caps
		Vertex* vertices	= new Vertex[numVerts];
		vec3* normals		= new vec3[numVerts];
		vec4* uvs			= new vec4[numVerts];

		vec4 vertColor(1.0f, 1.0f, 1.0f, 1.0f);

		int numIndices		= 3 * numLatSlices * numLongSlices * 2;
		GLuint* indices		= new GLuint[numIndices];

		// Generate a sphere about the Y axis:
		vec3 firstPosition	= vec3(0.0f, radius, 0.0f);
		vec3 firstNormal	= vec3(0, 1.0f, 0);
		vec3 firstTangent	= vec3(0, 0, 0); //
		vec3 firstBitangent	= vec3(0, 0, 0); //
		vec4 firstUv0		= vec4(0.5f, 1.0f, 0, 0);

		int currentIndex = 0;
		vertices[currentIndex++] = Vertex(firstPosition, firstNormal, firstTangent, firstBitangent, vertColor, firstUv0);

		// Rotate about Z: Arc down the side profile of our sphere
		// cos theta = adj/hyp -> hyp * cos theta = adj -> radius * cos theta = Y
		float zRadianStep	= glm::pi<float>() / (float)(numLongSlices + 1); // +1 to get the number of rows
		float zRadians		= zRadianStep; // Already added cap vertex, so start on the next step
		
		// Rotate about Y: Horizontal edges
		// sin theta = opp/hyp -> hyp * sin theta = opp -> radius * sin theta = X
		// cos theta = adj/hyp -> hyp * cos theta = adj -> radius * cos theta = Z
		float yRadianStep	= (2.0f * glm::pi<float>()) / (float)numLatSlices; //
		float yRadians		= 0.0f;

		// Build UV's, from top left (0,1) to bottom right (1.0, 0)
		float uvXStep	= 1.0f / (float)numLatSlices;
		float uvYStep	= 1.0f / (float)(numLongSlices + 1);
		float uvX		= 0;
		float uvY		= 1.0f - uvYStep;

		// Outer loop: Rotate about Z, tracing the arc of the side silhouette down the Y axis
		for (int curLongSlices = 0; curLongSlices < numLongSlices; curLongSlices++)
		{
			float y = radius * cos(zRadians);
			
			// Inner loop: Rotate about Y
			for (int curLatSlices = 0; curLatSlices < numLatSlices; curLatSlices++)
			{
				float x = radius * sin(yRadians) * sin(zRadians);
				float z = radius * cos(yRadians) * sin(zRadians);
				yRadians += yRadianStep;

				vec3 position	= vec3(x, y, z);
				vec3 normal		= normalize(position);

				vec3 tangent	= vec3(1, 0, 0); // TODO
				vec3 bitangent	= vec3(0, 1, 0); // TODO
				vec4 uv0		= vec4(uvX, uvY, 0, 0);

				vertices[currentIndex++] = Vertex(position, normal, tangent, bitangent, vertColor, uv0);

				uvX += uvXStep;
			}
			yRadians = 0.0f;
			zRadians += zRadianStep;

			uvX = 0;
			uvY -= uvYStep;
		}

		// Final endcap:
		vec3 finalPosition	= vec3(0.0f, -radius, 0.0f);
		vec3 finalNormal	= vec3(0, -1, 0);

		vec3 finalTangent	= vec3(0, 0, 0);
		vec3 finalBitangent	= vec3(0, 0, 0);
		vec4 finalUv0		= vec4(0.5f, 0.0f, 0, 0);

		vertices[currentIndex] = Vertex(finalPosition, finalNormal, finalTangent, finalBitangent, vertColor, finalUv0);

		// Indices: (Note: We use counter-clockwise vertex winding)
		currentIndex = 0;

		// Top cap:
		for (int i = 1; i <= numLatSlices; i++)
		{
			indices[currentIndex++] = (GLuint)0;
			indices[currentIndex++] = (GLuint)i;
			indices[currentIndex++] = (GLuint)(i + 1);
		}
		indices[currentIndex - 1] = 1; // Wrap the last edge back to the start

		// Mid section:
		int topLeft = 1;
		int topRight = topLeft + 1;
		int botLeft = 1 + numLatSlices;
		int botRight = botLeft + 1;
		for (unsigned int i = 0; i < (unsigned int)(numLongSlices - 1); i++)
		{
			for (int j = 0; j < numLatSlices - 1; j++)
			{
				// Top left triangle:
				indices[currentIndex++] = (GLuint)topLeft;
				indices[currentIndex++] = (GLuint)botLeft;
				indices[currentIndex++] = (GLuint)topRight;

				// Bot right triangle
				indices[currentIndex++] = (GLuint)topRight;
				indices[currentIndex++] = (GLuint)botLeft;
				indices[currentIndex++] = (GLuint)botRight;

				topLeft++;
				topRight++;
				botLeft++;
				botRight++;
			}
			// Wrap the edge around:
			// Top left triangle:
			indices[currentIndex++] = (GLuint)topLeft;
			indices[currentIndex++] = (GLuint)botLeft;
			indices[currentIndex++] = (GLuint)(topRight - numLatSlices);

			// Bot right triangle
			indices[currentIndex++] = (GLuint)(topRight - numLatSlices);
			indices[currentIndex++] = (GLuint)botLeft;
			indices[currentIndex++] = (GLuint)(botRight - numLatSlices);

			// Advance to the next row:
			topLeft++;
			botLeft++;
			topRight++;
			botRight++;
		}

		// Bottom cap:
		for (int i = numVerts - numLatSlices - 1; i < numVerts - 1; i++)
		{
			indices[currentIndex++] = (GLuint)i;
			indices[currentIndex++] = (GLuint)(numVerts - 1);
			indices[currentIndex++] = (GLuint)(i + 1);
		}
		indices[currentIndex - 1] = numVerts - numLatSlices - 1; // Wrap the last edge back to the start		

		return Mesh("sphere", vertices, numVerts, indices, numIndices);
	}

	void Mesh::ComputeBounds()
	{
		for (unsigned int i = 0; i < numVerts; i++)
		{
			if (vertices[i].position.x < localBounds.xMin)
			{
				localBounds.xMin = vertices[i].position.x;
			}
			if (vertices[i].position.x > localBounds.xMax)
			{
				localBounds.xMax = vertices[i].position.x;
			}

			if (vertices[i].position.y < localBounds.yMin)
			{
				localBounds.yMin = vertices[i].position.y;
			}
			if (vertices[i].position.y > localBounds.yMax)
			{
				localBounds.yMax = vertices[i].position.y;
			}

			if (vertices[i].position.z < localBounds.zMin)
			{
				localBounds.zMin = vertices[i].position.z;
			}
			if (vertices[i].position.z > localBounds.zMax)
			{
				localBounds.zMax = vertices[i].position.z;
			}
		}
	}
}


