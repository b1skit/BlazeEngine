#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <string>

using std::string;

namespace BlazeEngine
{
	class Shader
	{
	public:
		Shader() : Shader(DEFAULT_PATH) {} // Call the string-arg constructor with a default path
		Shader(const string filepath);
		~Shader();

		// Set the GPU to use the vertex/fragement shaders defined by this Shader
		void Bind();

	protected:


	private:
		static const unsigned int NUM_SHADERS = 2; // 2: Only define vertex and fragment shaders (3: Also use geometry shader)
		GLuint shaderProgram;
		GLuint shaders[NUM_SHADERS];

		const string DEFAULT_PATH = "./Shaders/defaultShader"; // Should this be static? Or, can we use the coreEngine.config value??
	};


	// Static Shader functions:
	static string LoadShader(const string& filepath);
	static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const string& errorMessage);
	static GLuint CreateShader(const string& text, GLenum shaderType);
}


