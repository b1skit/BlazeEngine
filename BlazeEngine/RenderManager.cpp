#include "RenderManager.h"
#include "CoreEngine.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
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
		glEnable(GL_CULL_FACE);				// Enable face culling
		glCullFace(GL_BACK);				// Cull back faces
		glDepthFunc(GL_LESS);				// How to sort Z
		glClearDepth((GLdouble)1.0);		// Set the default depth buffer clear value

		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));

		// Create a screen aligned quad mesh for rendering the GBuffer:
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

		gBufferDrawShader = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->shader.gBufferDrawShaderName);
	}


	void RenderManager::Shutdown()
	{
		LOG("Render manager shutting down...");

		if (screenAlignedQuad != nullptr)
		{
			screenAlignedQuad->DestroyMesh();
			delete screenAlignedQuad;
			screenAlignedQuad = nullptr;
		}

		if (gBufferDrawShader != nullptr)
		{
			gBufferDrawShader->Destroy();
			delete gBufferDrawShader;
			gBufferDrawShader = nullptr;
		}
	}


	void RenderManager::Update()
	{
		// TODO: Merge ALL meshes using the same material into a single draw call


		// Temporary hack: Render the keylight directly, since that's all we support at the moment... TODO: Loop through multiple lights
		RenderLightShadowMap(&CoreEngine::GetSceneManager()->GetKeyLight());


		// TODO: Render reflection probes


		// Fill GBuffer:
		Camera* mainCam = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_MAIN).at(0);
		RenderToGBuffer(mainCam);

		// //Forward render: Leaving this around for a while for debug purposes
		//RenderForward(mainCam);

		// Render from GBuffer:
		RenderFromGBuffer(mainCam);

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

		Shader* lightShader				= shadowCam->RenderMaterial()->GetShader();		

		// Bind:
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

		// Cleanup current material and shader:
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
			BindTextures(currentMaterial, shaderReference);

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
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			BindTextures(currentMaterial);
			BindShader(0);

		} // End Material loop
	}


	void RenderManager::RenderForward(Camera* renderCam)
	{
		glViewport(0, 0, this->xRes, this->yRes);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the currently bound FBO

		// Assemble common (model independent) matrices:
		mat4 view			= renderCam->View();
		mat4 shadowCam_vp	= CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight().ActiveShadowMap()->ShadowCamera()->ViewProjection();

		// Cache required values once outside of the loop:
		Light* keyLight						= &CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight();
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
			BindTextures(currentMaterial, shaderReference);

			// Bind the key light depth buffer and related data:
			BindTextures(shadowCamRenderMaterial, shaderReference);

			currentShader->UploadUniform("maxShadowBias", &keyLight->ActiveShadowMap()->MaxShadowBias(), UNIFORM_Float);
			currentShader->UploadUniform("minShadowBias", &keyLight->ActiveShadowMap()->MinShadowBias(), UNIFORM_Float);

			// Shadow texture 
			vec4 shadowDepth_TexelSize(0, 0, 0, 0);
			RenderTexture* depthTexture = (RenderTexture*)keyLight->ActiveShadowMap()->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
			if (depthTexture)
			{
				shadowDepth_TexelSize = vec4(1.0f / depthTexture->Width(), 1.0f / depthTexture->Height(), depthTexture->Width(), depthTexture->Height());
				// ^^ TODO: Compute this once and cache it for uploading
			}
			currentShader->UploadUniform("shadowDepth_TexelSize", &shadowDepth_TexelSize.x, UNIFORM_Vec4fv);

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
			BindTextures(currentMaterial);

			// Unbind the key light depth buffer
			BindTextures(currentMaterial);
			BindTextures(shadowCamRenderMaterial, 0);
			BindShader(0);

		} // End Material loop
	}


	void BlazeEngine::RenderManager::RenderFromGBuffer(Camera* renderCam)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, this->xRes, this->yRes);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the currently bound FBO

		// Assemble common (model independent) matrices:
		mat4 view			= renderCam->View();
		mat4 shadowCam_vp	= CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight().ActiveShadowMap()->ShadowCamera()->ViewProjection();

		Shader* currentShader			= gBufferDrawShader;
		GLuint shaderReference			= gBufferDrawShader->ShaderReference();
	
		// Bind:
		BindShader(shaderReference);

		BindTextures(renderCam->RenderMaterial(), shaderReference);

		BindMeshBuffers(screenAlignedQuad);

		// Upload common shader matrices:
		currentShader->UploadUniform("in_view", &view[0][0], UNIFORM_Matrix4fv);
		currentShader->UploadUniform("shadowCam_vp", &shadowCam_vp[0][0], UNIFORM_Matrix4fv);

		// Draw!
		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		BindTextures(renderCam->RenderMaterial(), 0);
		BindShader(0);
		BindMeshBuffers();
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


	// TODO: Should this be a per-texture member function ????
	void BlazeEngine::RenderManager::BindTextures(Material* currentMaterial, GLuint const& shaderReference /* = 0 */) // If shaderReference == 0, unbinds textures
	{
		// Temp hack: We'll need to replace this with some sort of isRenderMaterial flag in material if ever TEXTURE_COUNT == RENDER_TEXTURE_COUNT
		bool isRenderTexture = currentMaterial->NumTextureSlots() == RENDER_TEXTURE_COUNT;
		int textureOffset = 0;
		if (isRenderTexture)
		{
			textureOffset = RENDER_TEXTURE_0;
		}

		// Handle unbinding:
		if (shaderReference == 0)
		{
			for (int i = 0; i < currentMaterial->NumTextureSlots(); i++)
			{
				Texture* currentTexture = currentMaterial->AccessTexture((TEXTURE_TYPE)i);
				if (currentTexture)
				{
					glActiveTexture(GL_TEXTURE0 + textureOffset + i);
					glBindTexture(currentTexture->TextureTarget(), 0);
					glBindSampler(i, 0); // Assign to index/unit 0
				}
			}
		}
		else // Handle binding:
		{
			for (int i = 0; i < currentMaterial->NumTextureSlots(); i++)
			{
				Texture* currentTexture = currentMaterial->AccessTexture((TEXTURE_TYPE)i);
				if (currentTexture)
				{
					glActiveTexture(GL_TEXTURE0 + textureOffset + i);
					glBindTexture(currentTexture->TextureTarget(), currentTexture->TextureID());
					//glBindSampler(i, currentMaterial->Samplers(i));
					glBindSampler(i, currentTexture->Sampler()); // TODO: MOVE THIS WHOLE FUNCTION INTO THE TEXTURE!!!!!!!!!!!!!!!!!!!!!!!!!
				}
			}
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
			
		// Add the GBuffer Shader:
		shaders.push_back(this->gBufferDrawShader);


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

			// Upload light direction (world space) and color, and ambient light color:
			shaders.at(i)->UploadUniform("ambient", &(ambient->r), UNIFORM_Vec3fv);

			shaders.at(i)->UploadUniform("lightDirection", &(keyDir->x), UNIFORM_Vec3fv); // TODO: Move these to the main render loop once we've switched to deferred rendering w/multiple lights
			shaders.at(i)->UploadUniform("lightColor", &(keyCol->r), UNIFORM_Vec3fv);

			// Upload matrices:
			mat4 projection = sceneManager->GetMainCamera()->Projection();
			shaders.at(i)->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

			BindShader(0);
		}








		//

		//for (unsigned int i = 0; i < numMaterials; i++)
		//{
		//	Material* currentMaterial = sceneManager->GetMaterial(i);
		//	
		//	Shader* currentShader = currentMaterial->GetShader();
		//	BindShader(currentShader->ShaderReference());

		//	// Upload Texture sampler locations. Note: These must align with the locations defined in Material.h
		//	for (int currentTexture = 0; currentTexture < TEXTURE_COUNT; currentTexture++)
		//	{
		//		GLint samplerLocation = glGetUniformLocation(currentShader->ShaderReference(), Material::TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
		//		if (samplerLocation >= 0)
		//		{
		//			glUniform1i(samplerLocation, (TEXTURE_TYPE)currentTexture);
		//		}
		//	}
		//	// Upload RenderTexture sampler locations:
		//	for (int currentTexture = 0; currentTexture < RENDER_TEXTURE_COUNT; currentTexture++)
		//	{
		//		GLint samplerLocation = glGetUniformLocation(currentShader->ShaderReference(), Material::RENDER_TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
		//		if (samplerLocation >= 0)
		//		{
		//			glUniform1i(samplerLocation, (int)(RENDER_TEXTURE_0 + (TEXTURE_TYPE)currentTexture));
		//		}
		//	}
		//	
		//	// Upload light direction (world space) and color, and ambient light color:
		//	currentShader->UploadUniform("ambient", &(ambient->r), UNIFORM_Vec3fv);

		//	currentShader->UploadUniform("lightDirection", &(keyDir->x), UNIFORM_Vec3fv); // TODO: Move these to the main render loop once we've switched to deferred rendering w/multiple lights
		//	currentShader->UploadUniform("lightColor", &(keyCol->r), UNIFORM_Vec3fv);

		//	// Upload matrices:
		//	mat4 projection = sceneManager->GetMainCamera()->Projection();
		//	currentShader->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

		//	BindShader(0);
		//}


		//// TEMP HACK:
		//// TODO: Roll this into a loop 

		//// Configure the GBuffer draw shader:
		//BindShader(this->gBufferDrawShader->ShaderReference());

		//// Upload Texture sampler locations. Note: These must align with the locations defined in Material.h
		//for (int currentTexture = 0; currentTexture < TEXTURE_COUNT; currentTexture++)
		//{
		//	GLint samplerLocation = glGetUniformLocation(this->gBufferDrawShader->ShaderReference(), Material::TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
		//	if (samplerLocation >= 0)
		//	{
		//		glUniform1i(samplerLocation, (TEXTURE_TYPE)currentTexture);
		//	}
		//}
		//// Upload RenderTexture sampler locations:
		//for (int currentTexture = 0; currentTexture < RENDER_TEXTURE_COUNT; currentTexture++)
		//{
		//	GLint samplerLocation = glGetUniformLocation(this->gBufferDrawShader->ShaderReference(), Material::RENDER_TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
		//	if (samplerLocation >= 0)
		//	{
		//		glUniform1i(samplerLocation, (int)(RENDER_TEXTURE_0 + (TEXTURE_TYPE)currentTexture));
		//	}
		//}

		//// Upload light direction (world space) and color, and ambient light color:
		//this->gBufferDrawShader->UploadUniform("ambient", &(ambient->r), UNIFORM_Vec3fv);

		//this->gBufferDrawShader->UploadUniform("lightDirection", &(keyDir->x), UNIFORM_Vec3fv); // TODO: Move these to the main render loop once we've switched to deferred rendering w/multiple lights
		//this->gBufferDrawShader->UploadUniform("lightColor", &(keyCol->r), UNIFORM_Vec3fv);

		//// Upload matrices:
		//mat4 projection = sceneManager->GetMainCamera()->Projection();
		//this->gBufferDrawShader->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

		//BindShader(0);



		//// Configure the GBuffer fill shader:
		//BindShader(CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->GetShader()->ShaderReference());

		//// Upload Texture sampler locations. Note: These must align with the locations defined in Material.h
		//for (int currentTexture = 0; currentTexture < TEXTURE_COUNT; currentTexture++)
		//{
		//	GLint samplerLocation = glGetUniformLocation(CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->GetShader()->ShaderReference(), Material::TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
		//	if (samplerLocation >= 0)
		//	{
		//		glUniform1i(samplerLocation, (TEXTURE_TYPE)currentTexture);
		//	}
		//}
		//// Upload RenderTexture sampler locations:
		//for (int currentTexture = 0; currentTexture < RENDER_TEXTURE_COUNT; currentTexture++)
		//{
		//	GLint samplerLocation = glGetUniformLocation(CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->GetShader()->ShaderReference(), Material::RENDER_TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
		//	if (samplerLocation >= 0)
		//	{
		//		glUniform1i(samplerLocation, (int)(RENDER_TEXTURE_0 + (TEXTURE_TYPE)currentTexture));
		//	}
		//}

		//// Upload light direction (world space) and color, and ambient light color:
		//CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->GetShader()->UploadUniform("ambient", &(ambient->r), UNIFORM_Vec3fv);

		//CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->GetShader()->UploadUniform("lightDirection", &(keyDir->x), UNIFORM_Vec3fv); // TODO: Move these to the main render loop once we've switched to deferred rendering w/multiple lights
		//CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->GetShader()->UploadUniform("lightColor", &(keyCol->r), UNIFORM_Vec3fv);

		//// Upload matrices:
		////mat4 projection = sceneManager->GetMainCamera()->Projection();
		//CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->GetShader()->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

		//BindShader(0);
	}


}


