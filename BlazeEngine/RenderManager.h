// The Blaze rendering engine

#pragma once

#include "EngineComponent.h"
#include "EventManager.h"
#include "PostFXManager.h"

#include <string>

#include <GL/glew.h>

#include "SDL.h"

#define GLM_FORCE_SWIZZLE
#include "glm.hpp"

using glm::vec4;


namespace BlazeEngine
{
	// Pre-declarations:
	//------------------
	class Material;
	class Mesh;
	class Camera;
	class Shader;
	class Light;
	class Skybox;


	enum SHADER // Guaranteed shaders
	{
		SHADER_ERROR = 0,		// Displays hot pink
		SHADER_DEFAULT = 1,		// Lambert shader
	};

	
	class RenderManager : public EngineComponent
	{
	public:
		RenderManager() : EngineComponent("RenderManager") {}

		~RenderManager();

		// Singleton functionality:
		static RenderManager& Instance();
		RenderManager(RenderManager const&) = delete; // Disallow copying of our Singleton
		void operator=(RenderManager const&) = delete;

		// EngineComponent interface:
		void Startup();

		void Shutdown();

		void Update();

		
		// Member functions:
		//------------------

		// Perform post scene load initialization (eg. Upload static properties to shaders, initialize PostFX):
		void Initialize();


	private:
		void RenderLightShadowMap(Light* currentLight);
		//void RenderReflectionProbe();

		void RenderToGBuffer(Camera* const renderCam);	// Note: renderCam MUST have an attached GBuffer

		void RenderForward(Camera* renderCam);

		void RenderDeferredLight(Light* deferredLight); // Note: FBO, viewport

		void RenderSkybox(Skybox* skybox);

		void BlitToScreen();
		void BlitToScreen(Material* srcMaterial, Shader* blitShader);

		void Blit(Material* srcMat, int srcTex, Material* dstMat, int dstTex, Shader* shaderOverride = nullptr);


		// Configuration:
		//---------------
		int xRes					= -1;
		int yRes					= -1;
		string windowTitle			= "Default BlazeEngine window title";

		bool useForwardRendering	= false;

		vec4 windowClearColor		= vec4(0.0f, 0.0f, 0.0f, 0.0f);
		float depthClearColor		= 1.0f;
		
		// OpenGL components and settings:
		SDL_Window* glWindow		= 0;
		SDL_GLContext glContext		= 0;

		Material* outputMaterial	= nullptr;	// Deallocated in Shutdown()
		Mesh* screenAlignedQuad		= nullptr;	// Deallocated in Shutdown()

		// PostFX:
		PostFXManager* postFXManager = nullptr;	// Deallocated in Shutdown()

		
		// Private member functions:
		//--------------------------

		// Clear the window and fill it with a color
		void ClearWindow(vec4 clearColor);

	};
}


