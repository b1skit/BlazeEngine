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
		Shader(const string shaderName, const GLuint shaderReference, const unsigned int numShaders, GLuint* shaders);
		Shader(const Shader& existingShader);
		~Shader();

		// Getters/Setters:
		inline string Name() { return shaderName; }
		inline GLuint ShaderReference() { return shaderReference; }
		inline unsigned int NumShaders() { return numShaders; }
		inline GLuint* Shaders() { return shaders; };

	protected:


	private:
		string shaderName; // Extensionless filename of the shader. Will have ".vert" / ".frag" appended
		GLuint shaderReference;
		unsigned int numShaders;
		GLuint* shaders;
	};
}


