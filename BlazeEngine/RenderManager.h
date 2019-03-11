// The Blaze rendering engine

#pragma once
#include "EngineComponent.h"
#include "EventManager.h"
#include "Shader.h"
#include <string>

#define GLM_FORCE_SWIZZLE
#include "glm.hpp"
#include "SDL.h"

using std::pair;
using glm::vec4;


namespace BlazeEngine
{
	enum SHADER // Guaranteed shaders
	{
		SHADER_ERROR = 0,
		SHADER_DEFAULT = 1,
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
		void Startup(CoreEngine * coreEngine);

		void Shutdown();

		void Update();

		// Member functions:
		void Render(double alpha);

		/*unsigned int GetShaderIndex(string shaderName);*/

		
	private:
		// Configuration:
		int xRes;
		int yRes;
		string windowTitle;
		
		// OpenGL components and settings:
		SDL_Window* glWindow;
		SDL_GLContext glContext;
		
		enum
		{
			VERTEX_BUFFER_POSITION,
			/*VERTEX_BUFFER_INDEXES,*/

			VERTEX_BUFFER_SIZE, // Reserved: Number of buffers to allocate
		};

		GLuint vertexBufferObjects[VERTEX_BUFFER_SIZE];		// Buffer objects that hold vertices in GPU memory
		//GLuint vertexArrayObjects[VERTEX_BUFFER_SIZE];		// States needed to supply vertex data
		GLuint vertexArrayObject;
		GLuint elementBufferObjects[VERTEX_BUFFER_SIZE];	// Triangle vertex indices
		/*GLuint indexBufferObjects[VERTEX_BUFFER_SIZE];*/

		//// Shaders:
		//vector<Shader> shaders;

		//int CreateShader(string shaderName);
		//string LoadShaderFile(const string& filepath);
		//GLuint CreateGLShaderObject(const string& text, GLenum shaderType);
		//bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);
		////void BindShader(int shaderIndex); // Set the active vertex/fragement shader

		// Private member functions:
		void ClearWindow(vec4 clearColor);

	};
}


