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

		// Configure OpenGL for the next draw call
		void ConfigureRenderSettings(Camera* const renderCam);

		void Render(Camera* renderCam, bool attachMaterialTextures = false);
		void RenderFromGBuffer(Camera* renderCam);
		//TODO: Make these render functions private?

		// Upload static properties to shaders
		void InitializeShaders();

	private:
		// Configuration:
		int xRes				= -1;
		int yRes				= -1;
		string windowTitle		= "Default BlazeEngine window title";
		
		// OpenGL components and settings:
		SDL_Window* glWindow	= 0;
		SDL_GLContext glContext = 0;

		// GBuffer:
		vector<Mesh*> screenAlignedQuad;
		Shader* gBufferDrawShader	= nullptr;		// Deallocated in Shutdown()
		
		// Private member functions:
		//--------------------------

		// Clear the window and fill it with a color
		void ClearWindow(vec4 clearColor);

		// Sets the active shader
		void BindShader(GLuint const& shaderReference);

		// Bind a material's samplers to the currently bound shader. If doCleanup == true, binds to unit 0 (ie. for cleanup)
		void BindSamplers(Material* currentMaterial = nullptr);

		// Bind a material's textures to the currently bound shader
		void BindTextures(Material* currentMaterial, GLuint const& shaderReference = 0);	// If shaderReference == 0, unbinds textures

		// Bind a material's RenderTextures
		void BindFrameBuffers(Material* currentMaterial, GLuint const& shaderReference = 0);		// If shaderReference == 0, unbinds textures

		// Bind the mesh VAO, position, and index buffers. If mesh == nullptr, binds all elements to index 0 (ie. for cleanup)
		void BindMeshBuffers(Mesh* const mesh = nullptr);
	};
}


