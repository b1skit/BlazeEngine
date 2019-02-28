#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"

#include <fstream>

#include "glm.hpp"
#include "GL/glew.h"
#include <GL/GL.h> // MUST follow glew.h...
#include <SDL_egl.h>
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

using std::ifstream;
using glm::vec3;
using glm::vec4;


namespace BlazeEngine
{
	RenderManager::~RenderManager()
	{
		
	}

	RenderManager& RenderManager::Instance()
	{
		static RenderManager* instance = new RenderManager();
		return *instance;
	}

	void RenderManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager started!" });

		// Cache the relevant config data:
		this->xRes = coreEngine->GetConfig()->renderer.windowXRes;
		this->yRes = coreEngine->GetConfig()->renderer.windowYRes;
		this->windowTitle = coreEngine->GetConfig()->renderer.windowTitle;

		// Initialize SDL:
		/*SDL_Init(SDL_INIT_VIDEO);*/ // Currently doing a global init... 

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Create a window:
		glWindow = SDL_CreateWindow(
			coreEngine->GetConfig()->renderer.windowTitle.c_str(), 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			xRes, 
			yRes, 
			SDL_WINDOW_OPENGL
		);

		// Create an OpenGL context:
		glContext = SDL_GL_CreateContext(glWindow);
		
		// Initialize glew:
		GLenum glStatus = glewInit();
		if (glStatus != GLEW_OK)
		{
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, "Render manager start failed: glStatus not ok!" });
			return;
		}


		// ??
		glGenVertexArrays(1, &vertexArrayObject); // Size, target
		glBindVertexArray(vertexArrayObject);

		// Create and bind a vertex buffer:
		glGenBuffers(VERTEX_BUFFER_SIZE, vertexArrayBuffers); // Allocate buffer on the GPU
		glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[VERTEX_BUFFER_POSITION]); // Tell OpenGl to interpret buffer as an array



		// Tell OpenGL how to interpet the data we've put on the GPU:
		glEnableVertexAttribArray(0); // Treat data as an array
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Tell it how to read the array: attribute array, count (3 = 3 elements in vec3), data type, normalize?, space between steps(?), start offset

		glBindVertexArray(0); // Prevent further vertex array object operations affecting our vertex array object

		glBindVertexArray(vertexArrayObject);


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));

		// Shaders: MUST be initialized in the same order as our shader name enum!
		//Shader defaultShader(coreEngine->GetConfig()->shader.shaderDirectory + coreEngine->GetConfig()->shader.defaultShader);
		//shaders.push_back(defaultShader); 
		
		// Index 0
		CreateShader(coreEngine->GetConfig()->shader.defaultShader);
		// TO DO: Push error shader to index 1...


		
	}

	void RenderManager::Shutdown()
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager shutting down..." });

		glDeleteVertexArrays(1, &vertexArrayObject);

		// Detach and delete shaders:
		for (int i = 0; i < (int)shaders.size(); i++)
		{
			for (int j = 0; j < shaders.at(i).numShaders; j++)
			{
				glDetachShader(shaders.at(i).shaderProgram, shaders.at(i).shaders[i]);
				glDeleteShader(shaders.at(i).shaderProgram);
			}
			// Delete the shader program:
			glDeleteProgram(shaders.at(i).shaderProgram);
		}

		// Close our window:
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(glWindow);
		SDL_Quit();
	}

	void RenderManager::Update()
	{
		
	}


	void RenderManager::Render(double alpha)
	{
		// Loop through every renderable:
		vector<Renderable const*> const* renderables = coreEngine->BlazeSceneManager->GetRenderables();
		for (int i = 0; i < renderables->size(); i++)
		{
			// Loop through every mesh:
			int numRenderables = (int)renderables->size();
			for (int j = 0; j < numRenderables; j++)
			{
				Mesh* mesh = renderables->at(i)->ViewMeshes()->at(j);

				// ??
				//glGenVertexArrays(1, &vertexArrayObject); // Size, target
				//glBindVertexArray(vertexArrayObject);

				// Allocate a vertex buffer:
				//glGenBuffers(VERTEX_BUFFER_SIZE, vertexArrayBuffers); // Allocate buffer on the GPU
				//glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[VERTEX_BUFFER_POSITION]); // Tell OpenGl to interpret buffer as an array
				//^^^ Moved to startup

				// Copy vertex data into the buffer:
				glBufferData(GL_ARRAY_BUFFER, mesh->NumVerts() * sizeof(mesh->Vertices()[0]), mesh->Vertices(), GL_STATIC_DRAW); // Put data into the buffer
				// TODO: Define which obects that use GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW

				//// Tell OpenGL how to interpet the data we've put on the GPU:
				//glEnableVertexAttribArray(0); // Treat data as an array
				//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Tell it how to read the array: attribute array, count (3 = 3 elements in vec3), data type, normalize?, space between steps(?), start offset

				//glBindVertexArray(0); // Prevent further vertex array object operations affecting our vertex array object

				//glBindVertexArray(vertexArrayObject);

				glDrawArrays(GL_TRIANGLES, 0, mesh->NumVerts()); // Type, start index, size

				/*renderables->at(i)->GetTransform();*/
			}
		}

		//// Copy vertices to the currently bound buffer:
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // SHOULDN'T THIS BE sizeof(vertices) * num of verts OR vertices[0] ????????

		// Display the new frame:
		SDL_GL_SwapWindow(glWindow);


		// DEBUG:
		SDL_Delay((unsigned int)(1000.0 / 60.0));
	}

	unsigned int RenderManager::GetShaderIndex(string shaderName)
	{
		// TO DO: Implement this funciton
		// Return the index if it's found, or load the shader and return a new index otherwise

		int shaderIndex = -1;
		for (int i = 0; i < shaders.size(); i++)
		{
			if (shaders.at(i).Name() == shaderName)
			{
				shaderIndex = i;
				break;
			}
		}

		// If the shader was not found, attempt to load it:
		if (shaderIndex == -1)
		{

			// TO DO: Try and create a shader, and return the correct index:

			shaderIndex = CreateShader(shaderName);

			if (shaderIndex >= 0)
			{
				return shaderIndex;
			}
			else // If all else fails, return the error shader:
			{
				return 1; // TO DO: Make error shader 0, default shader 1 ??????????
			}
		}

		return shaderIndex;
	}

