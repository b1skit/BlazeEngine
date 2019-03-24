// Shader object

#include "Shader.h"
#include "CoreEngine.h"

#include <fstream>
using std::ifstream;


namespace BlazeEngine
{
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


	// Static functions:
	//*******************

	Shader* Shader::CreateShader(string shaderName)
	{
		CoreEngine::GetCoreEngine()->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Loading shader: " + shaderName) });

		GLuint shaderReference;
		unsigned int numShaders = 2; // TO DO : Allow loading of geometry shaders?
		GLuint* shaders = new GLuint[numShaders];

		// Create an empty shader program object, and get its reference:
		shaderReference = glCreateProgram();

		// Load the shader files:
		string vertexShader = LoadShaderFile(shaderName + ".vert");
		string fragmentShader = LoadShaderFile(shaderName + ".frag");
		if (vertexShader == "" || fragmentShader == "")
		{
			CoreEngine::GetCoreEngine()->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("Creating shader failed while loading shader files") });
			return nullptr;
		}

		// Create shader objects and attach them to the program objects:
		shaders[0] = CreateGLShaderObject(vertexShader, GL_VERTEX_SHADER);
		shaders[1] = CreateGLShaderObject(fragmentShader, GL_FRAGMENT_SHADER);
		for (unsigned int i = 0; i < numShaders; i++)
		{
			glAttachShader(shaderReference, shaders[i]); // Attach our shaders to the shader program
		}

		//// Associate our vertex attribute indexes with named variables:
		//glBindAttribLocation(shaderReference, 0, "in_position"); // Bind attribute 0 as "position" in the vertex shader
		//glBindAttribLocation(shaderReference, 1, "in_normal");
		//glBindAttribLocation(shaderReference, 2, "in_color");
		//glBindAttribLocation(shaderReference, 3, "in_uv0");
		//glBindAttribLocation(shaderReference, 4, "in_model");
		//glBindAttribLocation(shaderReference, 5, "in_view");
		//glBindAttribLocation(shaderReference, 6, "in_projection");
		//glBindAttribLocation(shaderReference, 7, "in_mv");
		//glBindAttribLocation(shaderReference, 8, "in_mvp");
		//glBindAttribLocation(shaderReference, 9, "ambient");
		//glBindAttribLocation(shaderReference, 10, "keyDirection");
		//glBindAttribLocation(shaderReference, 11, "keyColor");
		//glBindAttribLocation(shaderReference, 12, "keyIntensity");
		//// TO DO: Replace indexes with an enum??
		//// This isn't really needed, as we explicitely define locations in the shader...

		// Link our program object:
		glLinkProgram(shaderReference);
		if (!CheckShaderError(shaderReference, GL_LINK_STATUS, true))
		{
			return nullptr;
		}

		// Validate our program objects can execute with our current OpenGL state:
		glValidateProgram(shaderReference);
		if (!CheckShaderError(shaderReference, GL_VALIDATE_STATUS, true))
		{
			return nullptr;
		}

		// Delete the shader objects now that they've been linked into the program object:
		glDeleteShader(shaders[0]);
		glDeleteShader(shaders[1]);
		delete shaders;

		Shader* newShader = new Shader(shaderName, shaderReference);

		CoreEngine::GetCoreEngine()->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Successfully loaded " + shaderName) });

		return newShader;
	}

	string Shader::LoadShaderFile(const string& filename)
	{
		// Assemble the full shader file path:
		string filepath = CoreEngine::GetCoreEngine()->GetConfig()->shader.shaderDirectory + filename;

		ifstream file;
		file.open(filepath.c_str());

		string output;
		string line;
		if (file.is_open())
		{
			while (file.good())
			{
				getline(file, line);
				output.append(line + "\n");
			}
		}
		else
		{
			CoreEngine::GetCoreEngine()->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("LoadShaderFile failed: Could not open shader " + filepath) });

			return "";
		}

		return output;
	}

	GLuint Shader::CreateGLShaderObject(const string& shaderCode, GLenum shaderType)
	{
		GLuint shader = glCreateShader(shaderType);
		if (shader == 0)
		{
			CoreEngine::GetCoreEngine()->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("glCreateShader failed!") });
		}

		const GLchar* shaderSourceStrings[1];
		GLint shaderSourceStringLengths[1];

		shaderSourceStrings[0] = shaderCode.c_str();
		shaderSourceStringLengths[0] = (GLint)shaderCode.length();

		glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);
		glCompileShader(shader);

		CheckShaderError(shader, GL_COMPILE_STATUS, false);

		return shader;
	}

	bool Shader::CheckShaderError(GLuint shader, GLuint flag, bool isProgram)
	{
		GLint success = 0;
		GLchar error[1024] = { 0 }; // Error buffer

		if (isProgram)
		{
			glGetProgramiv(shader, flag, &success);
		}
		else
		{
			glGetShaderiv(shader, flag, &success);
		}

		if (success == GL_FALSE)
		{
			if (isProgram)
			{
				glGetProgramInfoLog(shader, sizeof(error), nullptr, error);
			}
			else
			{
				glGetShaderInfoLog(shader, sizeof(error), nullptr, error);
			}

			string errorAsString(error);

			CoreEngine::GetCoreEngine()->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("CheckShaderError failed: " + errorAsString) });

			return false;
		}
		else
		{
			return true;
		}
	}
}
