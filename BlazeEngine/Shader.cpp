// Shader object

#include "Shader.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"
#include "Material.h"


#include <fstream>
using std::ifstream;


namespace BlazeEngine
{
	// Static members:
	const string Shader::SHADER_KEYWORDS[SHADER_KEYWORD_COUNT] = 
	{
		"NO_ALBEDO_TEXTURE",
		"NO_NORMAL_TEXTURE",
		"NO_EMISSIVE_TEXTURE",
		"NO_RMAO_TEXTURE",
		"NO_COSINE_POWER",
	};


	Shader::Shader(const string shaderName, const GLuint shaderReference)
	{
		this->shaderName		= shaderName;
		this->shaderReference	= shaderReference;
	}


	Shader::Shader(const Shader& existingShader)
	{
		this->shaderName		= existingShader.shaderName;
		this->shaderReference	= existingShader.shaderReference;
	}


	void Shader::Destroy()
	{
		glDeleteProgram(this->shaderReference);
		this->shaderReference = 0;
	}


		void Shader::UploadUniform(GLchar const* uniformName, void const* value, UNIFORM_TYPE const& type, int count /*= 1*/)
	{
		GLint currentProgram;
		bool isBound = true;	// Track if the current shader is bound or not
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
				glUniformMatrix4fv(uniformID, count, GL_FALSE, (GLfloat const*)value);
				break;

			case UNIFORM_Matrix3fv:
				glUniformMatrix3fv(uniformID, count, GL_FALSE, (GLfloat const*)value);
				break;

			case UNIFORM_Vec3fv:
				glUniform3fv(uniformID, count, (GLfloat const*)value);
				break;

			case UNIFORM_Vec4fv:
				glUniform4fv(uniformID, count, (GLfloat const*)value);
				break;
			
			case UNIFORM_Float:
				glUniform1f(uniformID, *(GLfloat const*)value);
				break;

			case UNIFORM_Int:
				glUniform1i(uniformID, *(GLint const*)value);
				break;

			default:
				LOG_ERROR("Shader uniform upload failed: Recieved unimplemented uniform type");
			}
		}

		if (!isBound)
		{
			glUseProgram(currentProgram);
		}
	}

	void Shader::Bind(bool doBind)
	{
		if (doBind)
		{
			glUseProgram(this->shaderReference);
		}
		else
		{
			glUseProgram(0);
		}
	}


	// Static functions:
	//*******************

	Shader* Shader::CreateShader(string shaderFileName, vector<string> const*  shaderKeywords /*= nullptr*/)
	{
		LOG("\nCreating shader \"" + shaderFileName + "\"");

		// Create an empty shader program object, and get its reference:
		GLuint shaderReference = glCreateProgram();

		// Load the shader files:
		string vertexShader		= LoadShaderFile(shaderFileName + ".vert");
		string geometryShader	= LoadShaderFile(shaderFileName + ".geom");
		string fragmentShader	= LoadShaderFile(shaderFileName + ".frag");
		if (vertexShader == "" || fragmentShader == "")
		{
			glDeleteProgram(shaderReference);
			ReturnErrorShader(shaderFileName);
		}

		// Check if we're also loading a geometry shader:
		unsigned int numShaders = 3;
		bool hasGeometryShader	= true;

		int vertexShaderIndex	= 0;
		int geometryShaderIndex = 1;
		int fragmentShaderIndex = 2;

		if (geometryShader == "")
		{
			numShaders			= 2;
			hasGeometryShader	= false;

			geometryShaderIndex = -1;	// No geometry shader
			fragmentShaderIndex = 1;

			#if defined(DEBUG_SHADER_SETUP_LOGGING)
				LOG("No geometry shader found")
			#endif
		}
		GLuint* shaders			= new GLuint[numShaders];

		// Insert #defines:
		if (shaderKeywords != nullptr)
		{
			#if defined(DEBUG_SHADER_SETUP_LOGGING)
				LOG("Inserting defines into loaded vertex shader text")
			#endif
			InsertDefines(vertexShader, shaderKeywords);

			if (hasGeometryShader)
			{
				#if defined(DEBUG_SHADER_SETUP_LOGGING)
					LOG("Inserting defines into loaded geometry shader text")
				#endif
				InsertDefines(geometryShader, shaderKeywords);
			}

			#if defined(DEBUG_SHADER_SETUP_LOGGING)
				LOG("Inserting defines into loaded fragment shader text")
			#endif
			InsertDefines(fragmentShader, shaderKeywords);
		}

		// Process #include directives:
		#if defined(DEBUG_SHADER_SETUP_LOGGING)
			LOG("Inserting includes into loaded Vertex shader")
		#endif
		LoadIncludes(vertexShader);

		if (hasGeometryShader)
		{
			#if defined(DEBUG_SHADER_SETUP_LOGGING)
				LOG("Inserting includes into loaded Fragment shader")
			#endif
			LoadIncludes(geometryShader);
		}		

		#if defined(DEBUG_SHADER_SETUP_LOGGING)
			LOG("Inserting includes into loaded Fragment shader")
		#endif
		LoadIncludes(fragmentShader);


		// Create shader objects and attach them to the program objects:
		shaders[vertexShaderIndex]			= CreateGLShaderObject(vertexShader, GL_VERTEX_SHADER);

		if (hasGeometryShader)
		{
			shaders[geometryShaderIndex]	= CreateGLShaderObject(geometryShader, GL_GEOMETRY_SHADER);
		}

		shaders[fragmentShaderIndex]		= CreateGLShaderObject(fragmentShader, GL_FRAGMENT_SHADER);

		for (unsigned int i = 0; i < numShaders; i++)
		{
			glAttachShader(shaderReference, shaders[i]); // Attach our shaders to the shader program
		}

		Shader* newShader	= nullptr;
		bool shaderSuccess	= true;

		// Link our program object:
		glLinkProgram(shaderReference);
		if (!CheckShaderError(shaderReference, GL_LINK_STATUS, true))
		{
			glDeleteProgram(shaderReference);
			newShader		= ReturnErrorShader(shaderFileName);
			shaderSuccess	= false;
		}

		// Validate our program objects can execute with our current OpenGL state:
		glValidateProgram(shaderReference);
		if (shaderSuccess && !CheckShaderError(shaderReference, GL_VALIDATE_STATUS, true))
		{
			glDeleteProgram(shaderReference);
			newShader		= ReturnErrorShader(shaderFileName);
			shaderSuccess	= false;
		}

		// Delete the shader objects now that they've been linked into the program object:
		for (unsigned int i = 0; i < numShaders; i++)
		{
			glDeleteShader(shaders[i]);
		}
		delete[] shaders;
		shaders = nullptr;

		if (shaderSuccess)
		{
			newShader = new Shader(shaderFileName, shaderReference);

			// Initialize sampler locations:
			newShader->Bind(true);

			// Texture sampler locations. Note: These must align with the locations defined in Material.h
			for (int currentTexture = 0; currentTexture < TEXTURE_COUNT; currentTexture++)
			{
				GLint samplerLocation = glGetUniformLocation(newShader->ShaderReference(), Material::TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, (TEXTURE_TYPE)currentTexture);
				}
			}
			// RenderTexture sampler locations:
			for (int currentTexture = 0; currentTexture < RENDER_TEXTURE_COUNT; currentTexture++)
			{
				GLint samplerLocation = glGetUniformLocation(newShader->ShaderReference(), Material::RENDER_TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, (int)(RENDER_TEXTURE_0 + (TEXTURE_TYPE)currentTexture));
				}
			}

			// 2D shadow map textures sampler locations:
			for (int currentTexture = 0; currentTexture < DEPTH_TEXTURE_COUNT; currentTexture++)
			{
				GLint samplerLocation = glGetUniformLocation(newShader->ShaderReference(), Material::DEPTH_TEXTURE_SAMPLER_NAMES[currentTexture].c_str());
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, DEPTH_TEXTURE_0 + (TEXTURE_TYPE)currentTexture);
				}
			}

			// Cube map depth texture sampler locations
			for (int currentCubeMap = 0; currentCubeMap < CUBE_MAP_COUNT; currentCubeMap++)
			{
				GLint samplerLocation = glGetUniformLocation(newShader->ShaderReference(), Material::CUBE_MAP_TEXTURE_SAMPLER_NAMES[currentCubeMap].c_str());
				if (samplerLocation >= 0)
				{
					glUniform1i(samplerLocation, (TEXTURE_TYPE)(CUBE_MAP_0 + (currentCubeMap * CUBE_MAP_NUM_FACES)));
				}
			}

			// Cleanup:
			newShader->Bind(false);
		}		

		#if defined (DEBUG_SCENEMANAGER_SHADER_LOGGING)
			LOG("Finished creating shader \"" + shaderFileName + "\"");
		#endif

		return newShader;
	}

	Shader* BlazeEngine::Shader::ReturnErrorShader(string shaderName)
	{
		if (shaderName != CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("errorShaderName"))
		{
			LOG_ERROR("Creating shader \"" + shaderName + "\" failed while loading shader files. Returning error shader");
			return CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("errorShaderName"));
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
		string filepath = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("shaderDirectory") + filename;

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
			#if defined(DEBUG_SHADER_SETUP_LOGGING)
				LOG_WARNING("LoadShaderFile failed: Could not open shader " + filepath);
			#endif

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


	void Shader::InsertDefines(string& shaderText, vector<string> const* shaderKeywords)
	{
		if ((int)shaderText.length() <= 0 || shaderKeywords == nullptr || (int)shaderKeywords->size() <= 0)
		{
			return;
		}

		// Find the #version directive, and insert our keywords immediately after it

		int foundIndex = (int)shaderText.find("#version", 0);
		if (foundIndex == string::npos)
		{
			foundIndex = 0;
		}
		// Find the next newline character:
		int endLine = (int)shaderText.find("\n", foundIndex + 1);

		// Assemble our #define lines:
		const string DEFINE_KEYWORD = "#define ";
		string assembledKeywords = "";
		for (int currentKeyword = 0; currentKeyword < (int)shaderKeywords->size(); currentKeyword++)
		{
			string defineLine = DEFINE_KEYWORD + shaderKeywords->at(currentKeyword) + "\n";

			assembledKeywords += defineLine;
		}

		// Insert our #define lines:
		shaderText.insert(endLine + 1, assembledKeywords);		
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

		shaderSourceStrings[0]			= shaderCode.c_str();
		shaderSourceStringLengths[0]	= (GLint)shaderCode.length();

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

			LOG_ERROR("CheckShaderError() failed: " + errorAsString);

			return false;
		}
		else
		{
			return true;
		}
	}
}
