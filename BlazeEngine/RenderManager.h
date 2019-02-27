// The Blaze rendering engine

#pragma once
#include "EngineComponent.h"
#include "EventManager.h"
#include "Shader.h"
#include <string>

#include "glm.hpp"
#include "SDL.h"

using std::pair;
using glm::vec4;


namespace BlazeEngine
{
	enum SHADER // Guaranteed shaders
	{
		SHADER_DEFAULT = 0,
		/*SHADER_ERROR = 1,*/

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

		unsigned int GetShaderIndex(string shaderName);

		
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
			//VERTEX_BUFFER_NORMAL,

			VERTEX_BUFFER_SIZE, // Reserved: Number of buffers to allocate
		};

		GLuint vertexArrayBuffers[VERTEX_BUFFER_SIZE];
		GLuint vertexArrayObject;		

		// Shaders:
		vector<Shader> shaders;

		Shader CreateShader(string shaderName);

		//string LoadShader(const string& shaderName);
		//bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);
		////GLuint
		//Shader CreateShader(const string& text, GLenum shaderType);
		//

		// Private member functions:
		void ClearWindow(vec4 clearColor);

	};
}


