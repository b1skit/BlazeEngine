#include "RenderManager.h"
#include "CoreEngine.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "BuildConfiguration.h"
#include "Skybox.h"

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
			case GL_DEBUG_SOURCE_API:
				output += "GL_DEBUG_SOURCE_API\n";
				break;
			case GL_DEBUG_SOURCE_APPLICATION: 
				output += "GL_DEBUG_SOURCE_APPLICATION\n";
					break;

			case GL_DEBUG_SOURCE_THIRD_PARTY:
				output += "GL_DEBUG_SOURCE_THIRD_PARTY\n";
					break;
			default:
				output += "CURRENTLY UNRECOGNIZED ENUM VALUE: " + to_string(source) + " (Todo: Convert to hex!)\n"; // If we ever hit this, we should add the enum as a new string
			}

			output += "Type: ";

			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:
				output += "GL_DEBUG_TYPE_ERROR\n";
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				output += "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR\n";
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				output += "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR\n";
				break;
			case GL_DEBUG_TYPE_PORTABILITY:
				output += "GL_DEBUG_TYPE_PORTABILITY\n";
				break;
			case GL_DEBUG_TYPE_PERFORMANCE:
				output += "GL_DEBUG_TYPE_PERFORMANCE\n";
				break;
			case GL_DEBUG_TYPE_OTHER:
				output += "GL_DEBUG_TYPE_OTHER\n";
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
					output += "GL_DEBUG_SEVERITY_LOW\n";
				break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				output += "GL_DEBUG_SEVERITY_MEDIUM\n";
				break;
			case GL_DEBUG_SEVERITY_HIGH:
				output += "GL_DEBUG_SEVERITY_HIGH\n";
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

		// Cache the relevant config data:
		this->windowTitle			= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("windowTitle");
		this->xRes					= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowXRes");
		this->yRes					= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowYRes");
		this->useForwardRendering	= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<bool>("useForwardRendering");

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

		//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32); // Crashes if uncommented???

		SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
		SDL_SetRelativeMouseMode(SDL_TRUE);	// Lock the mouse to the window

		//// Make our buffer swap syncronized with the monitor's vertical refresh:
		//SDL_GL_SetSwapInterval(1);

		// Create a window:
		glWindow = SDL_CreateWindow
		(
			this->windowTitle.c_str(),
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
		glFrontFace(GL_CCW);				// Counter-clockwise vertex winding (OpenGL default)
		glEnable(GL_DEPTH_TEST);			// Enable Z depth testing
		glDepthFunc(GL_LESS);				// How to sort Z
		glEnable(GL_CULL_FACE);				// Enable face culling
		glCullFace(GL_BACK);				// Cull back faces

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		
		// Set the default buffer clear values:
		glClearColor(GLclampf(windowClearColor.r), GLclampf(windowClearColor.g), GLclampf(windowClearColor.b), GLclampf(windowClearColor.a));
		glClearDepth((GLdouble)depthClearColor);
		ClearWindow(windowClearColor);

		// Configure deferred output:
		outputMaterial = new Material("RenderManager_OutputMaterial", CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("blitShader"), (TEXTURE_TYPE)1, true);

		RenderTexture* outputTexture = new RenderTexture
		(
			this->xRes,
			this->yRes,
			"RenderManagerFrameOutput",
			false,
			RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO
		);
		
		outputTexture->Format()				= GL_RGBA;		// Note: Using 4 channels for future flexibility
		outputTexture->InternalFormat()		= GL_RGBA32F;		

		outputTexture->TextureMinFilter()	= GL_LINEAR;	// Note: Output is black if this is GL_NEAREST_MIPMAP_LINEAR
		outputTexture->TextureMaxFilter()	= GL_LINEAR;

		outputTexture->AttachmentPoint()	= GL_COLOR_ATTACHMENT0 + 0;

		outputTexture->ReadBuffer()			= GL_COLOR_ATTACHMENT0 + 0;
		outputTexture->DrawBuffer()			= GL_COLOR_ATTACHMENT0 + 0;

		outputTexture->Buffer();

		outputMaterial->AccessTexture(TEXTURE_ALBEDO) = outputTexture;

		// PostFX Manager:
		postFXManager = new PostFXManager(); // Initialized when RenderManager.Initialize() is called

		screenAlignedQuad = new Mesh
		(
			Mesh::CreateQuad
			(
				vec3(-1.0f,	1.0f,	0.0f),	// TL
				vec3(1.0f,	1.0f,	0.0f),	// TR
				vec3(-1.0f,	-1.0f,	0.0f),	// BL
				vec3(1.0f,	-1.0f,	0.0f)	// BR
			)
		);
	}


	void RenderManager::Shutdown()
	{
		LOG("Render manager shutting down...");

		if (outputMaterial != nullptr)
		{
			outputMaterial->Destroy();
			delete outputMaterial;
			outputMaterial = nullptr;
		}

		if (screenAlignedQuad != nullptr)
		{
			screenAlignedQuad->Destroy();
			delete screenAlignedQuad;
			screenAlignedQuad = nullptr;
		}

		if (postFXManager != nullptr)
		{
			delete postFXManager;
			postFXManager = nullptr;
		}
	}


	void RenderManager::Update()
	{
		// TODO: Merge ALL meshes using the same material into a single draw call

		Camera* mainCam = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_MAIN).at(0);

		// Fill shadow maps:
		glDisable(GL_CULL_FACE);
		vector<Light*>const* deferredLights = &CoreEngine::GetSceneManager()->GetDeferredLights();
		if (deferredLights)
		{
			for (int i = 0; i < (int)deferredLights->size(); i++)
			{
				if (deferredLights->at(i)->ActiveShadowMap() != nullptr)
				{
					RenderLightShadowMap(deferredLights->at(i));
				}
			}
		}
		glEnable(GL_CULL_FACE);


		// TODO: Render reflection probes


		// Forward rendering:
		if (this->useForwardRendering)
		{
			RenderForward(mainCam);
		}
		// Deferred rendering:
		else 
		{
			// Fill GBuffer:
			RenderToGBuffer(mainCam);

			// Render deferred lights:
			glBindFramebuffer(GL_FRAMEBUFFER, ((RenderTexture*)outputMaterial->AccessTexture((TEXTURE_TYPE)0))->FBO());
			glViewport(0, 0, this->xRes, this->yRes);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the currently bound FBO

			vector<Light*>const* deferredLights = &CoreEngine::GetSceneManager()->GetDeferredLights();

			// Render additive contributions:
			glEnable(GL_BLEND);

			if (deferredLights->size() > 0)
			{
				// Render the first light
				RenderDeferredLight(deferredLights->at(0));
				
				glBlendFunc(GL_ONE, GL_ONE); // TODO: Can we just set this once somewhere, instead of calling each frame?
				glDepthFunc(GL_GEQUAL);

				for (int i = 1; i < deferredLights->size(); i++)
				{
					// Select face culling:
					if (deferredLights->at(i)->Type() == LIGHT_AMBIENT_COLOR || deferredLights->at(i)->Type() == LIGHT_AMBIENT_IBL || deferredLights->at(i)->Type() == LIGHT_DIRECTIONAL)
					{
						glCullFace(GL_BACK);
					}
					else
					{
						glCullFace(GL_FRONT);	// For 3D deferred light meshes, we render back faces so something is visible even while we're inside the mesh		
					}

					RenderDeferredLight(deferredLights->at(i));
				}
			}
			glCullFace(GL_BACK);

			// Render the skybox on top of the frame:
			glDisable(GL_BLEND);
			RenderSkybox(CoreEngine::GetSceneManager()->GetSkybox());

			// Additively blit the emissive GBuffer texture to screen:
			glEnable(GL_BLEND);
			Blit(mainCam->RenderMaterial(), TEXTURE_EMISSIVE, outputMaterial, TEXTURE_ALBEDO);
			glDisable(GL_BLEND);

			// Post process finished frame:
			Material* finalFrameMaterial	= outputMaterial;	// Init as something valid
			Shader* finalFrameShader		= outputMaterial->GetShader();
			postFXManager->ApplyPostFX(finalFrameMaterial, finalFrameShader);


			// Cleanup:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glCullFace(GL_BACK);

			// Blit results to screen (Using the final post processing shader pass supplied by the PostProcessingManager):
			BlitToScreen(finalFrameMaterial, finalFrameShader);
		}
		

		// Display the final frame:
		SDL_GL_SwapWindow(glWindow);
	}


	void RenderManager::RenderLightShadowMap(Light* currentLight)
	{
		Camera* shadowCam = currentLight->ActiveShadowMap()->ShadowCamera();

		// Bind:
		Shader* lightShader = shadowCam->RenderMaterial()->GetShader();
		lightShader->Bind(true);
		
		// Configure the FrameBuffer:
		RenderTexture* lightDepthTexture = nullptr;

		switch (currentLight->Type())
		{
		case LIGHT_DIRECTIONAL:
		{
			lightDepthTexture = (RenderTexture*)shadowCam->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
		}
		break;

		case LIGHT_POINT:
		{
			lightDepthTexture = (RenderTexture*)shadowCam->RenderMaterial()->AccessTexture(CUBE_MAP_0_RIGHT);

			mat4 shadowCamProjection	= shadowCam->Projection();
			vec3 lightWorldPos			= currentLight->GetTransform().WorldPosition();

			mat4 const* cubeMap_vps = shadowCam->CubeViewProjection();

			lightShader->UploadUniform("shadowCamCubeMap_vp",	&cubeMap_vps[0][0][0],	UNIFORM_Matrix4fv, 6);
			lightShader->UploadUniform("lightWorldPos",			&lightWorldPos.x,		UNIFORM_Vec3fv);
			lightShader->UploadUniform("shadowCam_near",		&shadowCam->Near(),		UNIFORM_Float);
			lightShader->UploadUniform("shadowCam_far",			&shadowCam->Far(),		UNIFORM_Float);
		}
		break;

		case LIGHT_AMBIENT_COLOR:
		case LIGHT_AMBIENT_IBL:
		case LIGHT_AREA:
		case LIGHT_SPOT:
		case LIGHT_TUBE:
		default: // This should never happen...
			lightShader->Bind(false);
			return; 
		}

		if (lightDepthTexture == nullptr)
		{
			lightShader->Bind(false);
			return;
		}

		glViewport(0, 0, lightDepthTexture->Width(), lightDepthTexture->Height());
		glBindFramebuffer(GL_FRAMEBUFFER, lightDepthTexture->FBO());
		glClear(GL_DEPTH_BUFFER_BIT); // Clear the currently bound FBO	

		// Loop through each mesh:			
		vector<Mesh*> const* meshes = CoreEngine::GetSceneManager()->GetRenderMeshes();
		unsigned int numMeshes	= (unsigned int)meshes->size();
		for (unsigned int j = 0; j < numMeshes; j++)
		{
			Mesh* currentMesh = meshes->at(j);

			currentMesh->Bind(true);

			switch (currentLight->Type())
			{
			case LIGHT_DIRECTIONAL:
			{
				mat4 mvp			= shadowCam->ViewProjection() * currentMesh->GetTransform().Model();
				lightShader->UploadUniform("in_mvp",	&mvp[0][0],		UNIFORM_Matrix4fv);
			}
			break;

			case LIGHT_POINT:
			{
				mat4 model = currentMesh->GetTransform().Model();
				lightShader->UploadUniform("in_model",	&model[0][0],	UNIFORM_Matrix4fv);
			}
			break;

			case LIGHT_AMBIENT_COLOR:
			case LIGHT_AMBIENT_IBL:
			case LIGHT_AREA:
			case LIGHT_SPOT:
			case LIGHT_TUBE:
			default:
				return; // This should never happen...
			}
			// TODO: ^^^^ Only upload these matrices if they've changed			

			// Draw!
			glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
						
			// Cleanup current mesh:
			currentMesh->Bind(false);
		}

		// Cleanup:
		lightShader->Bind(false);
	}


	void BlazeEngine::RenderManager::RenderToGBuffer(Camera* const renderCam)
	{
		// For now, we just find the first valid texture, and assume it's FBO is the one we want to bind:
		RenderTexture* renderTexture	= (RenderTexture*)renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)0);
		if (renderTexture == nullptr)
		{
			for (int i = 1; i < renderCam->RenderMaterial()->NumTextureSlots(); i++)
			{
				renderTexture = (RenderTexture*)renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)i);
				if (renderTexture != nullptr)
				{
					break;
				}
			}
		}

		GLuint gBufferFBO				= renderTexture->FBO();
		
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
		glViewport(0, 0, renderTexture->Width(), renderTexture->Height());
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the currently bound FBO

		// Assemble common (model independent) matrices:
		mat4 view			= renderCam->View();

		// Configure render material:
		unsigned int numMaterials	= CoreEngine::GetSceneManager()->NumMaterials();
		Material* currentMaterial	= renderCam->RenderMaterial();

		Shader* currentShader				= nullptr;
		GLuint shaderReference				= 0;

		// Loop by material (+shader), mesh:
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Setup the current material and shader:
			currentMaterial		= CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			currentShader		= renderCam->RenderMaterial()->GetShader();
			shaderReference		= currentShader->ShaderReference();

			vector<Mesh*> const* meshes;

			// Bind:
			currentShader->Bind(true);
			currentMaterial->BindAllTextures(shaderReference);

			// Upload material properties:
			currentShader->UploadUniform(Material::MATERIAL_PROPERTY_NAMES[MATERIAL_PROPERTY_0].c_str(), &currentMaterial->Property(MATERIAL_PROPERTY_0).x, UNIFORM_Vec4fv);
			currentShader->UploadUniform("in_view", &view[0][0], UNIFORM_Matrix4fv);

			// Get all meshes that use the current material
			meshes = CoreEngine::GetSceneManager()->GetRenderMeshes(currentMaterialIndex);

			// Loop through each mesh:			
			unsigned int numMeshes	= (unsigned int)meshes->size();
			for (unsigned int j = 0; j < numMeshes; j++)
			{
				Mesh* currentMesh = meshes->at(j);

				currentMesh->Bind(true);

				// Assemble model-specific matrices:
				mat4 model			= currentMesh->GetTransform().Model();
				mat4 modelRotation	= currentMesh->GetTransform().Model(WORLD_ROTATION);
				mat4 mvp			= renderCam->ViewProjection() * model;

				// Upload mesh-specific matrices:
				currentShader->UploadUniform("in_model",			&model[0][0],			UNIFORM_Matrix4fv);
				currentShader->UploadUniform("in_modelRotation",	&modelRotation[0][0],	UNIFORM_Matrix4fv);
				currentShader->UploadUniform("in_mvp",				&mvp[0][0],				UNIFORM_Matrix4fv);
				// TODO: Only upload these matrices if they've changed ^^^^

				// Draw!
				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

				// Cleanup current mesh: 
				currentMesh->Bind(false);
			}

			// Cleanup:
			currentMaterial->BindAllTextures();
			currentShader->Bind(false);

		} // End Material loop

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void RenderManager::RenderForward(Camera* renderCam)
	{
		glViewport(0, 0, this->xRes, this->yRes);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the currently bound FBO

		// Assemble common (model independent) matrices:
		mat4 view			= renderCam->View();
		mat4 shadowCam_vp = mat4(1.0f);
		if (CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight() && CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight()->ActiveShadowMap() && CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight()->ActiveShadowMap()->ShadowCamera())
		{
			shadowCam_vp = CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight()->ActiveShadowMap()->ShadowCamera()->ViewProjection();
		}

		// Cache required values once outside of the loop:
		Light* keyLight						= CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight();

		// Temp fix: If we don't have a keylight, abort. TODO: Implement forward rendering of all light types
		if (keyLight == nullptr)
		{
			LOG_ERROR("\nNo keylight detected.A keylight is currently required for forward rendering mode. Quitting!\n");
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this });
			return;
		}

		// Loop by material (+shader), mesh:
		unsigned int numMaterials = CoreEngine::GetSceneManager()->NumMaterials();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Setup the current material and shader:
			Material* currentMaterial	= CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			Shader* currentShader		= currentMaterial->GetShader();		
			GLuint shaderReference		= currentShader->ShaderReference();

			// Bind:
			currentShader->Bind(true);
			currentMaterial->BindAllTextures(shaderReference);

			// Bind the key light depth buffer and related data:
			vec4 texelSize(0, 0, 0, 0);
			RenderTexture* depthTexture = (RenderTexture*)keyLight->ActiveShadowMap()->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
			if (depthTexture)
			{
				depthTexture->Bind(shaderReference, DEPTH_TEXTURE_0 + DEPTH_TEXTURE_SHADOW);

				texelSize = depthTexture->TexelSize();
			}
			currentShader->UploadUniform("maxShadowBias",			&keyLight->ActiveShadowMap()->MaxShadowBias(),	UNIFORM_Float);
			currentShader->UploadUniform("minShadowBias",			&keyLight->ActiveShadowMap()->MinShadowBias(),	UNIFORM_Float);
			currentShader->UploadUniform("texelSize",				&texelSize.x,									UNIFORM_Vec4fv);

			// Get all meshes that use the current material
			vector<Mesh*> const* meshes = CoreEngine::GetSceneManager()->GetRenderMeshes(currentMaterialIndex);

			// Upload common shader matrices:
			currentShader->UploadUniform("in_view", &view[0][0], UNIFORM_Matrix4fv);
			currentShader->UploadUniform("shadowCam_vp", &shadowCam_vp[0][0], UNIFORM_Matrix4fv);

			// Loop through each mesh:			
			unsigned int numMeshes	= (unsigned int)meshes->size();
			for (unsigned int j = 0; j < numMeshes; j++)
			{
				Mesh* currentMesh = meshes->at(j);
				currentMesh->Bind(true);

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
				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

				// Cleanup current mesh: 
				currentMesh->Bind(false);
			}

			// Cleanup current material and shader:
			currentMaterial->BindAllTextures();
			if (depthTexture)
			{
				depthTexture->Bind(0, DEPTH_TEXTURE_0 + DEPTH_TEXTURE_SHADOW);
			}
			currentShader->Bind(false);

		} // End Material loop
	}


	void BlazeEngine::RenderManager::RenderDeferredLight(Light* deferredLight)
	{
		Camera* renderCam = CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera();

		// Bind:
		Shader* currentShader	= deferredLight->DeferredMaterial()->GetShader();
		GLuint shaderReference	= currentShader->ShaderReference();

		currentShader->Bind(true);
		renderCam->RenderMaterial()->BindAllTextures(shaderReference);	// Bind GBuffer textures
		
		// Assemble common (model independent) matrices:
		bool hasShadowMap = deferredLight->ActiveShadowMap() != nullptr;
			
		mat4 model			= deferredLight->GetTransform().Model();
		mat4 view			= renderCam->View();
		mat4 mv				= view * model;
		mat4 mvp			= renderCam->ViewProjection() * deferredLight->GetTransform().Model();

		currentShader->UploadUniform("in_model",		&model[0][0],			UNIFORM_Matrix4fv);
		currentShader->UploadUniform("in_view",			&view[0][0],			UNIFORM_Matrix4fv);
		currentShader->UploadUniform("in_mv",			&mv[0][0],				UNIFORM_Matrix4fv);
		currentShader->UploadUniform("in_mvp",			&mvp[0][0],				UNIFORM_Matrix4fv);
		// TODO: Only upload these matrices if they've changed ^^^^
		// TODO: Break this out into a function: ALL of our render functions have a similar setup		

		// Light properties:
		currentShader->UploadUniform("lightColor", &deferredLight->Color().r, UNIFORM_Vec3fv);

		switch (deferredLight->Type())
		{
		case LIGHT_AMBIENT_IBL:
		{
			// Bind IBL cubemaps:
			if (!this->useForwardRendering)
			{
				Texture* IEMCubemap = ((ImageBasedLight*)deferredLight)->GetIBLMaterial()->AccessTexture(CUBE_MAP_0_RIGHT);
				if (IEMCubemap != nullptr)
				{
					IEMCubemap->Bind(currentShader->ShaderReference(), CUBE_MAP_0 + CUBE_MAP_0_RIGHT);
				}
			}
		}
			break;

		case LIGHT_DIRECTIONAL:
		{
			currentShader->UploadUniform("keylightWorldDir", &deferredLight->GetTransform().Forward().x, UNIFORM_Vec3fv);

			vec3 keylightViewDir = glm::normalize(view * vec4(deferredLight->GetTransform().Forward(), 0.0f));
			currentShader->UploadUniform("keylightViewDir", &keylightViewDir.x, UNIFORM_Vec3fv);
		}
			break;

		case LIGHT_AMBIENT_COLOR:
		case LIGHT_POINT:
		case LIGHT_SPOT:
		case LIGHT_AREA:
		case LIGHT_TUBE:
		{
			currentShader->UploadUniform("lightWorldPos", &deferredLight->GetTransform().WorldPosition().x, UNIFORM_Vec3fv);

			// TODO: Can we just upload this once when the light is created (and its shader is created)?  (And also update it if the light is ever moved)
		}
			break;
		default:
			break;
		}

		// Shadow properties:
		ShadowMap* activeShadowMap = deferredLight->ActiveShadowMap();

		mat4 shadowCam_vp(1.0);
		if (activeShadowMap != nullptr)
		{
			Camera* shadowCam = activeShadowMap->ShadowCamera();

			if (shadowCam != nullptr)
			{
				// Upload common shadow properties:
				currentShader->UploadUniform("shadowCam_vp",	&shadowCam->ViewProjection()[0][0],	UNIFORM_Matrix4fv);
				
				currentShader->UploadUniform("maxShadowBias",	&activeShadowMap->MaxShadowBias(),	UNIFORM_Float);
				currentShader->UploadUniform("minShadowBias",	&activeShadowMap->MinShadowBias(),	UNIFORM_Float);

				currentShader->UploadUniform("shadowCam_near",	&shadowCam->Near(),					UNIFORM_Float);
				currentShader->UploadUniform("shadowCam_far",	&shadowCam->Far(),					UNIFORM_Float);

				// Bind shadow depth textures:
				RenderTexture* depthTexture = nullptr;
				switch (deferredLight->Type())
				{
				case LIGHT_DIRECTIONAL:
				{
					depthTexture = (RenderTexture*)activeShadowMap->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
					if (depthTexture)
					{
						depthTexture->Bind(shaderReference, DEPTH_TEXTURE_0 + DEPTH_TEXTURE_SHADOW);
					}
				}
				break;

				case LIGHT_POINT:
				{
					depthTexture = (RenderTexture*)activeShadowMap->ShadowCamera()->RenderMaterial()->AccessTexture(CUBE_MAP_0_RIGHT);
					if (depthTexture)
					{
						depthTexture->Bind(shaderReference); // No need for a texture unit override
					}
				}
				break;

				// Other light types don't support shadows, yet:
				case LIGHT_AMBIENT_COLOR:
				case LIGHT_AMBIENT_IBL:
				case LIGHT_AREA:
				case LIGHT_SPOT:
				case LIGHT_TUBE:
				default:
					break;
				}

				vec4 texelSize(0, 0, 0, 0);
				if (depthTexture != nullptr)
				{
					texelSize = depthTexture->TexelSize();
				}
				currentShader->UploadUniform("texelSize", &texelSize.x, UNIFORM_Vec4fv);
			}
		}
		
		deferredLight->DeferredMesh()->Bind(true);

		// Draw!
		glDrawElements(GL_TRIANGLES, deferredLight->DeferredMesh()->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);


		// Cleanup:
		renderCam->RenderMaterial()->BindAllTextures();

		if (activeShadowMap != nullptr)
		{
			switch (deferredLight->Type())
			{
			case LIGHT_DIRECTIONAL:
			{
				activeShadowMap->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH)->Bind(0, DEPTH_TEXTURE_0 + DEPTH_TEXTURE_SHADOW);
			}			
			break;

			case LIGHT_POINT:
			{
				activeShadowMap->ShadowCamera()->RenderMaterial()->AccessTexture(CUBE_MAP_0_RIGHT)->Bind(0);
			}
			break;

			// Other light types don't support shadows, yet:
			case LIGHT_AMBIENT_COLOR:
			case LIGHT_AMBIENT_IBL:
			case LIGHT_AREA:
			case LIGHT_SPOT:
			case LIGHT_TUBE:
			default:
				break;
			}
		}
		
		currentShader->Bind(false);
		deferredLight->DeferredMesh()->Bind(false);
	}


	void BlazeEngine::RenderManager::RenderSkybox(Skybox* skybox)
	{
		if (skybox == nullptr)
		{
			return;
		}

		Camera* renderCam = CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera();

		// Bind:
		Shader* currentShader	= skybox->GetSkyMaterial()->GetShader();
		GLuint shaderReference	= currentShader->ShaderReference();

		currentShader->Bind(true);

		Texture* skyboxCubeMap = skybox->GetSkyMaterial()->AccessTexture(CUBE_MAP_0_RIGHT);

		skyboxCubeMap->Bind(shaderReference);

		Texture* depthTexture = (RenderTexture*)renderCam->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
		if (depthTexture)
		{
			depthTexture->Bind(shaderReference, RENDER_TEXTURE_0 + RENDER_TEXTURE_DEPTH);
		}

		// Assemble common (model independent) matrices:
		mat4 inverseViewProjection = glm::inverse(renderCam->ViewProjection()); // TODO: Only compute this if something has changed
		currentShader->UploadUniform("in_inverse_vp", &inverseViewProjection[0][0], UNIFORM_Matrix4fv);
				
		skybox->GetSkyMesh()->Bind(true);

		// Draw!
		glDrawElements(GL_TRIANGLES, skybox->GetSkyMesh()->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		skyboxCubeMap->Bind(0);
		depthTexture->Bind(0, RENDER_TEXTURE_0 + RENDER_TEXTURE_DEPTH);
		currentShader->Bind(false);
		skybox->GetSkyMesh()->Bind(false);
	}


	void BlazeEngine::RenderManager::BlitToScreen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		outputMaterial->GetShader()->Bind(true);
		outputMaterial->BindAllTextures(outputMaterial->GetShader()->ShaderReference());
		screenAlignedQuad->Bind(true);

		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		outputMaterial->BindAllTextures();
		outputMaterial->GetShader()->Bind(false);
		screenAlignedQuad->Bind(false);
	}


	void BlazeEngine::RenderManager::BlitToScreen(Material* srcMaterial, Shader* blitShader)
	{
		glViewport(0, 0, this->xRes, this->yRes);
			
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		blitShader->Bind(true);
		srcMaterial->BindAllTextures(blitShader->ShaderReference());
		screenAlignedQuad->Bind(true);

		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		srcMaterial->BindAllTextures();
		blitShader->Bind(false);
		screenAlignedQuad->Bind(false);
	}


	void BlazeEngine::RenderManager::Blit(Material* srcMat, int srcTex, Material* dstMat, int dstTex, Shader* shaderOverride /*= nullptr*/)
	{
		Shader* currentShader = shaderOverride;
		if (currentShader == nullptr)
		{
			currentShader = outputMaterial->GetShader(); // Output material has the blit shader attached to it
		}

		// Bind the output FBO: (Textures MUST already be attached...)
		glBindFramebuffer(GL_FRAMEBUFFER, ((RenderTexture*)dstMat->AccessTexture((TEXTURE_TYPE)dstTex))->FBO());
		glViewport(0, 0, dstMat->AccessTexture((TEXTURE_TYPE)dstTex)->Width(), dstMat->AccessTexture((TEXTURE_TYPE)dstTex)->Height());

		// Bind the blit shader and screen aligned quad:
		currentShader->Bind(true);
		screenAlignedQuad->Bind(true);

		// Bind the source texture into the slot specified in the blit shader:
		srcMat->AccessTexture((TEXTURE_TYPE)srcTex)->Bind(currentShader->ShaderReference(), RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO); // Note: Blit shader reads from this texture unit (for now)
		
		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		srcMat->AccessTexture((TEXTURE_TYPE)dstTex)->Bind(0, TEXTURE_ALBEDO);
		currentShader->Bind(false);
		screenAlignedQuad->Bind(false);
	}

	
	void RenderManager::ClearWindow(vec4 clearColor)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Set the initial color in both buffers:
		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(glWindow);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void BlazeEngine::RenderManager::Initialize()
	{
		SceneManager* sceneManager	= CoreEngine::GetSceneManager();
		unsigned int numMaterials	= sceneManager->NumMaterials();

		// Legacy forward rendering params:
		Light const* ambientLight	= nullptr;
		vec3 const* ambientColor	= nullptr;
		if ((ambientLight = CoreEngine::GetSceneManager()->GetAmbientLight()) != nullptr)
		{
			ambientColor = &CoreEngine::GetSceneManager()->GetAmbientLight()->Color();
		}

		vec3 const* keyDir			= nullptr;
		vec3 const* keyCol			= nullptr;
		Light const* keyLight		= nullptr;
		if ((keyLight = CoreEngine::GetSceneManager()->GetKeyLight()) != nullptr)
		{
			keyDir = &CoreEngine::GetSceneManager()->GetKeyLight()->GetTransform().Forward();
			keyCol = &CoreEngine::GetSceneManager()->GetKeyLight()->Color();
		}

		LOG("Uploading light and matrix data to shaders");
		#if defined(DEBUG_RENDERMANAGER_SHADER_LOGGING)
			LOG("Ambient: " + to_string(ambientColor->r) + ", " + to_string(ambientColor->g) + ", " + to_string(ambientColor->b));
			LOG("Key Dir: " + to_string(keyDir->x) + ", " + to_string(keyDir->y) + ", " + to_string(keyDir->z));
			LOG("Key Col: " + to_string(keyCol->r) + ", " + to_string(keyCol->g) + ", " + to_string(keyCol->b));
		#endif

		vec4 screenParams = vec4(this->xRes, this->yRes, 1.0f / this->xRes, 1.0f / this->yRes);
		vec4 projectionParams = vec4(1.0f, CoreEngine::GetSceneManager()->GetMainCamera()->Near(), CoreEngine::GetSceneManager()->GetMainCamera()->Far(), 1.0f / CoreEngine::GetSceneManager()->GetMainCamera()->Far());

		// Add all Material Shaders to a list:
		vector<Shader*> shaders;
		for (unsigned int i = 0; i < numMaterials; i++)
		{
			if (sceneManager->GetMaterial(i)->GetShader() != nullptr)
			{
				shaders.push_back(sceneManager->GetMaterial(i)->GetShader());
			}			
		}

		// Add all Camera Shaders:	
		for (int i = 0; i < CAMERA_TYPE_COUNT; i++)
		{
			vector<Camera*> cameras = CoreEngine::GetCoreEngine()->GetSceneManager()->GetCameras((CAMERA_TYPE)i);
			for (int currentCam = 0; currentCam < cameras.size(); currentCam++)
			{
				if (cameras.at(currentCam)->RenderMaterial() && cameras.at(currentCam)->RenderMaterial()->GetShader())
				{
					shaders.push_back(cameras.at(currentCam)->RenderMaterial()->GetShader());
				}
			}
		}
			
		// Add deferred light Shaders
		vector<Light*> const* deferredLights = &CoreEngine::GetCoreEngine()->GetSceneManager()->GetDeferredLights();
		for (int currentLight = 0; currentLight < (int)deferredLights->size(); currentLight++)
		{
			shaders.push_back(deferredLights->at(currentLight)->DeferredMaterial()->GetShader());
		}

		// Add skybox shader:
		Skybox* skybox = CoreEngine::GetSceneManager()->GetSkybox();
		if (skybox && skybox->GetSkyMaterial() && skybox->GetSkyMaterial()->GetShader())
		{
			shaders.push_back(skybox->GetSkyMaterial()->GetShader());
		}		
		
		// Add RenderManager shaders:
		shaders.push_back(outputMaterial->GetShader());

		// Configure all of the shaders:
		for (unsigned int i = 0; i < (int)shaders.size(); i++)
		{
			shaders.at(i)->Bind(true);

			// Upload light direction (world space) and color, and ambient light color:
			if (ambientLight != nullptr)
			{
				shaders.at(i)->UploadUniform("ambientColor", &(ambientColor->r), UNIFORM_Vec3fv);
			}			

			// NOTE: These values are overridden every frame for deferred light Shaders:
			if (keyLight != nullptr && this->useForwardRendering == true)
			{
				shaders.at(i)->UploadUniform("keylightWorldDir", &(keyDir->x), UNIFORM_Vec3fv);

				shaders.at(i)->UploadUniform("lightColor", &(keyCol->r), UNIFORM_Vec3fv);
			}

			// TODO: Shift more value uploads into the shader creation flow
			
			// Other params:
			shaders.at(i)->UploadUniform("screenParams", &(screenParams.x), UNIFORM_Vec4fv);
			shaders.at(i)->UploadUniform("projectionParams", &(projectionParams.x), UNIFORM_Vec4fv);

			float emissiveIntensity = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<float>("defaultSceneEmissiveIntensity");
			shaders.at(i)->UploadUniform("emissiveIntensity", &emissiveIntensity, UNIFORM_Float);
			// TODO: Load this from .FBX file, and set the cached value here


			// Upload matrices:
			mat4 projection = sceneManager->GetMainCamera()->Projection();
			shaders.at(i)->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

			shaders.at(i)->Bind(false);
		}

		// Initialize PostFX:
		postFXManager->Initialize(outputMaterial);
	}


}


