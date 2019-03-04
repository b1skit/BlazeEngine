#include "Shader.h"


namespace BlazeEngine
{
	/*Shader::Shader(const string shaderName, const GLuint shaderReference, const unsigned int numShaders, GLuint* shaders)*/
	Shader::Shader(const string shaderName, const GLuint shaderReference)
	{
		this->shaderName = shaderName;
		this->shaderReference = shaderReference;
	}

	Shader::Shader(const Shader& existingShader)
	{
		this->shaderName = existingShader.shaderName;
		this->shaderReference = existingShader.shaderReference;
	}

	Shader::~Shader()
	{
	
	}
}
