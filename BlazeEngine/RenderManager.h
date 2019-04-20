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


	enum SHADER // Guaranteed shaders
	{
		SHADER_ERROR = 0,		// Displays hot pink
		SHADER_DEFAULT = 1,		// Lambert shader
	};


	// Used for uploading uniforms
	enum UNIFORM_TYPE
	{
		UNIFORM_Matrix4fv,		// glUniformMatrix4fv
		UNIFORM_Vec3fv,			// glUniform3fv
brea
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
		void Render(double alpha);

		
	private:
		// Configuration:
		int xRes;
		int yRes;
		string windowTitle;
		
		// OpenGL components and settings:
		SDL_Window* glWindow;
		SDL_GLContext glContext;
		
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

		// Upload value(s) to a shader's uniform
		void UploadUniform(GLuint const& shaderReference, GLchar const* uniformName, GLfloat const* value, UNIFORM_TYPE const& type);

		// Bind the mesh VAO, position, and index buffers. If mesh == nullptr, binds all elements to index 0 (ie. for cleanup)
		void BindMeshBuffers(Mesh* const mesh = nullptr);


	};
}


