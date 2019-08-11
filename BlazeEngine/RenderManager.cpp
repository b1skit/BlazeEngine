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

		// Cache the relevant config data:
		this->xRes			= CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes;
		this->yRes			= CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes;
		this->windowTitle	= CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowTitle;

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
		glFrontFace(GL_CCW);				// Counter-clockwise vertex winding (OpenGL default)
		glEnable(GL_DEPTH_TEST);			// Enable Z depth testing
		glDepthFunc(GL_LESS);				// How to sort Z
		glEnable(GL_CULL_FACE);				// Enable face culling
		glCullFace(GL_BACK);				// Cull back faces
		
		// Set the default buffer clear values:
		glClearColor(GLclampf(windowClearColor.r), GLclampf(windowClearColor.g), GLclampf(windowClearColor.b), GLclampf(windowClearColor.a));
		glClearDepth((GLdouble)depthClearColor);
		ClearWindow(windowClearColor);

		// Configure deferred output:
		outputMaterial = new Material("RenderManager_OutputMaterial", CoreEngine::GetCoreEngine()->GetConfig()->shader.blitShader, (TEXTURE_TYPE)1, true);

		RenderTexture* outputTexture = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes,
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
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
	}


	void RenderManager::Update()
	{
		// TODO: Merge ALL meshes using the same material into a single draw call

		Camera* mainCam = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_MAIN).at(0);

		// Temporary hack: Render the keylight directly, since that's all we support at the moment... TODO: Loop through multiple lights
		glDisable(GL_CULL_FACE);
		if (CoreEngine::GetSceneManager()->GetKeyLight() != nullptr)
		{
			RenderLightShadowMap(CoreEngine::GetSceneManager()->GetKeyLight());
		}
		
		glEnable(GL_CULL_FACE);


		// TODO: Render reflection probes


		// Forward rendering:
		if (CoreEngine::GetCoreEngine()->GetConfig()->renderer.useForwardRendering)
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

			if (deferredLights->size() > 0)
			{
				// Render the first light
				RenderDeferredLight(deferredLights->at(0));

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE); // TODO: Can we just set this once somewhere, instead of calling each frame?

				for (int i = 1; i < deferredLights->size(); i++)
				{
					// Setup for screen aligned quads:
					if (deferredLights->at(i)->Type() == LIGHT_AMBIENT || deferredLights->at(i)->Type() == LIGHT_DIRECTIONAL)
					{
						glDisable(GL_DEPTH_TEST);
						glCullFace(GL_BACK);
					}
					else
					{
						glEnable(GL_DEPTH_TEST);
						glCullFace(GL_FRONT);
						glDepthFunc(GL_GREATER);				
					}

					RenderDeferredLight(deferredLights->at(i));
				}
			}

			// Additively blit the emissive GBuffer texture to screen additively:
			glDisable(GL_DEPTH_TEST);
			glCullFace(GL_BACK);
			Blit(mainCam->RenderMaterial(), TEXTURE_EMISSIVE, outputMaterial, TEXTURE_ALBEDO);


			// Deferred lights cleanup:
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glCullFace(GL_BACK);


			// Blit results to screen:
			BlitToScreen();
		}
		

		// Display the final frame:
		SDL_GL_SwapWindow(glWindow);
	}


	void RenderManager::RenderLightShadowMap(Light* currentLight)
	{
		Camera* shadowCam = currentLight->ActiveShadowMap()->ShadowCamera();

		// Configure the FrameBuffer:
		RenderTexture* lightDepthTexture = (RenderTexture*)shadowCam->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
		glViewport(0, 0, lightDepthTexture->Width(), lightDepthTexture->Height());
		glBindFramebuffer(GL_FRAMEBUFFER, lightDepthTexture->FBO());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the currently bound FBO

		// Bind:
		Shader* lightShader = shadowCam->RenderMaterial()->GetShader();
		BindShader(lightShader->ShaderReference());

		// Loop through each mesh:			
		vector<Mesh*> const* meshes = CoreEngine::GetSceneManager()->GetRenderMeshes();
		unsigned int numMeshes	= (unsigned int)meshes->size();
		for (unsigned int j = 0; j < numMeshes; j++)
		{
			Mesh* currentMesh = meshes->at(j);

			BindMeshBuffers(currentMesh);

			// Assemble model-specific matrices:
			mat4 mvp			= shadowCam->ViewProjection() * currentMesh->GetTransform().Model();

			// Upload mesh-specific matrices:
			lightShader->UploadUniform("in_mvp",				&mvp[0][0],				UNIFORM_Matrix4fv);

			// TODO: Only upload these matrices if they've changed ^^^^

			// Draw!
			glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
			
			// Cleanup current mesh:
			BindMeshBuffers();
		}

		// Cleanup:
		BindShader(0);
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
			BindShader(shaderReference);
			currentMaterial->BindAllTextures(shaderReference);

			// Upload material properties:
			currentShader->UploadUniform(Material::MATERIAL_PROPERTY_NAMES[MATERIAL_PROPERTY_0].c_str(), &currentMaterial->Property(MATERIAL_PROPERTY_0).x, UNIFORM_Vec4fv);

			// Get all meshes that use the current material
			meshes = CoreEngine::GetSceneManager()->GetRenderMeshes(currentMaterialIndex);

			// Loop through each mesh:			
			unsigned int numMeshes	= (unsigned int)meshes->size();
			for (unsigned int j = 0; j < numMeshes; j++)
			{
				Mesh* currentMesh = meshes->at(j);

				BindMeshBuffers(currentMesh);

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
				BindMeshBuffers();
			}

			// Cleanup:
			currentMaterial->BindAllTextures();
			BindShader(0);

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
		mat4 shadowCam_vp	= CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight()->ActiveShadowMap()->ShadowCamera()->ViewProjection();

		// Cache required values once outside of the loop:
		Light* keyLight						= CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight();
		Material* shadowCamRenderMaterial	= keyLight->ActiveShadowMap()->ShadowCamera()->RenderMaterial();

		// Loop by material (+shader), mesh:
		unsigned int numMaterials = CoreEngine::GetSceneManager()->NumMaterials();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Setup the current material and shader:
			Material* currentMaterial	= CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			Shader* currentShader		= currentMaterial->GetShader();		
			GLuint shaderReference		= currentShader->ShaderReference();

			// Bind:
			BindShader(shaderReference);
			currentMaterial->BindAllTextures(shaderReference);

			// Bind the key light depth buffer and related data:
			vec4 GBuffer_Depth_TexelSize(0, 0, 0, 0);
			RenderTexture* depthTexture = (RenderTexture*)keyLight->ActiveShadowMap()->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
			if (depthTexture)
			{
				depthTexture->Bind(shaderReference, DEPTH_TEXTURE_0 + TEXTURE_UNIT_SHADOW_DEPTH);

				GBuffer_Depth_TexelSize = vec4(1.0f / depthTexture->Width(), 1.0f / depthTexture->Height(), depthTexture->Width(), depthTexture->Height());
				// ^^ TODO: Compute this once and cache it for uploading
			}
			currentShader->UploadUniform("maxShadowBias",			&keyLight->ActiveShadowMap()->MaxShadowBias(),	UNIFORM_Float);
			currentShader->UploadUniform("minShadowBias",			&keyLight->ActiveShadowMap()->MinShadowBias(),	UNIFORM_Float);
			currentShader->UploadUniform("GBuffer_Depth_TexelSize", &GBuffer_Depth_TexelSize.x,						UNIFORM_Vec4fv);

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
				glDrawElements(GL_TRIANGLES, currentMesh->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

				// Cleanup current mesh: 
				BindMeshBuffers();
			}

			// Cleanup current material and shader:
			currentMaterial->BindAllTextures();
			if (depthTexture)
			{
				depthTexture->Bind(0, DEPTH_TEXTURE_0 + TEXTURE_UNIT_SHADOW_DEPTH);
			}
			BindShader(0);

		} // End Material loop
	}


	void BlazeEngine::RenderManager::RenderDeferredLight(Light* deferredLight)
	{
		Camera* renderCam = CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera();
		
		// Assemble common (model independent) matrices:
		bool hasShadowMap = deferredLight->ActiveShadowMap() != nullptr;
		mat4 shadowCam_vp(1.0);
		if (hasShadowMap)
		{
			shadowCam_vp = deferredLight->ActiveShadowMap()->ShadowCamera()->ViewProjection();
		}		

		mat4 model			= deferredLight->GetTransform().Model();
		mat4 view			= renderCam->View();
		mat4 mv				= view * model;
		mat4 mvp			= renderCam->ViewProjection() * deferredLight->GetTransform().Model();

		// Light properties:
		vec3 lightWorldPos	= deferredLight->GetTransform().WorldPosition();

		// Bind:
		Shader* currentShader	= deferredLight->DeferredMaterial()->GetShader();
		GLuint shaderReference	= currentShader->ShaderReference();

		BindShader(shaderReference);
		renderCam->RenderMaterial()->BindAllTextures(shaderReference);	// Bind GBuffer textures

		ShadowMap* activeShadowMap = deferredLight->ActiveShadowMap();
		if (activeShadowMap != nullptr)
		{
			vec4 GBuffer_Depth_TexelSize(0, 0, 0, 0);
			RenderTexture* depthTexture = (RenderTexture*)activeShadowMap->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
			if (depthTexture)
			{
				depthTexture->Bind(shaderReference, DEPTH_TEXTURE_0 + TEXTURE_UNIT_SHADOW_DEPTH);

				GBuffer_Depth_TexelSize = vec4(1.0f / depthTexture->Width(), 1.0f / depthTexture->Height(), depthTexture->Width(), depthTexture->Height());
				// ^^ TODO: Compute this once and cache it for uploading each frame
			}
			currentShader->UploadUniform("GBuffer_Depth_TexelSize", &GBuffer_Depth_TexelSize.x, UNIFORM_Vec4fv);

			currentShader->UploadUniform("maxShadowBias", &deferredLight->ActiveShadowMap()->MaxShadowBias(), UNIFORM_Float);
			currentShader->UploadUniform("minShadowBias", &deferredLight->ActiveShadowMap()->MinShadowBias(), UNIFORM_Float);
		}

		// Upload common shader matrices:
		if (hasShadowMap)
		{
			currentShader->UploadUniform("shadowCam_vp",	&shadowCam_vp[0][0],	UNIFORM_Matrix4fv);
		}		

		currentShader->UploadUniform("in_model",		&model[0][0],			UNIFORM_Matrix4fv);
		currentShader->UploadUniform("in_view",			&view[0][0],			UNIFORM_Matrix4fv);
		currentShader->UploadUniform("in_mv",			&mv[0][0],				UNIFORM_Matrix4fv);
		currentShader->UploadUniform("in_mvp",			&mvp[0][0],				UNIFORM_Matrix4fv);
		// TODO: Only upload these matrices if they've changed ^^^^
		// TODO: Break this out into a function: ALL of our render functions have a similar setup...
		
		// Upload current light's parameters:
		currentShader->UploadUniform("lightWorldDir",	&deferredLight->GetTransform().Forward().x, UNIFORM_Vec3fv); // Note: This only makes sense w.r.t KEYLIGHT's world direction...
		currentShader->UploadUniform("lightColor",		&deferredLight->Color().r,					UNIFORM_Vec3fv);
		currentShader->UploadUniform("lightWorldPos",	&lightWorldPos.x,							UNIFORM_Vec3fv);

		BindMeshBuffers(deferredLight->DeferredMesh());

		// Draw!
		glDrawElements(GL_TRIANGLES, deferredLight->DeferredMesh()->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		renderCam->RenderMaterial()->BindAllTextures();
		if (activeShadowMap != nullptr)
		{
			activeShadowMap->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH)->Bind(0, DEPTH_TEXTURE_0 + TEXTURE_UNIT_SHADOW_DEPTH);
		}
		BindShader(0);
		BindMeshBuffers();
	}


	void BlazeEngine::RenderManager::BlitToScreen()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		BindShader(outputMaterial->GetShader()->ShaderReference());
		outputMaterial->BindAllTextures(outputMaterial->GetShader()->ShaderReference());
		BindMeshBuffers(screenAlignedQuad);

		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		outputMaterial->BindAllTextures();
		BindShader(0);
		BindMeshBuffers();
	}

	void BlazeEngine::RenderManager::Blit(Material* srcMat, int srcTex, Material* dstMat, int dstTex)
	{
		// Bind the output FBO: (Textures MUST already be attached...)
		glBindFramebuffer(GL_FRAMEBUFFER, ((RenderTexture*)dstMat->AccessTexture((TEXTURE_TYPE)dstTex))->FBO());
		glViewport(0, 0, dstMat->AccessTexture((TEXTURE_TYPE)dstTex)->Width(), dstMat->AccessTexture((TEXTURE_TYPE)dstTex)->Height());

		// Bind the blit shader and screen aligned quad:
		GLuint shaderReference = outputMaterial->GetShader()->ShaderReference();
		BindShader(shaderReference);
		BindMeshBuffers(screenAlignedQuad);

		// Bind the source texture into the slot specified in the blit shader:
		srcMat->AccessTexture((TEXTURE_TYPE)srcTex)->Bind(shaderReference, RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO); // Bind to what the blitShader is expecting
		
		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		srcMat->AccessTexture((TEXTURE_TYPE)dstTex)->Bind(0, TEXTURE_ALBEDO);
		BindShader(0);
		BindMeshBuffers();
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


	void BlazeEngine::RenderManager::BindShader(GLuint const& shaderReference)
	{
		glUseProgram(shaderReference);
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

		// Legacy forward rendering parms:
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

		// Add all Material Shaders to a list:
		vector<Shader*> shaders;
		for (unsigned int i = 0; i < numMaterials; i++)
		{
			shaders.push_back(sceneManager->GetMaterial(i)->GetShader());
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
		
		// Add RenderManager shaders:
		shaders.push_back(outputMaterial->GetShader());


		// Configure all of the shaders:
		for (unsigned int i = 0; i < (int)shaders.size(); i++)
		{
			BindShader(shaders.at(i)->ShaderReference());

			// Upload Texture sampler locations. Note: These must align with the locations defined in Material.h
			for (int currentTexture = 0; currentTexture < TEXTURE_COUNT; currentTexture++)
			{
				GLint samplerLocation = glGetUniformLocation(shaders.at(i)->ShaderReference(), Material::TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, (TEXTURE_TYPE)currentTexture);
				}
			}
			// Upload RenderTexture sampler locations:
			for (int currentTexture = 0; currentTexture < RENDER_TEXTURE_COUNT; currentTexture++)
			{
				GLint samplerLocation = glGetUniformLocation(shaders.at(i)->ShaderReference(), Material::RENDER_TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, (int)(RENDER_TEXTURE_0 + (TEXTURE_TYPE)currentTexture));
				}
			}

			// Upload sampler location for shadow map textures
			for (int currentTexture = 0; currentTexture < DEPTH_TEXTURE_COUNT; currentTexture++)
			{
				GLint samplerLocation = glGetUniformLocation(shaders.at(i)->ShaderReference(), Material::DEPTH_TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, DEPTH_TEXTURE_0 + (TEXTURE_TYPE)currentTexture);
				}
			}
			
			// Upload light direction (world space) and color, and ambient light color:
			if (ambientLight != nullptr)
			{
				shaders.at(i)->UploadUniform("ambientColor", &(ambientColor->r), UNIFORM_Vec3fv);
			}			

			// NOTE: These values are overridden every frame for deferred light Shaders:
			if (keyLight != nullptr)
			{
				shaders.at(i)->UploadUniform("lightWorldDir", &(keyDir->x), UNIFORM_Vec3fv);
				shaders.at(i)->UploadUniform("lightColor", &(keyCol->r), UNIFORM_Vec3fv);
			}
			

			// Other params:
			shaders.at(i)->UploadUniform("screenParams", &(screenParams.x), UNIFORM_Vec4fv);

			// Upload matrices:
			mat4 projection = sceneManager->GetMainCamera()->Projection();
			shaders.at(i)->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

			BindShader(0);
		}
	}


}


