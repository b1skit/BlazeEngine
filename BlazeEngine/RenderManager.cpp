#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"


#include "SDL.h"
//#include <SDL_egl.h>
#include "GL/glew.h"
#include <GL/GL.h> // MUST follow glew.h...

#define GLM_FORCE_SWIZZLE
#include "glm.hpp"

#undef main // Required to prevent SDL from redefining main...

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

	void RenderManager::Startup()
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Render manager started!") });

		// TO DO: IMPLEMENT PER-COMPONENT INITIALIZATION

		//// Initialize SDL:
		//if (SDL_Init(SDL_INIT_VIDEO) < 0)
		//{
		//	CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Could not initialize SDL video") });
		//	return;
		//}

		// Cache the relevant config data:
		this->xRes = CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes;
		this->yRes = CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes;
		this->windowTitle = CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowTitle;

		// Configure SDL before creating a window:
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		if (SDL_GL_SetAttribute(
			SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE) < 0)
		{
			cout << "set attrib failed\n";
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Could not set context attribute") });
			return;
		}

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		/*SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);*/

		// /* Enable multisampling for a nice antialiased effect */
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);*/

		SDL_SetRelativeMouseMode(SDL_TRUE);	// Lock the mouse to the window

		//// Make our buffer swap syncronized with the monitor's vertical refresh:
		//SDL_GL_SetSwapInterval(1);

		// Create a window:
		glWindow = SDL_CreateWindow(
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowTitle.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			xRes, 
			yRes, 
			SDL_WINDOW_OPENGL
		);
		if (glWindow == NULL)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Could not create window") });
			return;
		}

		// Create an OpenGL context and make it current:
		glContext = SDL_GL_CreateContext(glWindow);
		if (glContext == NULL)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Could not create OpenGL context") });
			return;
		}
		if (SDL_GL_MakeCurrent(glWindow, glContext) < 0)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Failed to make OpenGL context current") });
			return;
		}

		// Initialize glew:
		glewExperimental = GL_TRUE; // Expose OpenGL 3.x+ interfaces
		GLenum glStatus = glewInit();
		if (glStatus != GLEW_OK)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Render manager start failed: glStatus not ok!") });
			return;
		}

		// Configure OpenGL:
		glEnable(GL_DEPTH_TEST);			// Enable Z depth testing
		glFrontFace(GL_CCW);				// Counter-clockwise vertex winding (OpenGL default)
		glEnable(GL_CULL_FACE);				// Enable face culling
		glCullFace(GL_BACK);				// Cull back faces
		//glDepthFunc(GL_LESS);				// How to sort Z


		// Create and bind our Vertex Array Object:
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);		

		// Create and bind a vertex buffer:
		glGenBuffers(1, &vertexBufferObjects[BUFFER_VERTICES]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[BUFFER_VERTICES]);
		
		// Position:
		glEnableVertexAttribArray(VERTEX_POSITION); // Indicate that the vertex attribute at index 0 is being used
		glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); // Define array of vertex attribute data: index, number of components (3 = 3 elements in vec3), type, should data be normalized?, stride, offset from start to 1st component
		
		// Normals:
		glEnableVertexAttribArray(VERTEX_NORMAL);
		glVertexAttribPointer(VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// Color buffer:
		glEnableVertexAttribArray(VERTEX_COLOR);
		glVertexAttribPointer(VERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		
		// UV's:
		glEnableVertexAttribArray(VERTEX_UV);
		glVertexAttribPointer(VERTEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

		
		// Create and bind texture samplers:
		glGenSamplers(1, &vertexBufferObjects[BUFFER_ALBEDO_SAMPLER]); // TO DO: Use a differnt array to contain sampler objects...
		glBindSampler(0, vertexBufferObjects[BUFFER_ALBEDO_SAMPLER]); // Assign to index/unit 0
		if (!glIsSampler(vertexBufferObjects[BUFFER_ALBEDO_SAMPLER]))
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, this, new string("Could not create sampler") });
		}
		// TO DO: Set sampler parameters? glSamplerParameter???


		// Bind our index buffer:
		glGenBuffers(1, &vertexBufferObjects[BUFFER_INDEXES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObjects[BUFFER_INDEXES]);


		// Cleanup:
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindSampler(0, 0);


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));
	}

	void RenderManager::Shutdown()
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Render manager shutting down...") });

		glDeleteVertexArrays(BUFFER_COUNT, &vertexArrayObject);
		glDeleteBuffers(BUFFER_COUNT, vertexBufferObjects);

		glDeleteSamplers(1, &vertexBufferObjects[BUFFER_ALBEDO_SAMPLER]);
	}

	void RenderManager::Update()
	{
	}


	void RenderManager::Render(double alpha)
	{
		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TO DO: Loop by material, shader, mesh:
		// Pre-store all vertices for the scene in (material, shader) buffers!

		// TO DO: Merge ALL meshes using the same material into a single draw call


		// Get the current list of shaders:
		Shader const* const* shaders = CoreEngine::GetSceneManager()->GetShaders();

		// Assemble common (model independent) matrices:
		mat4 view = CoreEngine::GetSceneManager()->MainCamera()->View();
		mat4 projection = CoreEngine::GetSceneManager()->MainCamera()->Projection();
		

		unsigned int numMaterials = CoreEngine::GetSceneManager()->NumMaterials();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Bind the required VAO:
			glBindVertexArray(vertexArrayObject);

			// Get the current material:
			Material* currentMaterial = CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			
			// Setup the current shader:
			unsigned int shaderIndex = currentMaterial->ShaderIndex();
			glUseProgram(shaders[shaderIndex]->ShaderReference()); // ...TO DO: Decide whether to use this directly, or via BindShader() ?


			// Bind textures:
			Texture* albedo = currentMaterial->GetTexture(TEXTURE_ALBEDO);
			glBindTexture(albedo->Target(), albedo->TextureID());


			// TO DO: Bind multiple textures (normal, etc)


			// Upload common shader matrices:
			GLuint matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "in_view");
			if (matrixID >= 0)
			{
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &view[0][0]);
			}
			matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "in_projection");
			if (matrixID >= 0)
			{
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &projection[0][0]);
			}

			// Upload ambient light data:
			matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "ambient");
			if (matrixID >= 0)
			{
				glUniform4fv(matrixID, 1, &CoreEngine::GetSceneManager()->GetAmbient()[0]);
			}

			// Upload key key light direction (world space):
			matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "keyDirection");
			if (matrixID >= 0)
			{
				vec3 keyDirection = CoreEngine::GetSceneManager()->GetKeyLight().GetTransform().Forward() * -1.0f; // Reverse the direction: Points surface->light
				glUniform3fv(matrixID, 1, &keyDirection[0]);
			}
			matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "keyColor");
			if (matrixID >= 0)
			{
				glUniform4fv(matrixID, 1, &CoreEngine::GetSceneManager()->GetKeyLight().Color().r);
			}
			matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "keyIntensity");
			if (matrixID >= 0)
			{
				glUniform1f(matrixID, CoreEngine::GetSceneManager()->GetKeyLight().Intensity());
			}

			// Loop through each mesh:
			vector<Mesh*> const* materialMeshes = CoreEngine::GetSceneManager()->GetRenderMeshes(currentMaterialIndex);
			unsigned int numMeshes = (unsigned int)materialMeshes->size();
			for (unsigned int j = 0; j < numMeshes; j++)
			{
				Mesh* currentMesh = materialMeshes->at(j);

				// Assemble model-specific matrices:
				mat4 model = currentMesh->GetTransform()->Model();
				mat4 mv = view * model;
				mat4 mvp = CoreEngine::GetSceneManager()->MainCamera()->ViewProjection() * model;
				
				mat3 mv_it = glm::transpose(glm::inverse(mat3(mv)));
				
				// Bind our position VBO as active and copy vertex data into the buffer
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[BUFFER_VERTICES]);		// TO DO: Define when/which obects should use GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW ??
				glBufferData(GL_ARRAY_BUFFER, currentMesh->NumVerts() * sizeof(Vertex), &currentMesh->Vertices()[0].position.x, GL_STATIC_DRAW); // <- should we be null checking?

				// Bind our index VBO as active:
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObjects[BUFFER_INDEXES]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, currentMesh->NumIndices() * sizeof(GLubyte), &currentMesh->Indices()[0], GL_STATIC_DRAW);

				// Upload mesh-specific matrices:
				matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "in_model");
				if (matrixID >= 0)
				{
					glUniformMatrix4fv(matrixID, 1, GL_FALSE, &model[0][0]);
				}
				matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "in_mv");
				if (matrixID >= 0)
				{
					glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mv[0][0]);
				}
				matrixID = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "in_mvp");
				if (matrixID >= 0)
				{
					glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
				}				

				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_BYTE, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type,const GLvoid * indices);

				// Cleanup: 
				glBindBuffer(GL_ARRAY_BUFFER, 0); //Bind object 0 to GL_ARRAY_BUFFER to unbind vertexBufferObjects[BUFFER_VERTICES]
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			// Cleanup:
			glBindTexture(GL_TEXTURE_2D, 0);
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


