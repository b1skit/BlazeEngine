// The Blaze rendering engine

#pragma once

#include "EngineComponent.h"
#include "EventManager.h"
#include "Shader.h"

#include <string>


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
	class Texture;
	class Camera;


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
		void Render(Camera* renderCam);

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
		
		// Private member functions:
		//--------------------------

		// Clear the window and fill it with a color
		void ClearWindow(vec4 clearColor);

		// Sets the active shader
		void BindShader(GLuint const& shaderReference);

		// Bind a material's samplers to the currently bound shader. If doCleanup == true, binds to unit 0 (ie. for cleanup)
		void BindSamplers(Material* currentMaterial, bool doCleanup = false);

		// Bind a material's textures to the currently bound shader
		void BindTextures(GLuint const& shaderReference, Material* currentMaterial);

		// Bind the mesh VAO, position, and index buffers. If mesh == nullptr, binds all elements to index 0 (ie. for cleanup)
		void BindMeshBuffers(Mesh* const mesh = nullptr);
	};
}


