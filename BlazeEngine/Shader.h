#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

using std::string;
using std::vector;


namespace BlazeEngine
{
	// Used for uploading shader uniforms
	enum UNIFORM_TYPE
	{
		UNIFORM_Matrix4fv,		// glUniformMatrix4fv
		UNIFORM_Matrix3fv,		// glUniformMatrix3fv
		UNIFORM_Vec3fv,			// glUniform3fv
		UNIFORM_Vec4fv,			// glUniform4fv
		UNIFORM_Float,			// glUniform1f
	};


	// Shader #define keywords
	enum SHADER_KEYWORDS
	{
		NO_ALBEDO_TEXTURE,
		NO_NORMAL_TEXTURE,
		NO_EMISSIVE_TEXTURE,
		NO_RMAO_TEXTURE, 
		NO_COSINE_POWER,

		SHADER_KEYWORD_COUNT	// RESERVED: How many shader keywords we have
	}; // Note: If new enums are added, don't forget to update Shader::TEXTURE_SAMPLER_NAMES[] as well!


	class Shader
	{
	public:
		Shader() {} // Do nothing
		Shader(const string shaderName, const GLuint shaderReference);
		Shader(const Shader& existingShader);

		/*~Shader() {}*/

		void Destroy();

		// Getters/Setters:
		inline string const& Name()						{ return shaderName; }
		inline GLuint const& ShaderReference() const	{ return shaderReference; }

		void UploadUniform(GLchar const* uniformName, GLfloat const* value, UNIFORM_TYPE const& type, int count = 1);


		// Static functions:
		//------------------
		static Shader* CreateShader(string shaderName, vector<string> const*  shaderKeywords = nullptr);


		// Static members:
		const static string SHADER_KEYWORDS[SHADER_KEYWORD_COUNT];

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

		// Helper function: Inserts #define statements into shader text
		static void InsertDefines(string& shaderText, vector<string> const* shaderKeywords);

		static GLuint CreateGLShaderObject(const string& text, GLenum shaderType);
		static bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);

	};
}


