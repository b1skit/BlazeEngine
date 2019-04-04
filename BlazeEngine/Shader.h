#pragma once

#include <GL/glew.h>
#include <string>

using std::string;

namespace BlazeEngine
{
	class Shader
	{
	public:
		Shader() {} // Do nothing
		Shader(const string shaderName, const GLuint shaderReference);
		Shader(const Shader& existingShader);
		~Shader();

		// Getters/Setters:
		inline string const& Name() { return shaderName; }
		inline GLuint ShaderReference() const { return shaderReference; }

		// Static functions:
		static Shader* CreateShader(string shaderName);


	protected:


	private:
		string shaderName		= "uninitializedShader"; // Extensionless filename of the shader. Will have ".vert" / ".frag" appended
		GLuint shaderReference	= 0;


		// Private static functions:
		static string LoadShaderFile(const string& filepath);
		static GLuint CreateGLShaderObject(const string& text, GLenum shaderType);
		static bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);
	};
}


