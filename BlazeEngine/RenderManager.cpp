#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"

//#include <stddef.h>


#define GLM_FORCE_SWIZZLE
#include "glm.hpp"
#include "GL/glew.h"
#include <GL/GL.h> // MUST follow glew.h...
#include <SDL_egl.h>
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

//using std::ifstream;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;


// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	RenderManager::~RenderManager()
	{
		// Close our window in the destructor so we can still read any final OpenGL error messages before it is destroyed
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(glWindow);
		SDL_Quit();
	}

	RenderManager& RenderManager::Instance()
	{
		static RenderManager* instance = new RenderManager();
		return *instance;
	}

	void RenderManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("Render manager started!") });

		// Cache the relevant config data:
		this->xRes = coreEngine->GetConfig()->renderer.windowXRes;
		this->yRes = coreEngine->GetConfig()->renderer.windowYRes;
		this->windowTitle = coreEngine->GetConfig()->renderer.windowTitle;

		// Initialize SDL:
		/*SDL_Init(SDL_INIT_VIDEO);*/ // TO DO: IMPLEMENT PER-COMPONENT INITIALIZATION

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Create a window:
		glWindow = SDL_CreateWindow(
			coreEngine->GetConfig()->renderer.windowTitle.c_str(), 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			xRes, 
			yRes, 
			SDL_WINDOW_OPENGL
		);

		// Create an OpenGL context:
		glContext = SDL_GL_CreateContext(glWindow);
		
		// Configure SDL:
		SDL_SetRelativeMouseMode(SDL_TRUE);	// Lock the mouse to the window

		/*SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);*/

		// /* Enable multisampling for a nice antialiased effect */
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);*/


		//// Make our buffer swap syncronized with the monitor's vertical refresh:
		//SDL_GL_SetSwapInterval(1);

		// Configure OpenGL:
		glEnable(GL_DEPTH_TEST);			// Enable Z depth testing
		//glDepthFunc(GL_LESS);				// How to sort Z
		
		

		// Initialize glew:
		GLenum glStatus = glewInit();
		if (glStatus != GLEW_OK)
		{
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Render manager start failed: glStatus not ok!") });
			return;
		}

		// Generate and bind our Vertex Array Object as active:
		glGenVertexArrays(1, &vertexArrayObject); // Allocate the required number of vertex array objects (VAO's)
		glBindVertexArray(vertexArrayObject); // Bind our VAO

		// Create and bind a vertex buffer to a buffer binding point allocated on the GPU suitable for vertices:
		glGenBuffers(1, &vertexBufferObjects[BUFFER_VERTICES]); // Create a vertex position buffer object and store its handle
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[BUFFER_VERTICES]); // Bind our VBO to GL_ARRAY_BUFFER
		
		// Position:
		glEnableVertexAttribArray(0); // Indicate that the vertex attribute at index 0 is being used
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); // Define array of vertex attribute data: index, number of components (3 = 3 elements in vec3), type, should data be normalized?, stride, offset from start to 1st component
		
		// Normals:
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Color buffer:
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		
		// UV's:
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		



		// Bind our index buffer:
		glGenBuffers(1, &vertexBufferObjects[BUFFER_INDEXES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObjects[BUFFER_INDEXES]);

		// Cleanup:
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));
	}

	void RenderManager::Shutdown()
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("Render manager shutting down...") });

		glDeleteVertexArrays(BUFFER_COUNT, &vertexArrayObject);
		glDeleteBuffers(BUFFER_COUNT, vertexBufferObjects);

		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void RenderManager::Update()
	{
		
	}


	void RenderManager::Render(double alpha)
	{
		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TO DO: Loop by material, shader, mesh:
		// Pre-store all vertices for the scene in (material, shader) buffers?

		// TO DO: Merge ALL meshes using the same material into a single draw call


		vector<Shader>* shaders = coreEngine->BlazeSceneManager->GetShaders();


		// Assemble common (model independent) matrices:
		mat4 view = this->coreEngine->BlazeSceneManager->MainCamera()->View();
		mat4 projection = this->coreEngine->BlazeSceneManager->MainCamera()->Projection();
		

		unsigned int numMaterials = coreEngine->BlazeSceneManager->NumMaterials();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			Material* currentMaterial = coreEngine->BlazeSceneManager->GetMaterial(currentMaterialIndex);
			// TO DO: Material setup: texture uploads etc




			// Bind the required VAO:
			glBindVertexArray(vertexArrayObject);

			// Setup the current shader:
			unsigned int shaderIndex = coreEngine->BlazeSceneManager->GetShaderIndex(currentMaterialIndex);
			glUseProgram(shaders->at(shaderIndex).ShaderReference()); // ...TO DO: Decide whether to use this directly, or via BindShader() ?

			// Upload common shader matrices:
			GLuint matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "in_view");
			if (matrixID >= 0)
			{
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &view[0][0]);
			}
			matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "in_projection");
			if (matrixID >= 0)
			{
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &projection[0][0]);
			}

			// Upload ambient light data:
			matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "ambient");
			if (matrixID >= 0)
			{
				glUniform4fv(matrixID, 1, &coreEngine->BlazeSceneManager->GetAmbient()[0]);
			}

			// Upload key light forward direction in world space:
			matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "keyDirection");
			if (matrixID >= 0)
			{
				vec3 keyDirection = coreEngine->BlazeSceneManager->GetKeyLight().GetTransform().Forward() * -1.0f; // Reverse the direction: Points surface->light
				glUniform3fv(matrixID, 1, &keyDirection[0]);
			}
			matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "keyColor");
			if (matrixID >= 0)
			{
				glUniform4fv(matrixID, 1, &coreEngine->BlazeSceneManager->GetKeyLight().Color().r);
			}
			matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "keyIntensity");
			if (matrixID >= 0)
			{
				glUniform1f(matrixID, coreEngine->BlazeSceneManager->GetKeyLight().Intensity());
			}

			// Loop through each mesh:
			vector<Mesh*> const* materialMeshes = coreEngine->BlazeSceneManager->GetRenderMeshes(currentMaterialIndex);
			unsigned int numMeshes = (unsigned int)materialMeshes->size();
			for (unsigned int j = 0; j < numMeshes; j++)
			{
				Mesh* currentMesh = materialMeshes->at(j);

				// Assemble model-specific matrices:
				mat4 model = currentMesh->GetTransform()->Model();
				mat4 mv = view * model;
				mat4 mvp = coreEngine->BlazeSceneManager->MainCamera()->ViewProjection() * model;
				
				mat3 mv_it = glm::transpose(glm::inverse(mat3(mv)));
				
				// Bind our position VBO as active and copy vertex data into the buffer
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[BUFFER_VERTICES]);		// TO DO: Define when/which obects should use GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW ??
				glBufferData(GL_ARRAY_BUFFER, currentMesh->NumVerts() * sizeof(Vertex), &currentMesh->Vertices()[0].position.x, GL_STATIC_DRAW); // <- should we be null checking?

				// Bind our index VBO as active:
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObjects[BUFFER_INDEXES]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentMesh->NumIndices() * sizeof(GLubyte), &currentMesh->Indices()[0], GL_STATIC_DRAW);

				// Upload mesh-specific matrices:
				matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "in_model");
				if (matrixID >= 0)
				{
					glUniformMatrix4fv(matrixID, 1, GL_FALSE, &model[0][0]);
				}
				matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "in_mv");
				if (matrixID >= 0)
				{
					glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mv[0][0]);
				}
				matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "in_mvp");
				if (matrixID >= 0)
				{
					glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
				}				

				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_BYTE, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type,const GLvoid * indices);

				// Cleanup: 
				glBindBuffer(GL_ARRAY_BUFFER, 0); //Bind object 0 to GL_ARRAY_BUFFER to unbind vertexBufferObjects[BUFFER_VERTICES]
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}

		// Display the new frame:
		SDL_GL_SwapWindow(glWindow);
	}



	void RenderManager::ClearWindow(vec4 clearColor)
	{
		// Set the initial color in both buffers:
		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(glWindow);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}


