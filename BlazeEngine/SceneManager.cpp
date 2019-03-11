#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"

#include <fstream>
using std::ifstream;

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	SceneManager::SceneManager() : EngineComponent("SceneManager")
	{
		// TODO: Set these with meaningful values...
		gameObjects.reserve(100);
		renderables.reserve(100);
		meshes.reserve(100);
		materials.reserve(100);
		shaders.reserve(100);
	}

	SceneManager::~SceneManager()
	{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			delete gameObjects.at(i);
		}
	}

	SceneManager& SceneManager::Instance()
	{
		static SceneManager* instance = new SceneManager();
		return *instance;
	}

	void SceneManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		// Initialize our Shaders to match the order of the SHADER enum:
		CreateShader(coreEngine->GetConfig()->shader.errorShader);		// Index 0
		CreateShader(coreEngine->GetConfig()->shader.defaultShader);	// Index 1

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Scene manager started!" });
	}

	void SceneManager::Shutdown()
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Scene manager shutting down..." });

		for (int i = 0; i < (int)meshes.size(); i++)
		{
			delete meshes[i].Vertices();
		}


		// Detach and delete shaders:
		for (int i = 0; i < (int)shaders.size(); i++)
		{
			// Delete the shader program:
			glDeleteProgram(shaders.at(i).ShaderReference());
		}
	}

	void SceneManager::Update()
	{
		for (int i = 0; i < (int)gameObjects.size(); i++)
		{
			gameObjects.at(i)->Update();
		}
	}

	void SceneManager::HandleEvent(EventInfo const * eventInfo)
	{

		return;
	}

	void SceneManager::LoadScene(string scenePath)
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "Could not load " + scenePath + ", as SceneManager.LoadScene() is not implemented. Using a debug hard coded path for now!" });

		// Flush any existing scene objects: (NOTE: Any objects that access these must be shut down first!)
		// TO DO: Make sure we're deallocating everything before clearing the lists
		//gameObjects.clear();
		/*renderables.clear();*/
		/*meshes.clear();*/  // TO DO: delete meshes.Vertices()
		/*materials.clear();*/
		/*lights.clear();
		mainCamera = Camera();*/
		
		// Load our .FBX:
		// ...

		// Create a new game object for every item in the .FBX:
		// ...

		// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:

		// Allocate vertices: (Normally, we'll do this when loading a .FBX)
		Vertex* vertices = new Vertex[3];
		vertices[0] = Vertex(vec3(-0.5f, -0.5f, -10.0f));
		vertices[1] = Vertex(vec3(0.5f, -0.5f, -10.0f));
		vertices[2] = Vertex(vec3(0.0f, 0.5f, -10.0f));

		// Create a material and shader:
		unsigned int shaderIndex = GetShaderIndex(coreEngine->GetConfig()->shader.defaultShader);
		
		Material material( shaderIndex );
		this->materials.push_back(material);
		int materialIndex = (int)this->materials.size() - 1;

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		Mesh mesh(vertices, 3, &(this->materials.at(materialIndex)));
		this->meshes.push_back(mesh);
		int meshIndex = (int)this->meshes.size() - 1; // Store the index so we can pass the address

		// Assemble a list of all meshes held by a Renderable:
		vector<Mesh*> viewMeshes;
		viewMeshes.push_back( &(this->meshes.at(meshIndex)) ); // Store the address of our mesh to pass to our Renderable
		Renderable testRenderable(viewMeshes);
		
		// Construct a GameObject:
		GameObject* testObject = new GameObject("testObject", testRenderable);

		/*testObject.GetTransform()->LocalPosition() = vec3(1,2,3);*/
		
		// Add test objects to scene:
		this->gameObjects.push_back(testObject);
		int gameObjectIndex = (int)this->gameObjects.size() - 1;
		
		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		this->renderables.push_back(gameObjects[gameObjectIndex]->GetRenderable());

		// 2nd test mesh:
		Vertex* vertices2 = new Vertex[3];

		vertices2[0] = Vertex(vec3(-1.0f, 0.5f, -10.0f));
		vertices2[1] = Vertex(vec3(-0.5f, 0.2f, -10.0f));
		vertices2[2] = Vertex(vec3(0.0f, 0.5f, -10.0f));

		// Create a material and shader:
		int shaderIndex2 = GetShaderIndex(coreEngine->GetConfig()->shader.errorShader);

		Material material2(shaderIndex2);
		this->materials.push_back(material2);
		int materialIndex2 = (int)this->materials.size() - 1;

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		Mesh mesh2(vertices2, 3, &(this->materials.at(materialIndex2)));
		this->meshes.push_back(mesh2);
		int meshIndex2 = (int)this->meshes.size() - 1; // Store the index so we can pass the address

		// Assemble a list of all meshes held by a Renderable:
		vector<Mesh*> viewMeshes2;
		viewMeshes2.push_back(&(this->meshes.at(meshIndex2))); // Store the address of our mesh to pass to our Renderable
		Renderable testRenderable2(viewMeshes2);

		// Construct a GameObject:
		GameObject* testObject2 = new GameObject("testObject2", testRenderable2);

		// Add test objects to scene:
		this->gameObjects.push_back(testObject2); // nukes renderables[0].viewMeshes[0] ???????		
		// Renderables is a POINTER to a renderable object, that isn't being copied correctly!!!

		int gameObjectIndex2 = (int)this->gameObjects.size() - 1;

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		this->renderables.push_back(gameObjects[gameObjectIndex2]->GetRenderable());

		//Vertex* cubeVerts = new Vertex[12];
		//cubeVerts[0] = Vertex(vec3(-1.0f, 1.0f, 1.0f));
		//cubeVerts[1] = Vertex(vec3(-1.0f, -1.0f, 1.0f));
		//cubeVerts[2] = Vertex(vec3(1.0f, -1.0f, 1.0f));
		//cubeVerts[3] = Vertex(vec3(1.0f, 1.0f, 1.0f));
		//cubeVerts[4] = Vertex(vec3(-1.0f, -1.0f, -1.0f));
		//cubeVerts[5] = Vertex(vec3(1.0f, -1.0f, -1.0f));
		//cubeVerts[6] = Vertex(vec3(1.0f, 1.0f, -1.0f));
		//cubeVerts[7] = Vertex(vec3(-1.0f, 1.0f, -1.0f));

		/*1
		0,1,3
		2
		3,1,2
		3
		7,4,0
		4
		0,4,1
		5
		3,2,5
		6
		3,5,6
		7
		7,0,3
		8
		7,3,6
		9
		1,4,2
		10
		2,4,5
		11
		7,6,4
		12
		6,5,4
		*/



		// Set up a player object:
		PlayerObject* player = new PlayerObject();
		gameObjects.push_back(player);
		this->mainCamera = player->GetCamera();
		mainCamera->Initialize(
			vec3(0, 0, 0),
			(float)coreEngine->GetConfig()->renderer.windowXRes / (float)coreEngine->GetConfig()->renderer.windowYRes,
			coreEngine->GetConfig()->viewCam.fieldOfView, 
			coreEngine->GetConfig()->viewCam.near, 
			coreEngine->GetConfig()->viewCam.far
			);
		
	}






	unsigned int SceneManager::GetShaderIndex(string shaderName)
	{
		// Return the index if it's found, or load the shader and return a new index, or return the error shader otherwise
		int shaderIndex = -1;
		for (int i = 0; i < shaders.size(); i++)
		{
			if (shaders.at(i).Name() == shaderName)
			{
				return i; // We're done!
			}
		}

		// If we've made it this far, the shader was not found. Attempt to load it:
		shaderIndex = CreateShader(shaderName);

		if (shaderIndex >= 0)
		{
			return shaderIndex;
		}
		else // If all else fails, return the error shader:
		{
			return 0;
		}
	}

	int SceneManager::CreateShader(string shaderName)
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Loading shader: " + shaderName });

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
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "Creating shader failed while loading shader files" });
			return -1;
		}

		// Create shader objects and attach them to the program objects:
		shaders[0] = CreateGLShaderObject(vertexShader, GL_VERTEX_SHADER);
		shaders[1] = CreateGLShaderObject(fragmentShader, GL_FRAGMENT_SHADER);
		for (unsigned int i = 0; i < numShaders; i++)
		{
			glAttachShader(shaderReference, shaders[i]); // Attach our shaders to the shader program
		}

		// Associate our vertex attribute indexes with named variables:
		glBindAttribLocation(shaderReference, 0, "in_position"); // Bind attribute 0 as "position" in the vertex shader
		glBindAttribLocation(shaderReference, 1, "in_mvp"); // Bind attribute 0 as "position" in the vertex shader
		// TO DO: Bind other attributes (color, uv, etc)...
		// TO DO: Replace indexes with an enum

		// Link our program object:
		glLinkProgram(shaderReference);
		if (!CheckShaderError(shaderReference, GL_LINK_STATUS, true))
		{
			return -1;
		}

		// Validate our program objects can execute with our current OpenGL state:
		glValidateProgram(shaderReference);
		if (!CheckShaderError(shaderReference, GL_VALIDATE_STATUS, true))
		{
			return -1;
		}



		// Delete the shader objects now that they've been linked into the program object:
		glDeleteShader(shaders[0]);
		glDeleteShader(shaders[1]);
		delete shaders;


		/*Shader newShader(shaderName, shaderReference, numShaders, shaders);*/
		Shader newShader(shaderName, shaderReference);

		this->shaders.push_back(newShader);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Successfully loaded " + shaderName });

		return (int)this->shaders.size() - 1;
	}

	string SceneManager::LoadShaderFile(const string& filename)
	{
		// Assemble the full shader file path:
		string filepath = coreEngine->GetConfig()->shader.shaderDirectory + filename;

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
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "LoadShaderFile failed: Could not open shader " + filepath });

			return "";
		}

		return output;
	}

	GLuint SceneManager::CreateGLShaderObject(const string& shaderCode, GLenum shaderType)
	{
		GLuint shader = glCreateShader(shaderType);
		if (shader == 0)
		{
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "glCreateShader failed!" });
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

	bool SceneManager::CheckShaderError(GLuint shader, GLuint flag, bool isProgram)
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

			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "CheckShaderError failed: " + errorAsString });

			return false;
		}
		else
		{
			return true;
		}
	}

	//void RenderManager::BindShader(int shaderIndex)
	//{
	//	glUseProgram(shaders.at(shaderIndex).ShaderReference());
	//}
}