int RenderManager::CreateShader(string shaderName)
{
	this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "RenderManager created shader " + shaderName });

	// TEMP:
	Shader newShader(coreEngine->GetConfig()->shader.shaderDirectory + shaderName);


	// UPDATED FLOW

	// Shader vars:

	//string shaderName; // <- copy this
	GLuint shaderProgram;
	static const unsigned int NUM_SHADERS = 2; // <- numShaders
	//GLuint shaders[NUM_SHADERS];
	GLuint* shaders = new GLuint[NUM_SHADERS];

	
	shaderProgram = glCreateProgram();

	// TO DO: Handle failure in DoCreateShader() (returns "")
	shaders[0] = DoCreateShader(LoadShader(shaderName + ".vert"), GL_VERTEX_SHADER);
	shaders[1] = DoCreateShader(LoadShader(shaderName + ".frag"), GL_FRAGMENT_SHADER);

	for (int i = 0; i < NUM_SHADERS; i++)
	{
		glAttachShader(shaderProgram, shaders[i]); // Attach our shaders to the shader program
	}

	// 
	glBindAttribLocation(shaderProgram, 0, "position"); // Bind attribute 0 to the "position" variable in the vertex shader

	// Link:
	glLinkProgram(shaderProgram);
	CheckShaderError(shaderProgram, GL_LINK_STATUS, true);

	// Validate:
	glValidateProgram(shaderProgram);
	CheckShaderError(shaderProgram, GL_VALIDATE_STATUS, true);


	// Temp: Copy the values to the shader via public variabls
	// TO DO: Pass these via a constructor, or setters
	newShader.numShaders = NUM_SHADERS;
	newShader.shaders = shaders;
	newShader.shaderProgram = shaderProgram;


	// TODO: Return -1 if we fail somehow, otherwise return the index we pushed

	// TEMP:
	this->shaders.push_back(newShader);
	return (int)this->shaders.size() - 1;

}

string RenderManager::LoadShader(const string& filepath)
{
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
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "RenderManager.LoadShader() failed: Could not open shader " + string(filepath) });

		return "";
	}

	return output;
}

// TO DO: Handle failure!
GLuint RenderManager::DoCreateShader(const string& text, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "RenderManager glCreateShader failed!" });
	}

	const GLchar* shaderSourceStrings[1];
	GLint shaderSourceStringLengths[1];

	shaderSourceStrings[0] = text.c_str();
	shaderSourceStringLengths[0] = (GLint)text.length();

	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false);

	return shader;
}

bool RenderManager::CheckShaderError(GLuint shader, GLuint flag, bool isProgram)
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

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "RenderManager.CheckShaderError() failed: " + errorAsString});

		return false;
	}
	else
	{
		return true;
	}
}

//void RenderManager::BindShader()
//{
//	glUseProgram(shaderProgram);
//}



void RenderManager::ClearWindow(vec4 clearColor)
	{
		// Set the initial color in both buffers:
		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(glWindow);

		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT);
	}


}


