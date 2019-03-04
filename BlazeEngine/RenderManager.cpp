#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"

//#include <fstream>

#include "glm.hpp"
#include "GL/glew.h"
#include <GL/GL.h> // MUST follow glew.h...
#include <SDL_egl.h>
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

//using std::ifstream;
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
		/*SDL_Init(SDL_INIT_VIDEO);*/ // TO DO: IMPLEMENT PER-COMPONENT INITIALIZATION

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

		// Generate and bind names for our Vertex Array Objects:
		glGenVertexArrays(1, vertexArrayObjects); // Allocate the required number of vertex array objects (VAO's)
		glBindVertexArray(vertexArrayObjects[VERTEX_BUFFER_POSITION]); // Bind our VAO


		// Create and bind a vertex buffer to a buffer binding point allocated on the GPU suitable for vertices:
		glGenBuffers(1, vertexBufferObjects); // Create a vertex position buffer object and store its handle
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_POSITION]); // Bind our VBO to GL_ARRAY_BUFFER
		//glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES?!), vertexPositions, GL_STATIC_DRAW); // -> glBufferData: Allocates memory for the buffer and copy over the vertex data...
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Cleanup: Bind object 0 to GL_ARRAY_BUFFER to unbind vertexBufferObjects[VERTEX_BUFFER_POSITION]
		

		// Tell OpenGL how to interpet the data we've put on the GPU:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Define array of vertex attribute data: index, number of components (3 = 3 elements in vec3), type, should data be normalized?, stride, offset from start to 1st component
		glEnableVertexAttribArray(0); // Enable the vertex attribute


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));

	}

	void RenderManager::Shutdown()
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager shutting down..." });

		glDeleteVertexArrays(VERTEX_BUFFER_SIZE, vertexArrayObjects);
		glDeleteBuffers(VERTEX_BUFFER_SIZE, vertexBufferObjects);


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
		vector<Shader>* shaders = coreEngine->BlazeSceneManager->GetShaders(); // TO DO: Cache these off during Startup() ?

		for (int i = 0; i < renderables->size(); i++)
		{
			// Loop through every view mesh:
			int numViewMeshes = (int)renderables->at(i)->ViewMeshes()->size();
			for (int j = 0; j < numViewMeshes; j++)
			{
				Mesh* mesh = renderables->at(i)->ViewMeshes()->at(j);


				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_POSITION]); // Bind our VBO to GL_ARRAY_BUFFER

				// Copy vertex data into the buffer:
				glBufferData(GL_ARRAY_BUFFER, mesh->NumVerts() * sizeof(mesh->Vertices()[0]), mesh->Vertices(), GL_STATIC_DRAW); // Put data into the buffer
				// ^^ TODO: Define when/which obects should use GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW

				// Tell OpenGL how to interpet the data we've put on the GPU:
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Define array of vertex attribute data: index, number of components (3 = 3 elements in vec3), type, should data be normalized?, stride, offset from start to 1st component
				glEnableVertexAttribArray(0); // Enable the vertex attribute

				glBindBuffer(GL_ARRAY_BUFFER, 0); // Cleanup: Bind object 0 to GL_ARRAY_BUFFER to unbind vertexBufferObjects[VERTEX_BUFFER_POSITION]
				


				// Set the active shader: ...TO DO: Decide whether to use this directly, or via BindShader() ?
				glUseProgram(shaders->at(renderables->at(i)->ViewMeshes()->at(j)->GetMaterial()->GetShaderIndex()).ShaderReference()); // ...TO DO: Decide whether to use this directly, or via BindShader() ?

				// Bind the required VAO:
				glBindVertexArray(vertexArrayObjects[VERTEX_BUFFER_POSITION]);


				//glBufferData(GL_ARRAY_BUFFER, mesh->NumVerts() * sizeof(mesh->Vertices()[0]), mesh->Vertices(), GL_STATIC_DRAW); // Put data into the buffer


				// Draw!
				glDrawArrays(GL_TRIANGLES, 0, mesh->NumVerts()); // Type, start index, size



				/*renderables->at(i)->GetTransform();*/
			}
		}

		// Display the new frame:
		SDL_GL_SwapWindow(glWindow);


		// DEBUG:
		SDL_Delay((unsigned int)(1000.0 / 60.0));
	}

	//unsigned int RenderManager::GetShaderIndex(string shaderName)
	//{
	//	// Return the index if it's found, or load the shader and return a new index, or return the error shader otherwise
	//	int shaderIndex = -1;
	//	for (int i = 0; i < shaders.size(); i++)
	//	{
	//		if (shaders.at(i).Name() == shaderName)
	//		{
	//			return i; // We're done!
	//		}
	//	}

	//	// If we've made it this far, the shader was not found. Attempt to load it:
	//	shaderIndex = CreateShader(shaderName);

	//	if (shaderIndex >= 0)
	//	{
	//		return shaderIndex;
	//	}
	//	else // If all else fails, return the error shader:
	//	{
	//		return 0;
	//	}
	//}

	//int RenderManager::CreateShader(string shaderName)
	//{
	//	this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Loading shader: " + shaderName });

	//	GLuint shaderReference;
	//	unsigned int numShaders = 2; // TO DO : Allow loading of geometry shaders?
	//	GLuint* shaders = new GLuint[numShaders];

	//	// Create an empty shader program object, and get its reference:
	//	shaderReference = glCreateProgram();

	//	// Load the shader files:
	//	string vertexShader = LoadShaderFile(shaderName + ".vert");
	//	string fragmentShader = LoadShaderFile(shaderName + ".frag");
	//	if (vertexShader == "" || fragmentShader == "")
	//	{
	//		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "Creating shader failed while loading shader files"});
	//		return -1;
	//	}

	//	// Create shader objects and attach them to the program objects:
	//	shaders[0] = CreateGLShaderObject(vertexShader, GL_VERTEX_SHADER);
	//	shaders[1] = CreateGLShaderObject(fragmentShader, GL_FRAGMENT_SHADER);
	//	for (unsigned int i = 0; i < numShaders; i++)
	//	{
	//		glAttachShader(shaderReference, shaders[i]); // Attach our shaders to the shader program
	//	}

	//	// Associate our vertex attribute indexes with named variables:
	//	glBindAttribLocation(shaderReference, 0, "position"); // Bind attribute 0 to the "position" variable in the vertex shader

	//	// Link our program object:
	//	glLinkProgram(shaderReference);
	//	if (!CheckShaderError(shaderReference, GL_LINK_STATUS, true))
	//	{
	//		return -1;
	//	}

	//	// Validate our program objects can execute with our current OpenGL state:
	//	glValidateProgram(shaderReference);
	//	if (!CheckShaderError(shaderReference, GL_VALIDATE_STATUS, true))
	//	{
	//		return -1;
	//	}



	//	// Delete the shader objects now that they've been linked into the program object:
	//	glDeleteShader(shaders[0]);
	//	glDeleteShader(shaders[1]);
	//	delete shaders;


	//	/*Shader newShader(shaderName, shaderReference, numShaders, shaders);*/
	//	Shader newShader(shaderName, shaderReference);

	//	this->shaders.push_back(newShader);

	//	this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Successfully loaded " + shaderName });

	//	return (int)this->shaders.size() - 1;
	//}

	//string RenderManager::LoadShaderFile(const string& filename)
	//{
	//	// Assemble the full shader file path:
	//	string filepath = coreEngine->GetConfig()->shader.shaderDirectory + filename;

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
	//		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "LoadShaderFile failed: Could not open shader " + filepath });

	//		return "";
	//	}

	//	return output;
	//}

	//GLuint RenderManager::CreateGLShaderObject(const string& shaderCode, GLenum shaderType)
	//{
	//	GLuint shader = glCreateShader(shaderType);
	//	if (shader == 0)
	//	{
	//		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "glCreateShader failed!" });
	//	}

	//	const GLchar* shaderSourceStrings[1];
	//	GLint shaderSourceStringLengths[1];

	//	shaderSourceStrings[0] = shaderCode.c_str();
	//	shaderSourceStringLengths[0] = (GLint)shaderCode.length();

	//	glShaderSource(shader, 1, shaderSourceStrings, shaderSourceStringLengths);
	//	glCompileShader(shader);

	//	CheckShaderError(shader, GL_COMPILE_STATUS, false);

	//	return shader;
	//}

	//bool RenderManager::CheckShaderError(GLuint shader, GLuint flag, bool isProgram)
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

	//		string errorAsString(error);

	//		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "CheckShaderError failed: " + errorAsString});

	//		return false;
	//	}
	//	else
	//	{
	//		return true;
	//	}
	//}

	////void RenderManager::BindShader(int shaderIndex)
	////{
	////	glUseProgram(shaders.at(shaderIndex).ShaderReference());
	////}


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


