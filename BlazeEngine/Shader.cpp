#include "Shader.h"
#include <fstream>

using std::ifstream;

#include <iostream> // DEBUG
using std::cout;


namespace BlazeEngine
{
	Shader::Shader(const string shaderName)
	{
		this->shaderName = shaderName; // ERROR: Currently, this name includes the file path...

		shaders = new GLuint[numShaders]; // TO DO: Get numShaders as a parameter

		//shaderProgram = glCreateProgram();

		//shaders[0] = CreateShader(LoadShader(shaderName + ".vert"), GL_VERTEX_SHADER);
		//shaders[1] = CreateShader(LoadShader(shaderName + ".frag"), GL_FRAGMENT_SHADER);

		//for (int i = 0; i < NUM_SHADERS; i++)
		//{
		//	glAttachShader(shaderProgram, shaders[i]); // Attach our shaders to the shader program
		//}

		//// 
		//glBindAttribLocation(shaderProgram, 0, "position"); // Bind attribute 0 to the "position" variable in the vertex shader

		//// Link:
		//glLinkProgram(shaderProgram);
		//CheckShaderError(shaderProgram, GL_LINK_STATUS, true, "Error: Shader program linking failed: ");

		//// Validate:
		//glValidateProgram(shaderProgram);
		//CheckShaderError(shaderProgram, GL_VALIDATE_STATUS, true, "Error: Shader program is invalid: ");
	}

	Shader::Shader(const Shader& existingShader)
	{
		this->shaderReference = existingShader.shaderReference;
		this->numShaders = existingShader.numShaders;
		this->shaders = new GLuint[this->numShaders];

		this->shaderName = existingShader.shaderName;
	}

	Shader::~Shader()
	{
		delete shaders;

		//// Detach and delete the shader:
		//for (int i = 0; i < NUM_SHADERS; i++)
		//{
		//	glDetachShader(shaderProgram, shaders[i]);
		//	glDeleteShader(shaderProgram);
		//}

		//// Delete the shader program:
		//glDeleteProgram(shaderProgram);
	}

	//void Shader::Bind()
	//{
	//	glUseProgram(shaderProgram);
	//}

	//static string LoadShader(const string& filepath)
	//{
	//	ifstream file;
	//	file.open(filepath.c_str());

	//	string output;
	//	string line;
	//	if (file.is_open())
	//	{
	//		while (file.good())
	//		{
	//			getline(file, line);
	//			output.append(line + "\n");
	//		}
	//	}
	//	else
	//	{
	//		//return ""; // Return an empty string if we fail
	//		// TO DO: TRIGGER AN ERROR EVENT IF THIS FAILS!!!!!!!!!!

	//		cout << "DEBUG: Shader.LoadShader() FAILED! UNABLE TO LOAD SHADER " << filepath << "\n";
	//	}

	//	return output;
	//}

	//static bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram, string errorMessage)
	//{
	//	GLint success = 0;
	//	GLchar error[1024] = { 0 }; // Error buffer

	//	if (isProgram)
	//	{
	//		glGetProgramiv(shader, flag, &success);
	//	}
	//	else
	//	{
	//		glGetShaderiv(shader, flag, &success);
	//	}

	//	if (success == GL_FALSE)
	//	{
	//		if (isProgram)
	//		{
	//			glGetProgramInfoLog(shader, sizeof(error), nullptr, error);
	//		}
	//		else
	//		{
	//			glGetShaderInfoLog(shader, sizeof(error), nullptr, error);
	//		}
	//		
	//		//cout << "DEBUG: Shader.CheckShaderError() FAILED:\n" << error << "\n"; // TO DO: TRIGGER AN ERROR EVENT IF THIS FAILS!!!!!!!!!!

	//		string errorAsString(error);
	//		errorMessage = "Shader.CheckShaderError() failed: " + errorAsString;

	//		return false;
	//	}
	//	else
	//	{
	//		return true;
	//	}
	//}

	//static GLuint CreateShader(const string& text, GLenum shaderType)
	//{
	//	GLuint shader = glCreateShader(shaderType);
	//	if (shader == 0)
	//	{
	//		cout << "Error: Shader.CreateShader() failed\n"; // TO DO: TRIGGER AN ERROR EVENT IF THIS FAILS!!!!!!!!!!
	//	}

	//	const GLchar* shaderSourceStrings[1];
	//	GLint shaderSourceStringLengths[1];

	//	shaderSourceStrings[0] = text.c_str();
	//	shaderSourceStringLengths[0] = (GLint)text.length();

	//	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);
	//	glCompileShader(shader);

	//	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader compilation failed: ");

	//	return shader;
	//}
}
