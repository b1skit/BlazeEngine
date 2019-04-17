#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "BuildConfiguration.h"

#include <string>

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



namespace BlazeEngine
{
	// OpenGL error message helper function: (Enable/disable via BuildConfiguration.h)
	#if defined(DEBUG_LOG_OPENGL)
		void GLAPIENTRY GLMessageCallback
		(
				GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam
		)
		{
			string output = "\nOpenGL Error Callback:\nSource: ";

			switch (source)
			{
			case GL_DEBUG_SOURCE_APPLICATION: 
				output += "GL_DEBUG_SOURCE_APPLICATION\n";
					break;

			case GL_DEBUG_SOURCE_THIRD_PARTY:
				output += "GL_DEBUG_SOURCE_THIRD_PARTY\n";
					break;
			default:
				output += "\n";
			}

			output += "Type: ";

			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:
				output += "ERROR\n";
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				output += "DEPRECATED_BEHAVIOR\n";
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				output += "UNDEFINED_BEHAVIOR\n";
				break;
			case GL_DEBUG_TYPE_PORTABILITY:
				output += "PORTABILITY\n";
				break;
			case GL_DEBUG_TYPE_PERFORMANCE:
				output += "PERFORMANCE\n";
				break;
			case GL_DEBUG_TYPE_OTHER:
				output += "OTHER\n";
				break;
			default:
				output += "\n";
			}

			output += "id: " + to_string(id) + "\n";

			output += "Severity: ";
			switch (severity)
			{
			#if defined(LOG_VERBOSITY_ALL)
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				output += "NOTIFICATION\n";
				break;
			#else
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				return; // DO NOTHING
			#endif
			case GL_DEBUG_SEVERITY_LOW :
					output += "LOW\n";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				output += "MEDIUM\n";
				break;
			case GL_DEBUG_SEVERITY_HIGH:
				output += "HIGH\n";
				break;
			default:
				output += "\n";
			}
		
			output += "Message: " + string(message);

			switch(severity)
			{
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				LOG(output);
				break;
			default:
				LOG_ERROR(output);
			}
		}
	#endif


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
		LOG("Render manager started!");

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

		// Configure OpenGL logging:
		#if defined(DEBUG_LOG_OPENGL)		// Defined in BuildConfiguration.h
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(GLMessageCallback, 0);
		#endif

		// Configure other OpenGL settings:
		glEnable(GL_DEPTH_TEST);			// Enable Z depth testing
		glFrontFace(GL_CCW);				// Counter-clockwise vertex winding (OpenGL default)
		glEnable(GL_CULL_FACE);				// Enable face culling
		glCullFace(GL_BACK);				// Cull back faces
		//glDepthFunc(GL_LESS);				// How to sort Z


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));
	}

	void RenderManager::Shutdown()
	{
		LOG("Render manager shutting down...");
	}

	void RenderManager::Update()
	{
	}


	void RenderManager::Render(double alpha)
	{
		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// TO DO: Merge ALL meshes using the same material into a single draw call


		// Assemble common (model independent) matrices:
		mat4 view = CoreEngine::GetSceneManager()->MainCamera()->View();
		mat4 projection = CoreEngine::GetSceneManager()->MainCamera()->Projection();
		
		// Loop by material, shader, mesh:
		unsigned int numMaterials = CoreEngine::GetSceneManager()->NumMaterials();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Get the current material:
			Material* currentMaterial = CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			
			// Set the current shader:
			GLuint shaderReference = currentMaterial->GetShader()->ShaderReference();
			glUseProgram(shaderReference); // ...TO DO: Decide whether to use this directly, or via BindShader() ?

			// Bind the samplers:
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				glBindSampler(i, currentMaterial->Samplers(i));
			}			

			// Bind textures:
			Texture* albedo = currentMaterial->GetTexture(TEXTURE_ALBEDO);
			if (albedo)
			{
				GLuint samplerLocation = glGetUniformLocation(shaderReference, "albedo");
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
				GLuint samplerLocation = glGetUniformLocation(shaderReference, "normal");
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
				GLuint samplerLocation = glGetUniformLocation(shaderReference, "roughness");
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
				GLuint samplerLocation = glGetUniformLocation(shaderReference, "metallic");
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
				GLuint samplerLocation = glGetUniformLocation(shaderReference, "ambientOcclusion");
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, TEXTURE_AMBIENT_OCCLUSION);
				}
				glActiveTexture(GL_TEXTURE0 + TEXTURE_AMBIENT_OCCLUSION);
				glBindTexture(ambient->Target(), ambient->TextureID());
			}
						

			// Upload common shader matrices:
			GLuint viewID = glGetUniformLocation(shaderReference, "in_view");
			if (viewID >= 0)
			{
				glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
			}
			GLuint projectionID = glGetUniformLocation(shaderReference, "in_projection");
			if (projectionID >= 0)
			{
				glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projection[0][0]);
			}

			// Upload ambient light data:
			GLuint ambientID = glGetUniformLocation(shaderReference, "ambient");
			if (ambientID >= 0)
			{
				glUniform4fv(ambientID, 1, &CoreEngine::GetSceneManager()->GetAmbient()[0]);
			}

			// Upload key key light direction (world space):
			GLuint keyDirID = glGetUniformLocation(shaderReference, "keyDirection");
			if (keyDirID >= 0)
			{
				vec3 keyDirection = CoreEngine::GetSceneManager()->GetKeyLight().GetTransform().Forward() * -1.0f; // Reverse the direction: Points surface->light
				glUniform3fv(keyDirID, 1, &keyDirection[0]);
			}
			GLuint keyColorID = glGetUniformLocation(shaderReference, "keyColor");
			if (keyColorID >= 0)
			{
				glUniform4fv(keyColorID, 1, &CoreEngine::GetSceneManager()->GetKeyLight().Color().r);
			}
			GLuint keyIntensityID = glGetUniformLocation(shaderReference, "keyIntensity");
			if (keyIntensityID >= 0)
			{
				glUniform1f(keyIntensityID, CoreEngine::GetSceneManager()->GetKeyLight().Intensity());
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
				mat4 model	= currentMesh->GetTransform().Model();
				mat4 mv		= view * model;
				mat4 mvp	= CoreEngine::GetSceneManager()->MainCamera()->ViewProjection() * model;
				
				mat3 mv_it = glm::transpose(glm::inverse(mat3(mv)));

				// Upload mesh-specific matrices:
				GLuint modelID = glGetUniformLocation(shaderReference, "in_model");
				if (modelID >= 0)
				{
					glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
				}
				GLuint mvID = glGetUniformLocation(shaderReference, "in_mv");
				if (mvID >= 0)
				{
					glUniformMatrix4fv(mvID, 1, GL_FALSE, &mv[0][0]);
				}
				GLuint mvpID = glGetUniformLocation(shaderReference, "in_mvp");
				if (mvpID >= 0)
				{
					glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
				}				

				// Bind our position and index buffers:
				glBindBuffer(GL_ARRAY_BUFFER, currentMesh->VBO(BUFFER_VERTICES));
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentMesh->VBO(BUFFER_INDEXES));

				// Draw!
				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type,const GLvoid * indices);

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


