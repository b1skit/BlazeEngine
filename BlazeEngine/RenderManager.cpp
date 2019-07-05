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

		// /* Enable multisampling for a nice antialiased effect */
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);


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
		screenAlignedQuad = vector<Mesh*>(1);	// Only holds 1 element
		screenAlignedQuad.at(0) = new Mesh
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

		for (int i = 0; i < (int)screenAlignedQuad.size(); i++)
		{
			if (screenAlignedQuad.at(i) != nullptr)
			{
				screenAlignedQuad.at(i)->DestroyMesh();
				delete screenAlignedQuad.at(i);
				screenAlignedQuad.at(i) = nullptr;
			}
		}
		screenAlignedQuad.clear();

		if (gBufferDrawShader != nullptr)
		{
			gBufferDrawShader->Destroy();
			delete gBufferDrawShader;
			gBufferDrawShader = nullptr;
		}
	}


	void RenderManager::Update()
	{
		// Render shadow maps
		vector<Camera*> cameras = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_SHADOW);
		for (int currentCam = 0; currentCam < (int)cameras.size(); currentCam++)
		{
			ConfigureRenderSettings(cameras.at(currentCam));
			Render(cameras.at(currentCam));
		}

		// TODO: Render reflection probes

		// Fill GBuffer
		cameras = CoreEngine::GetSceneManager()->GetCameras(CAMERA_TYPE_MAIN);
		ConfigureRenderSettings(cameras.at(0));
		Render(cameras.at(0), true);


		//// TODO: Render scene using GBuffer
		//// TEMP: Detach main cameras render material, render, then reattach it
		//Material* temp = cameras.at(0)->RenderMaterial();
		//cameras.at(0)->RenderMaterial() = nullptr;

		//ConfigureRenderSettings(cameras.at(0));
		//Render(cameras.at(0));

		//cameras.at(0)->RenderMaterial() = temp;




		// Render the final image from the GBuffer:
		Material* temp = cameras.at(0)->RenderMaterial();
		cameras.at(0)->RenderMaterial() = nullptr;

		ConfigureRenderSettings(cameras.at(0)); // UNBIND FBO (since RenderMaterial is null

		// TEST: reattach material now that we've called ConfigureRenderSettings()
		cameras.at(0)->RenderMaterial() = temp;

		RenderFromGBuffer(cameras.at(0));

		//

		// PROBLEMS:
		// We're not unbinding the attached framebuffer UNTIL ConfigureRenderSettings is called AGAIN (w/cam w/no material)
		// We need to attach Camera's render textures for reading


		// Display the final frame:
		SDL_GL_SwapWindow(glWindow);
	}


	void RenderManager::ConfigureRenderSettings(Camera* const renderCam)
	{
		// TODO: Move various openGL config settings from startup to here?

		Material* renderMaterial = renderCam->RenderMaterial();

		// Render to FrameBuffer:
		if (renderMaterial != nullptr)
		{
			// Bind all RenderTextures:
			for (int i = 0; i < RENDER_TEXTURE_COUNT; i++)
			{
				RenderTexture* currentTexture = (RenderTexture*)renderMaterial->AccessTexture((TEXTURE_TYPE)i);
				if (currentTexture != nullptr)
				{
					// TODO: Should binding/unbinding be a RenderTexture member function??
					// Need a way to bind for reading OR writing
					// We could totally nuke this whole function then...
					glViewport(0, 0, currentTexture->Width(), currentTexture->Height());

					glBindFramebuffer(GL_FRAMEBUFFER, currentTexture->FBO());
					// TODO: we only need to set this once per material? No need to keep binding the same FBO...

					// Writing: Set the viewport, bind the FBO (ie this function)
					// Reading: Bind the render textures as textures, ?
				}
			}
		}
		else // Render to the view window:
		{
			glViewport(0, 0, xRes, yRes);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}


	void RenderManager::Render(Camera* renderCam, bool renderingToGBuffer /*= false*/)
	{

		// TODO: Merge ALL meshes using the same material into a single draw call


		// Assemble common (model independent) matrices:
		mat4 view			= renderCam->View();
		mat4 shadowCam_vp	= CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight().ActiveShadowMap()->ShadowCamera()->ViewProjection();

		// Configure render material:
		unsigned int numMaterials;
		Material* currentMaterial	= renderCam->RenderMaterial();
		bool renderingToScreen		= true;
		if (currentMaterial == nullptr || renderingToGBuffer) // Render to viewport
		{
			numMaterials = CoreEngine::GetSceneManager()->NumMaterials();
		}
		else // Render to FrameBuffers
		{
			numMaterials = 1;
			renderingToScreen = false;
		}

		// Clear the required buffers before rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO: Configure buffer clearing via Camera/Material/RenderTexture properties

		// Loop by material (+shader), mesh:
		Shader* currentShader				= nullptr;
		GLuint shaderReference				= 0;
		Light* keyLight						= &CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight();
		Material* shadowCamRenderMaterial	= keyLight->ActiveShadowMap()->ShadowCamera()->RenderMaterial();
		for (unsigned int currentMaterialIndex = 0; currentMaterialIndex < numMaterials; currentMaterialIndex++)
		{
			// Setup the current material and shader:
			if (renderingToScreen || renderingToGBuffer)
			{
				currentMaterial = CoreEngine::GetSceneManager()->GetMaterial(currentMaterialIndex);
			}
			
			if (renderingToGBuffer)
			{
				currentShader = renderCam->RenderMaterial()->GetShader();
			}
			else
			{
				currentShader = currentMaterial->GetShader();
			}			
			shaderReference = currentShader->ShaderReference();

			vector<Mesh*> const* meshes;

			// Bind:
			BindShader(shaderReference);
			if (renderingToScreen || renderingToGBuffer)
			{
				BindSamplers(currentMaterial);
				BindTextures(currentMaterial, shaderReference);

				if (!renderingToGBuffer) // No need to attach shadows for gbuffer...
				{
					// Bind the key light depth buffer and related data:
					BindFrameBuffers(shadowCamRenderMaterial, shaderReference); // TODO: CHECK IF I AM OR SHOULD BE UNBINDING THIS????????????
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
				}

				// Get all meshes that use the current material
				meshes = CoreEngine::GetSceneManager()->GetRenderMeshes(currentMaterialIndex);
			}
			else // Rendering to framebuffer: Get all the meshes in 1 pass
			{
				meshes = CoreEngine::GetSceneManager()->GetRenderMeshes();
			}

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
			if (renderingToScreen || renderingToGBuffer)
			{
				BindSamplers();
				BindTextures(currentMaterial);

				// Unbind the key light depth buffer
				if (!renderingToGBuffer)
				{
					BindFrameBuffers(currentMaterial);
				}
			}
			BindShader(0);
		
		} // End Material loop
	}


	void BlazeEngine::RenderManager::RenderFromGBuffer(Camera* renderCam)
	{
		// We're rendering w/a generic shader
		//	possibly, all shaders in the scene depending on a stencil mask... But for now, we'll just use phong

		// Assemble common (model independent) matrices:
		mat4 view			= renderCam->View();
		//mat4 shadowCam_vp	= CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight().ActiveShadowMap()->ShadowCamera()->ViewProjection();

		// Clear the required buffers before rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO: Configure buffer clearing via Camera/Material/RenderTexture properties
		
		GLuint shaderReference		= gBufferDrawShader->ShaderReference();

		// Bind:
		BindShader(gBufferDrawShader->ShaderReference());
		
		
		//BindTextures(renderCam->RenderMaterial(), shaderReference); // DOESN'T WORK FOR FRAMEBUFFERS

		//for (int i = 0; i < renderCam->RenderMaterial()->NumTextures(); i++)
		//{
		//	if (renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)i) != nullptr)
		//	{
		//		((RenderTexture*)renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)i))->AttachmentPoint() = GL_READ_FRAMEBUFFER;
		//	}
		//}

		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, ((RenderTexture*)renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)0))->FBO());

		BindFrameBuffers(renderCam->RenderMaterial(), shaderReference); // <-- Binds like textures

		//BindSamplers(renderCam->RenderMaterial()); // <-- This DOES make a difference for framebuffers!!!!!!!


		//Light* keyLight						= &CoreEngine::GetCoreEngine()->GetSceneManager()->GetKeyLight();
		//Material* shadowCamRenderMaterial	= keyLight->ActiveShadowMap()->ShadowCamera()->RenderMaterial();

		//BindFrameBuffers(shadowCamRenderMaterial, shaderReference);
		//gBufferDrawShader->UploadUniform("maxShadowBias", &keyLight->ActiveShadowMap()->MaxShadowBias(), UNIFORM_Float);
		//gBufferDrawShader->UploadUniform("minShadowBias", &keyLight->ActiveShadowMap()->MinShadowBias(), UNIFORM_Float);

		//// Shadow texture 
		//vec4 shadowDepth_TexelSize(0, 0, 0, 0);
		//RenderTexture* depthTexture = (RenderTexture*)keyLight->ActiveShadowMap()->ShadowCamera()->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH);
		//if (depthTexture)
		//{
		//	shadowDepth_TexelSize = vec4(1.0f / depthTexture->Width(), 1.0f / depthTexture->Height(), depthTexture->Width(), depthTexture->Height());
		//	// ^^ TODO: Compute this once and cache it for uploading
		//}
		//gBufferDrawShader->UploadUniform("shadowDepth_TexelSize", &shadowDepth_TexelSize.x, UNIFORM_Vec4fv);

		// Upload common shader matrices:
		gBufferDrawShader->UploadUniform("in_view", &view[0][0], UNIFORM_Matrix4fv);
		//gBufferDrawShader->UploadUniform("shadowCam_vp", &shadowCam_vp[0][0], UNIFORM_Matrix4fv);

		BindMeshBuffers(screenAlignedQuad.at(0));

		// TODO: Only upload this stuff if it has changed ^^^^


		// Draw!
		glDrawElements(GL_TRIANGLES, screenAlignedQuad.at(0)->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);


		for (int i = 0; i < renderCam->RenderMaterial()->NumTextures(); i++)
		{
			if (renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)i) != nullptr)
			{
				((RenderTexture*)renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)i))->AttachmentPoint() = GL_FRAMEBUFFER;
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, ((RenderTexture*)renderCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)0))->FBO());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		


		// Cleanup: 
		BindMeshBuffers();
		BindSamplers(); // <-- Wouldn't we need to call this for multiple textures???
						// Also, this is hard-coded for Textures. What about RTs?
		//BindTextures(shadowCamRenderMaterial);
		//BindFrameBuffers(shadowCamRenderMaterial);
		BindTextures(renderCam->RenderMaterial());
		BindFrameBuffers(renderCam->RenderMaterial());
		BindShader(0);
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


	void BlazeEngine::RenderManager::BindSamplers(Material* currentMaterial /* = nullptr*/) // Unbinds all samplers if currentMaterial == nullptr
	{
		// I THINK THIS IS ONLY APPROPRIATE FOR TEXTURES (NOT RT'S)
		// TODO: WRAP THIS INTO BindTextures, like for BindFrameBuffers
		if (currentMaterial != nullptr)
		{
			for (int i = 0; i < currentMaterial->NumTextures(); i++)
			{
				glBindSampler(i, currentMaterial->Samplers(i));
			}
		}
		else // Do cleanup:
		{
			//for (int i = 0; i < TEXTURE_COUNT; i++)
			for (int i = 0; i < RENDER_TEXTURE_COUNT; i++)
			{
				glBindSampler(i, 0); // Assign to index/unit 0
			}
		}
	}


	void BlazeEngine::RenderManager::BindTextures(Material* currentMaterial, GLuint const& shaderReference /* = 0 */) // If shaderReference == 0, unbinds textures
	{
		// Handle unbinding:
		if (shaderReference == 0)
		{
			for (int i = 0; i < currentMaterial->NumTextures(); i++)
			{
				Texture* currentTexture = currentMaterial->AccessTexture((TEXTURE_TYPE)i);
				if (currentTexture)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					glBindTexture(currentTexture->TextureTarget(), 0);
				}
			}
		}
		else
		{
			for (int i = 0; i < currentMaterial->NumTextures(); i++)
			{
				// TEMP HACK:
				bool isRenderTexture = currentMaterial->NumTextures() == RENDER_TEXTURE_COUNT ? true : false;

				GLint samplerLocation;
				if (isRenderTexture)
				{
					samplerLocation = glGetUniformLocation(shaderReference, Material::RENDER_TEXTURE_SAMPLER_NAMES[i].c_str());
				}
				else
				{
					samplerLocation = glGetUniformLocation(shaderReference, Material::TEXTURE_SAMPLER_NAMES[i].c_str());
				}
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, (TEXTURE_TYPE)i);
				}
				// ^^^ Should this be here???? Am I using samplers right???

				Texture* currentTexture = currentMaterial->AccessTexture((TEXTURE_TYPE)i);
				if (currentTexture)
				{
					if (isRenderTexture)
					{
						glActiveTexture(GL_TEXTURE0 + RENDER_TEXTURE_0 + (TEXTURE_TYPE)i);
						glBindTexture(currentTexture->TextureTarget(), currentTexture->TextureID());
					}
					else
					{
						glActiveTexture(GL_TEXTURE0 + (TEXTURE_TYPE)i);
						glBindTexture(currentTexture->TextureTarget(), currentTexture->TextureID());
					}
				}
				// IS THIS NECCESSARY??? ^^^^^
				//  (DONT THINK SO!!! REVERT THIS!!!!!)

				//GLint samplerLocation = glGetUniformLocation(shaderReference, Material::TEXTURE_SAMPLER_NAMES[i].c_str());
				//if (samplerLocation >= 0)
				//{
				//	glUniform1i(samplerLocation, (TEXTURE_TYPE)i);
				//}
				//// ^^^ Should this be here???? Am I using samplers right???

				//Texture* currentTexture = currentMaterial->AccessTexture((TEXTURE_TYPE)i);
				//if (currentTexture)
				//{
				//	glActiveTexture(GL_TEXTURE0 + (TEXTURE_TYPE)i);
				//	glBindTexture(currentTexture->TextureTarget(), currentTexture->TextureID());
				//}
			}
		}
	}


	void RenderManager::BindFrameBuffers(Material* currentMaterial, GLuint const& shaderReference /* = 0 */)		// If shaderReference == 0, unbinds textures
	{
		// TODO: Rename this to BindFrameBufferTextures

		if (currentMaterial == nullptr)
		{
			LOG_ERROR("Attempting to bind/unbind framebuffers, but received a null material");
			return;
		}

		// Unbind:
		if (shaderReference == 0)
		{
			for (int i = 0; i < currentMaterial->NumTextures(); i++)
			{
				Texture* currentTexture = currentMaterial->AccessTexture((TEXTURE_TYPE)i);
				if (currentTexture)
				{
					glActiveTexture(GL_TEXTURE0 + RENDER_TEXTURE_0 + i);
					glBindTexture(currentTexture->TextureTarget(), 0);
				}
			}
			return;
		}

		// Bind:
		for (int i = 0; i < currentMaterial->NumTextures(); i++)
		{
			Texture* currentTexture = currentMaterial->AccessTexture((TEXTURE_TYPE)i);
			if (currentTexture)
			{
				glActiveTexture(GL_TEXTURE0 + RENDER_TEXTURE_0 + i);
				glBindTexture(currentTexture->TextureTarget(), currentTexture->TextureID());
				// binds to TEXTURE_2D....

				// Bind samplers:
				GLint samplerLocation = glGetUniformLocation(shaderReference, Material::RENDER_TEXTURE_SAMPLER_NAMES[i].c_str());
				if (samplerLocation >= 0)
				{
					LOG_ERROR("FOUND SAMPLER: " + string(Material::RENDER_TEXTURE_SAMPLER_NAMES[i].c_str()));

					glUniform1i(samplerLocation, RENDER_TEXTURE_0 + i);
				}

				else
					LOG_ERROR("MISSING SAMPLER: " + string(Material::RENDER_TEXTURE_SAMPLER_NAMES[i].c_str()));

				//glActiveTexture(GL_TEXTURE0 + RENDER_TEXTURE_0 + i);
				//glBindTexture(currentTexture->TextureTarget(), currentTexture->TextureID());
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

		for (unsigned int i = 0; i < numMaterials; i++)
		{
			Material* currentMaterial = sceneManager->GetMaterial(i);
			
			Shader* currentShader = currentMaterial->GetShader();
			RenderManager::BindShader(currentShader->ShaderReference());
			
			// Upload light direction (world space) and color, and ambient light color:
			currentShader->UploadUniform("ambient", &(ambient->r), UNIFORM_Vec3fv);

			currentShader->UploadUniform("lightDirection", &(keyDir->x), UNIFORM_Vec3fv); // TODO: Move these to the main render loop once we've switched to deferred rendering w/multiple lights
			currentShader->UploadUniform("lightColor", &(keyCol->r), UNIFORM_Vec3fv);

			// Upload matrices:
			mat4 projection = sceneManager->GetMainCamera()->Projection();
			currentShader->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

			RenderManager::BindShader(0);
		}
	}


}


