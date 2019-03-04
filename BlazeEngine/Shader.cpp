#include "Shader.h"
//#include <fstream>
//
//using std::ifstream;

//#include <iostream> // DEBUG
//using std::cout;


namespace BlazeEngine
{
	/*Shader::Shader(const string shaderName, const GLuint shaderReference, const unsigned int numShaders, GLuint* shaders)*/
	Shader::Shader(const string shaderName, const GLuint shaderReference)
	{
		this->shaderName = shaderName;
		this->shaderReference = shaderReference;
		//this->numShaders = numShaders;
		//this->shaders = shaders;
	}

	Shader::Shader(const Shader& existingShader)
	{
		this->shaderName = existingShader.shaderName;
		this->shaderReference = existingShader.shaderReference;
		//this->numShaders = existingShader.numShaders;
		//
		//this->shaders = new GLuint[this->numShaders];
		//for (unsigned int i = 0; i < numShaders; i++)
		//{
		//	this->shaders[i] = existingShader.shaders[i];
		//}
	}

	Shader::~Shader()
	{
		/*if (shaders != nullptr)
		{
			delete shaders;
		}	*/		
	}
}
