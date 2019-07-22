// The Blaze rendering engine

#pragma once

#include "EngineComponent.h"
#include "EventManager.h"

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

		// Upload static properties to shaders
		void InitializeShaders();


	private:
		void RenderLightShadowMap(Light* currentLight);
		//void RenderReflectionProbe();

		void RenderToGBuffer(Camera* const renderCam);	// Note: renderCam MUST have an attached GBuffer

		void RenderForward(Camera* renderCam);

		void RenderDeferredLight(Light* deferredLight); // Note: FBO, viewport

		void BlitToScreen();


		// Configuration:
		//---------------
		int xRes					= -1;
		int yRes					= -1;
		string windowTitle			= "Default BlazeEngine window title";

		vec4 windowClearColor		= vec4(0.0f, 0.0f, 0.0f, 0.0f);
		float depthClearColor		= 1.0f;
		
		// OpenGL components and settings:
		SDL_Window* glWindow		= 0;
		SDL_GLContext glContext		= 0;

		Material* outputMaterial	= nullptr;
		Mesh* screenAlignedQuad		= nullptr;
		
		// Private member functions:
		//--------------------------

		// Clear the window and fill it with a color
		void ClearWindow(vec4 clearColor);

		// Sets the active shader
		void BindShader(GLuint const& shaderReference);

		// Bind a material's Textures/RenderTextures and samplers to the currently bound shader
		void BindTextures(Material* currentMaterial, GLuint const& shaderReference = 0);	// If shaderReference == 0, unbinds textures

		// Bind the mesh VAO, position, and index buffers. If mesh == nullptr, binds all elements to index 0 (ie. for cleanup)
		void BindMeshBuffers(Mesh* const mesh = nullptr);
	};
}


