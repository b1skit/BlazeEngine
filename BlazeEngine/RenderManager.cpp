#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"


#include "SDL.h"
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


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));
	}

	void RenderManager::Shutdown()
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Render manager shutting down...") });

	}

	void RenderManager::Update()
	{
	}


	void RenderManager::Render(double alpha)
	{
		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// TO DO: Merge ALL meshes using the same material into a single draw call


		// Get the current list of all shaders:
		Shader const* const* shaders = CoreEngine::GetSceneManager()->GetShaders();

		// Assemble common (model independent) matrices:
		mat4 view = CoreEngine::GetSceneManager()->MainCamera()->View();
		mat4 projection = CoreEngine::GetSceneManager()->MainCamera()->Projection();
		
		// Loop by material, shader, mesh:
		unsigned int numMaterials = CoreEngine::GetSceneManager()->NumMaterials();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Get the current material:
			Material* currentMaterial = CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			
			// Setup the current shader:
			unsigned int shaderIndex = currentMaterial->ShaderIndex();
			glUseProgram(shaders[shaderIndex]->ShaderReference()); // ...TO DO: Decide whether to use this directly, or via BindShader() ?

			// Bind the samplers:
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				glBindSampler(i, currentMaterial->Samplers(i));
			}			

			// Bind textures:
			Texture* albedo = currentMaterial->GetTexture(TEXTURE_ALBEDO);
			if (albedo)
			{
				GLuint samplerLocation = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "albedo");
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, TEXTURE_ALBEDO);
				}
				glActiveTexture(GL_TEXTURE0 + TEXTURE_ALBEDO);
				glBindTexture(albedo->Target(), albedo->TextureID());
			}
			Texture* normal = currentMaterial->GetTexture(TEXTURE_NORMAL);
			if (normal)
			{
				GLuint samplerLocation = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "normal");
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, TEXTURE_NORMAL);
				}
				glActiveTexture(GL_TEXTURE0 + TEXTURE_NORMAL);
				glBindTexture(normal->Target(), normal->TextureID());
			}
			Texture* roughness = currentMaterial->GetTexture(TEXTURE_ROUGHNESS);
			if (roughness)
			{
				GLuint samplerLocation = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "roughness");
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, TEXTURE_ROUGHNESS);
				}
				glActiveTexture(GL_TEXTURE0 + TEXTURE_ROUGHNESS);
				glBindTexture(roughness->Target(), roughness->TextureID());
			}
			Texture* metallic = currentMaterial->GetTexture(TEXTURE_METALLIC);
			if (metallic)
			{
				GLuint samplerLocation = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "metallic");
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, TEXTURE_METALLIC);
				}
				glActiveTexture(GL_TEXTURE0 + TEXTURE_METALLIC);
				glBindTexture(metallic->Target(), metallic->TextureID());
			}
			Texture* ambient = currentMaterial->GetTexture(TEXTURE_AMBIENT_OCCLUSION);
			if (ambient)
			{
				GLuint samplerLocation = glGetUniformLocation(shaders[shaderIndex]->ShaderReference(), "ambientOcclusion");
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, TEXTURE_AMBIENT_OCCLUSION);
				}
				glActiveTexture(GL_TEXTURE0 + TEXTURE_AMBIENT_OCCLUSION);
				glBindTexture(ambient->Target(), ambient->TextureID());
			}
						

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

				// Bind the mesh VAO:
				glBindVertexArray(currentMesh->VAO());


				// Assemble model-specific matrices:
				mat4 model = currentMesh->GetTransform()->Model();
				mat4 mv = view * model;
				mat4 mvp = CoreEngine::GetSceneManager()->MainCamera()->ViewProjection() * model;
				
				mat3 mv_it = glm::transpose(glm::inverse(mat3(mv)));

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

				// Bind our position and index buffers:
				glBindBuffer(GL_ARRAY_BUFFER, currentMesh->VBO(BUFFER_VERTICES));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentMesh->VBO(BUFFER_INDEXES));

				// Draw!
				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_BYTE, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type,const GLvoid * indices);

				// Cleanup: 
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			// Cleanup:
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				if (currentMaterial->GetTexture((TEXTURE_TYPE)i) != nullptr)
				{
					glBindTexture(currentMaterial->GetTexture((TEXTURE_TYPE)i)->Target(), 0);
				}		
				glBindSampler(i, 0); // Assign to index/unit 0
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


