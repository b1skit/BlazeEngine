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
		Shader(const string shaderName);
		Shader(const Shader& existingShader);
		~Shader();

		//// Set the GPU to use the vertex/fragement shaders defined by this Shader
		//void Bind();

		// Getters/Setters:
		inline string Name() { return shaderName; }


		// Temporary public variables:
		unsigned int numShaders = 2;
		GLuint* shaders;
		GLuint shaderProgram;

	protected:


	private:
		//static const unsigned int NUM_SHADERS = 2; // 2: Only define vertex and fragment shaders (3: Also use geometry shader)
		//GLuint shaderProgram;
		//GLuint shaders[NUM_SHADERS];

		string shaderName; // Extensionless filename of the shader. Will have ".vert" / ".frag" appended
		
	};


	//// Static Shader functions:
	//static string LoadShader(const string& filepath);
	//static bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram, string errorMessage);
	//static GLuint CreateShader(const string& text, GLenum shaderType);
}


