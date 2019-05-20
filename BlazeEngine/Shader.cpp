// Shader object

#include "Shader.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

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


	void Shader::UploadUniform(GLchar const* uniformName, GLfloat const* value, UNIFORM_TYPE const& type)
	{
		GLint currentProgram;
		bool isBound = true;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		if (currentProgram != this->shaderReference)
		{
			glUseProgram(this->shaderReference);
			isBound = false;
		}

		GLuint uniformID = glGetUniformLocation(this->shaderReference, uniformName);
		if (uniformID >= 0)
		{
			switch (type)
			{
			case UNIFORM_Matrix4fv:
				glUniformMatrix4fv(uniformID, 1, GL_FALSE, value);	// Location, count, transpose?, value
				break;

			case UNIFORM_Matrix3fv:
				glUniformMatrix3fv(uniformID, 1, GL_FALSE, value);
				break;

			case UNIFORM_Vec3fv:
				glUniform3fv(uniformID, 1, value);					// Location, count, value
				break;

			default:
				LOG_ERROR("Shader uniform upload failed: Recieved invalid uniform type");
			}
		}

		if (!isBound)
		{
			glUseProgram(currentProgram);
		}
	}

	// Static functions:
	//*******************

	Shader* Shader::CreateShader(string shaderName)
	{
		LOG("Creating shader \"" + shaderName + "\"");

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
			glDeleteProgram(shaderReference);
			ReturnErrorShader(shaderName);
		}

		// Process #include directives:
		#if defined(DEBUG_SHADER_SETUP_LOGGING)
			LOG("Processing loaded Vertex shader")
		#endif
		LoadIncludes(vertexShader);

		#if defined(DEBUG_SHADER_SETUP_LOGGING)
			LOG("Processing loaded Fragment shader")
		#endif
		LoadIncludes(fragmentShader);

		// Create shader objects and attach them to the program objects:
		shaders[0] = CreateGLShaderObject(vertexShader, GL_VERTEX_SHADER);
		shaders[1] = CreateGLShaderObject(fragmentShader, GL_FRAGMENT_SHADER);
		for (unsigned int i = 0; i < numShaders; i++)
		{
			glAttachShader(shaderReference, shaders[i]); // Attach our shaders to the shader program
		}

		// Link our program object:
		glLinkProgram(shaderReference);
		if (!CheckShaderError(shaderReference, GL_LINK_STATUS, true))
		{
			glDeleteProgram(shaderReference);
			ReturnErrorShader(shaderName);
		}

		// Validate our program objects can execute with our current OpenGL state:
		glValidateProgram(shaderReference);
		if (!CheckShaderError(shaderReference, GL_VALIDATE_STATUS, true))
		{
			glDeleteProgram(shaderReference);
			ReturnErrorShader(shaderName);
		}

		// Delete the shader objects now that they've been linked into the program object:
		glDeleteShader(shaders[0]);
		glDeleteShader(shaders[1]);
		delete[] shaders;


		Shader* newShader = new Shader(shaderName, shaderReference);

		#if defined (DEBUG_SCENEMANAGER_SHADER_LOGGING)
			LOG("Finished creating shader \"" + shaderName + "\"");
		#endif

		return newShader;
	}

	Shader * BlazeEngine::Shader::ReturnErrorShader(string shaderName)
	{
		if (shaderName != CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName)
		{
			LOG_ERROR("Creating shader \"" + shaderName + "\" failed while loading shader files. Returning error shader");
			return CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName);
		}
		else
		{
			LOG_ERROR("Creating shader failed while loading shader files. Returning nullptr");
			return nullptr; // Worst case: We can't find the error shader. This will likely cause a crash if it ever occurs.
		}
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
			LOG_ERROR("LoadShaderFile failed: Could not open shader " + filepath);

			return "";
		}

		return output;
	}


	void Shader::LoadIncludes(string& shaderText)
	{
		#if defined(DEBUG_SHADER_SETUP_LOGGING)
			LOG("Processing shader #include directives");
			bool foundInclude = false;
		#endif

		const string INCLUDE_KEYWORD = "#include";

		int foundIndex = 0;
		while (foundIndex != string::npos && foundIndex < shaderText.length())
		{
			foundIndex = (int)shaderText.find(INCLUDE_KEYWORD, foundIndex + 1);
			if (foundIndex != string::npos)
			{
				// Check we're not on a commented line:
				int checkIndex = foundIndex;
				bool foundComment = false;
				while (checkIndex >= 0 && shaderText[checkIndex] != '\n')
				{
					if (shaderText[checkIndex] == '/' && checkIndex > 0 && shaderText[checkIndex - 1] == '/')
					{
						foundComment = true;
						break;
					}
					checkIndex--;
				}
				if (foundComment)
				{
					continue;
				}

				int endIndex = (int)shaderText.find("\n", foundIndex + 1);
				if (endIndex != string::npos)
				{
					int firstQuoteIndex, lastQuoteIndex;
					
					firstQuoteIndex = (int)shaderText.find("\"", foundIndex + 1);
					if (firstQuoteIndex != string::npos && firstQuoteIndex > 0 && firstQuoteIndex < endIndex)
					{
						lastQuoteIndex = (int)shaderText.find("\"", firstQuoteIndex + 1);
						if (lastQuoteIndex != string::npos && lastQuoteIndex > firstQuoteIndex && lastQuoteIndex < endIndex)
						{
							firstQuoteIndex++; // Move ahead 1 element from the first quotation mark

							string includeFileName = shaderText.substr(firstQuoteIndex, lastQuoteIndex - firstQuoteIndex);

							#if defined(DEBUG_SHADER_SETUP_LOGGING)
								string includeDirective = shaderText.substr(foundIndex, endIndex - foundIndex - 1);	// - 1 to move back from the index of the last "
								LOG("Found include directive \"" + includeDirective + "\". Attempting to load file \"" + includeFileName + "\"");
							#endif							

							string includeFile = LoadShaderFile(includeFileName);
							if (includeFile != "")
							{
								// Perform the insertion:
								string firstHalf = shaderText.substr(0, foundIndex);
								string secondHalf = shaderText.substr(endIndex + 1, shaderText.length() - 1);
								shaderText = firstHalf + includeFile + secondHalf;								

								#if defined(DEBUG_SHADER_SETUP_LOGGING)
									LOG("Successfully processed shader directive \"" + includeDirective + "\"");
									foundInclude = true;
								#endif	
							}
							else
							{
								LOG_ERROR("Could not find include file. Shader loading failed.");
								return;
							}
						}
					}
				}
			}							
		}

		#if defined(DEBUG_SHADER_SETUP_LOGGING)
			if (foundInclude)
			{

				#if defined(DEBUG_SHADER_PRINT_FINAL_SHADER)
					LOG("Final shader text:\n" + shaderText);
				#else
					LOG("Finished processing #include directives");
				#endif
			}
			else
			{
				LOG("No #include directives processed. Shader is unchanged");
			}
		#endif
	}


	GLuint Shader::CreateGLShaderObject(const string& shaderCode, GLenum shaderType)
	{
		GLuint shader = glCreateShader(shaderType);
		if (shader == 0)
		{
			LOG_ERROR("glCreateShader failed!");
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

			LOG_ERROR("CheckShaderError failed: " + errorAsString);

			return false;
		}
		else
		{
			return true;
		}
	}
}
