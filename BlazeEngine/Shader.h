#pragma once

#include <GL/glew.h>
#include <string>

using std::string;

namespace BlazeEngine
{
	class Shader
	{
	public:
		Shader(const string shaderName, const GLuint shaderReference);
		Shader(const Shader& existingShader);
		~Shader();

		// Getters/Setters:
		inline string Name() { return shaderName; }
		inline GLuint ShaderReference() const { return shaderReference; }

		// Static functions:
		static Shader* CreateShader(string shaderName);


	protected:


	private:
		string shaderName; // Extensionless filename of the shader. Will have ".vert" / ".frag" appended
		GLuint shaderReference;


		// Private static functions:
		static string LoadShaderFile(const string& filepath);
		static GLuint CreateGLShaderObject(const string& text, GLenum shaderType);
		static bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);
	};
}


