#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
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
			#if defined(DEBUG_LOG_OPENGL_NOTIFICATIONS)
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

		// TODO: IMPLEMENT PER-COMPONENT INITIALIZATION

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

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0)
		{
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

		SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
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
		glewExperimental	= GL_TRUE; // Expose OpenGL 3.x+ interfaces
		GLenum glStatus		= glewInit();
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
		//// Render shadows:
		//vector<Camera*> cameras = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_SHADOW);
		//for (int currentCam = 0; currentCam < (int)cameras.size(); currentCam++)
		//{
		//	Render(cameras.at(currentCam));
		//}

		//// Render reflections:
		//cameras = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_SHADOW);
		//for (int currentCam = 0; currentCam < (int)cameras.size(); currentCam++)
		//{
		//	Render(cameras.at(currentCam));
		//}

		//// Render main camera:
		//cameras = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_MAIN);
		//Render(cameras.at(0));
		//
		//// // ^^ISSUE: Flickering - Rendering multiple cams to the screen!!!
		//
		//// TO DO: Move various openGL config settings from startup to here?


		// TEMP DEBUG:
		//vector<Camera*> cameras = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_SHADOW);
		vector<Camera*> cameras = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_MAIN);
		
		Render(cameras.at(0));

		//cameras[0]->DebugPrint();

		// Will need seperate render functions that configure the draw modes etc....
	}


	void RenderManager::Render(Camera* renderCam)
	{
		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// TODO: Merge ALL meshes using the same material into a single draw call


		// Assemble common (model independent) matrices:
		mat4 view = renderCam->View();
		
		// Loop by material (+shader), mesh:
		unsigned int numMaterials = CoreEngine::GetSceneManager()->NumMaterials();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Setup the current material and shader:
			Material* currentMaterial = CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			Shader* currentShader = currentMaterial->GetShader();
			GLuint shaderReference = currentShader->ShaderReference();

			BindShader(shaderReference);

			BindSamplers(currentMaterial);

			BindTextures(shaderReference, currentMaterial);

			// Upload common shader matrices:
			currentShader->UploadUniform("in_view", &view[0][0], UNIFORM_Matrix4fv);

			// Loop through each mesh:
			vector<Mesh*> const* materialMeshes = CoreEngine::GetSceneManager()->GetRenderMeshes(currentMaterialIndex);
			unsigned int numMeshes = (unsigned int)materialMeshes->size();
			for (unsigned int j = 0; j < numMeshes; j++)
			{
				Mesh* currentMesh = materialMeshes->at(j);

				BindMeshBuffers(currentMesh);

				// Assemble model-specific matrices:
				mat4 model			= currentMesh->GetTransform().Model();
				mat4 modelRotation	= currentMesh->GetTransform().Model(WORLD_ROTATION);
				mat4 mv				= view * model;
				mat4 mvp			= renderCam->ViewProjection() * model;

				// Upload mesh-specific matrices:
				currentShader->UploadUniform("in_model",			&model[0][0],			UNIFORM_Matrix4fv);
				currentShader->UploadUniform("in_modelRotation",	&modelRotation[0][0],	UNIFORM_Matrix4fv);
				currentShader->UploadUniform("in_mv",				&mv[0][0],				UNIFORM_Matrix4fv);
				currentShader->UploadUniform("in_mvp",				&mvp[0][0],				UNIFORM_Matrix4fv);

				// TODO: Only upload these matrices if they've changed ^^^^

				// Draw!
				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type,const GLvoid * indices);

				// Cleanup current mesh: 
				BindMeshBuffers();
			}

			// Cleanup current material and shader:
			BindSamplers(currentMaterial, true);
			BindShader(0);
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

	void BlazeEngine::RenderManager::BindShader(GLuint const& shaderReference)
	{
		glUseProgram(shaderReference);
	}

	void BlazeEngine::RenderManager::BindSamplers(Material* currentMaterial, bool doCleanup /*= false*/)
	{
		if (!doCleanup)
		{
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				glBindSampler(i, currentMaterial->Samplers(i));
			}
		}
		else
		{
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				glBindSampler(i, 0); // Assign to index/unit 0
			}
		}
	}

	void BlazeEngine::RenderManager::BindTextures(GLuint const& shaderReference, Material* currentMaterial)
	{
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
		Texture* RMAO = currentMaterial->GetTexture(TEXTURE_RMAO);
		if (RMAO)
		{
			GLuint samplerLocation = glGetUniformLocation(shaderReference, "RMAO");
			if (samplerLocation >= 0)
			{
				glUniform1i(samplerLocation, TEXTURE_RMAO);
			}
			glActiveTexture(GL_TEXTURE0 + TEXTURE_RMAO);
			glBindTexture(RMAO->Target(), RMAO->TextureID());
		}
	}


	void BlazeEngine::RenderManager::BindMeshBuffers(Mesh* const mesh /*= nullptr*/) // If mesh == nullptr, binds to element 0
	{
		if (mesh)
		{
			glBindVertexArray(mesh->VAO());
			glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO(BUFFER_VERTICES));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->VBO(BUFFER_INDEXES));
		}
		else
		{
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}

	void BlazeEngine::RenderManager::InitializeShaders()
	{
		SceneManager* sceneManager = CoreEngine::GetSceneManager();
		unsigned int numMaterials = sceneManager->NumMaterials();

		vec3 const* ambient = &CoreEngine::GetSceneManager()->GetAmbient();
		vec3 const* keyDir = &CoreEngine::GetSceneManager()->GetKeyLight().GetTransform().Forward();
		vec3 const* keyCol = &CoreEngine::GetSceneManager()->GetKeyLight().Color();

		LOG("Uploading light and matrix data to shaders");
		#if defined(DEBUG_RENDERMANAGER_SHADER_LOGGING)
			LOG("Ambient: " + to_string(ambient->r) + ", " + to_string(ambient->g) + ", " + to_string(ambient->b));
			LOG("Key Dir: " + to_string(keyDir->x) + ", " + to_string(keyDir->y) + ", " + to_string(keyDir->z));
			LOG("Key Col: " + to_string(keyCol->r) + ", " + to_string(keyCol->g) + ", " + to_string(keyCol->b));
		#endif

		for (unsigned int i = 0; i < numMaterials; i++)
		{
			Material* currentMaterial = sceneManager->GetMaterial(i);
			
			Shader* currentShader = currentMaterial->GetShader();
			RenderManager::BindShader(currentShader->ShaderReference());
			
			// Upload key light direction (world space) and color, and ambient light color:
			currentShader->UploadUniform("ambient", &(ambient->r), UNIFORM_Vec3fv);
			currentShader->UploadUniform("keyDirection", &(keyDir->x), UNIFORM_Vec3fv);
			currentShader->UploadUniform("keyColor", &(keyCol->r), UNIFORM_Vec3fv);

			// Upload matrices:
			mat4 projection = sceneManager->GetMainCamera()->Projection();
			currentShader->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

			RenderManager::BindShader(0);
		}
	}


}


