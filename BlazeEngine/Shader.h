#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

using std::string;


namespace BlazeEngine
{
	// Used for uploading shader uniforms
	enum UNIFORM_TYPE
	{
		UNIFORM_Matrix4fv,		// glUniformMatrix4fv
		UNIFORM_Matrix3fv,		// glUniformMatrix3fv
		UNIFORM_Vec3fv,			// glUniform3fv

	};


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

		void UploadUniform(GLchar const* uniformName, GLfloat const* value, UNIFORM_TYPE const& type);

		// Static functions:
		static Shader* CreateShader(string shaderName);


	protected:


	private:
		string shaderName		= "uninitializedShader"; // Extensionless filename of the shader. Will have ".vert" / ".frag" appended
		GLuint shaderReference	= 0;


		// Private static functions:
		//--------------------------
		
		// Helper function: Attempts to load and return the error shader. Returns nullptr if the error shader can't be loaded
		static Shader* ReturnErrorShader(string shaderName);

		// Helper function: Loads the contents of a file named "filepath" within the shaders directory
		static string LoadShaderFile(const string& filepath);

		// Helper function: Processes #include directives, loading included files from within the shaders directory
		static void LoadIncludes(string& shaderText);

		static GLuint CreateGLShaderObject(const string& text, GLenum shaderType);
		static bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);

	};
}


