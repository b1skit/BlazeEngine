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
		
	}

	SceneManager::~SceneManager()
	{
		
	}

	SceneManager& SceneManager::Instance()
	{
		static SceneManager* instance = new SceneManager();
		return *instance;
	}

	void SceneManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		// Initialize materials:
		if (materials)
		{
			for (unsigned int i = 0; i < MAX_MATERIALS; i++)
			{
				if (materials[i])
				{
					delete materials[i];
				}
			}
			delete[] materials;
		}
		materials = new Material*[MAX_MATERIALS];
		for (unsigned int i = 0; i < MAX_MATERIALS; i++)
		{
			materials[i] = nullptr;
		}
		currentMaterialCount = 0;


		// Initialize shaders:
		shaders.reserve(100);

		// Initialize our Shaders to match the order of the SHADER enum:
		CreateShader(coreEngine->GetConfig()->shader.errorShader);		// Index 0
		CreateShader(coreEngine->GetConfig()->shader.defaultShader);	// Index 1

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("Scene manager started!") });
	}

	void SceneManager::Shutdown()
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("Scene manager shutting down...") });

		for (int i = 0; i < (int)currentScene->meshes.size(); i++)
		{
			if (currentScene->meshes[i].Vertices())
			{
				delete currentScene->meshes[i].Vertices();
			}
			
			if (currentScene->meshes[i].Indices())
			{
				delete currentScene->meshes[i].Indices();
			}

			if (materials)
			{
				for (unsigned int i = 0; i < MAX_MATERIALS; i++)
				{
					if (materials[i])
					{
						delete materials[i];
					}
				}
				delete [] materials;
				currentMaterialCount = 0;
			}

			for (int i = 0; i < currentScene->gameObjects.size(); i++)
			{
				delete currentScene->gameObjects.at(i);
			}
			
			if (currentScene)
			{
				delete currentScene;
			}
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
		for (int i = 0; i < (int)currentScene->gameObjects.size(); i++)
		{
			currentScene->gameObjects.at(i)->Update();
		}
	}

	void SceneManager::HandleEvent(EventInfo const * eventInfo)
	{

		return;
	}

	void SceneManager::LoadScene(string scenePath)
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_DEBUG, this, new string("Could not load " + scenePath + ", as LoadScene() is not implemented. Using a debug hard coded path for now!") });

		if (currentScene)
		{
			coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_DEBUG, this, new string("WARNING: Current scene already exists. Debug deallocation is to just delete it!") });
			delete currentScene;
		}
		currentScene = new Scene();


		// Flush any existing scene objects: (NOTE: Any objects that access these must be shut down first!)
		// TO DO: Make sure we're deallocating everything before clearing the lists
		//gameObjects.clear();
		/*renderables.clear();*/
		/*meshes.clear();*/  // TO DO: delete meshes.Vertices()
		/*materials.clear();*/
		/*forwardLights.clear();
		mainCamera = Camera();*/
		
		// Load our .FBX:
		// ...

		// Create a new game object for every item in the .FBX:
		// ...
		// TO DO: Guard file loading flow with exception handling




		//// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:

		//// Allocate vertices: (Normally, we'll do this when loading a .FBX)
		//int numVerts = 4;
		//Vertex* vertices = new Vertex[numVerts];
		//vertices[0] = Vertex(vec3(-0.5f, -0.5f, -20.0f), vec3(0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));
		//vertices[1] = Vertex(vec3(0.5f, -0.5f, -20.0f), vec3(0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));
		//vertices[2] = Vertex(vec3(0.0f, 0.5f, -20.0f), vec3(0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));
		//vertices[3] = Vertex(vec3(0.0f, -1.0f, -20.0f), vec3(0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));

		//int numIndices = 6;
		//GLubyte* vertexIndices = new GLubyte[numIndices]
		//{
		//	0, 1, 2,
		//	0, 3, 1,
		//};

		//// Create a material and shader:
		//unsigned int shaderIndex = GetShaderIndex(coreEngine->GetConfig()->shader.defaultShader);
		//
		//Material material( shaderIndex );
		//this->materials.push_back(material);
		//int materialIndex = (int)this->materials.size() - 1;

		//// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		//Mesh mesh(vertices, numVerts, vertexIndices, numIndices, &(this->materials.at(materialIndex)));
		//this->meshes.push_back(mesh);
		//int meshIndex = (int)this->meshes.size() - 1; // Store the index so we can pass the address

		//// Assemble a list of all meshes held by a Renderable:
		//vector<Mesh*> viewMeshes;
		//viewMeshes.push_back( &(this->meshes.at(meshIndex)) ); // Store the address of our mesh to pass to our Renderable
		//Renderable testRenderable(viewMeshes);
		//
		//// Construct a GameObject:
		//GameObject* testObject = new GameObject("testObject", testRenderable);

		///*testObject.GetTransform()->LocalPosition() = vec3(1,2,3);*/
		//
		//// Add test objects to scene:
		//this->gameObjects.push_back(testObject);
		//int gameObjectIndex = (int)this->gameObjects.size() - 1;
		//
		//// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		//this->renderables.push_back(gameObjects[gameObjectIndex]->GetRenderable());

		//// 2nd test mesh:
		//Vertex* vertices2 = new Vertex[3];

		//vertices2[0] = Vertex(vec3(-1.0f, 0.5f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));
		//vertices2[1] = Vertex(vec3(-0.5f, 0.2f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));
		//vertices2[2] = Vertex(vec3(0.0f, 0.5f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f));

		//GLubyte* vertexIndices2 = new GLubyte[3]
		//{
		//	/*0, 1, 2*/
		//	2, 1, 0 // TEMP: REVERSED!!!!
		//};

		//// Create a material and shader:
		//int shaderIndex2 = GetShaderIndex(coreEngine->GetConfig()->shader.errorShader);

		//Material material2(shaderIndex2);
		//this->materials.push_back(material2);
		//int materialIndex2 = (int)this->materials.size() - 1;

		//// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		//Mesh mesh2(vertices2, 3,vertexIndices2, 3, &(this->materials.at(materialIndex2)));
		//this->meshes.push_back(mesh2);
		//int meshIndex2 = (int)this->meshes.size() - 1; // Store the index so we can pass the address

		//// Assemble a list of all meshes held by a Renderable:
		//vector<Mesh*> viewMeshes2;
		//viewMeshes2.push_back(&(this->meshes.at(meshIndex2))); // Store the address of our mesh to pass to our Renderable
		//Renderable testRenderable2(viewMeshes2);

		//// Construct a GameObject:
		//GameObject* testObject2 = new GameObject("testObject2", testRenderable2);

		//// Add test objects to scene:
		//this->gameObjects.push_back(testObject2); // nukes renderables[0].viewMeshes[0] ???????		
		//// Renderables is a POINTER to a renderable object, that isn't being copied correctly!!!

		//int gameObjectIndex2 = (int)this->gameObjects.size() - 1;

		//// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		//this->renderables.push_back(gameObjects[gameObjectIndex2]->GetRenderable());

	
		

		// Note: BlazeEngine uses a RHCS in all cases
		vec3 positions[8];
		// "Front" side:
		positions[0] = vec3(-1.0f, 1.0f, 1.0f);		
		positions[1] = vec3(-1.0f, -1.0f, 1.0f);
		positions[2] = vec3(1.0f, -1.0f, 1.0f);
		positions[3] = vec3(1.0f, 1.0f, 1.0f);
		// "Back" side:
		positions[4] = vec3(-1.0f, 1.0f, -1.0f);	
		positions[5] = vec3(-1.0f, -1.0f, -1.0f);
		positions[6] = vec3(1.0f, -1.0f, -1.0f);
		positions[7] = vec3(1.0f, 1.0f, -1.0f);
		
		vec3 normals[6]
		{
			vec3(0.0f, 0.0f, 1.0f),		// Front = 0
			vec3(0.0f, 0.0f, -1.0f),	// Back = 1
			vec3(-1.0f, 0.0f, 0.0f),	// Left = 2
			vec3(1.0f, 0.0f, 0.0f),		// Right = 3
			vec3(0.0f, 1.0f, 0.0f),		// Up = 4
			vec3(0.0f, -1.0f, 0.0f),	// Down = 5
		};

		vec4 colors[8]
		{
			vec4(0.0f, 0.0f, 0.0f, 1.0f),
			vec4(0.0f, 0.0f, 1.0f, 1.0f),
			vec4(0.0f, 1.0f, 0.0f, 1.0f),
			vec4(0.0f, 1.0f, 1.0f, 1.0f),
			vec4(1.0f, 0.0f, 0.0f, 1.0f),
			vec4(1.0f, 0.0f, 1.0f, 1.0f),
			vec4(1.0f, 1.0f, 0.0f, 1.0f),
			vec4(1.0f, 1.0f, 1.0f, 1.0f),
		};

		vec2 uvs[4]
		{
			vec2(0.0f, 0.0f),
			vec2(0.0f, 1.0f),
			vec2(1.0f, 0.0f),
			vec2(1.0f, 1.0f),
		};

		int numVerts = 24;
		Vertex* cubeVerts = new Vertex[numVerts]
		{
			// Front face:
			Vertex(positions[0], normals[0], colors[0], uvs[1]), // HINT: position index should = color index
			Vertex(positions[1], normals[0], colors[1], uvs[0]), // All UV's should be used once per face
			Vertex(positions[2], normals[0], colors[2], uvs[3]), //2
			Vertex(positions[3], normals[0], colors[3], uvs[2]), //3

			// Left face:
			Vertex(positions[4], normals[2], colors[4], uvs[1]), //4
			Vertex(positions[5], normals[2], colors[5], uvs[0]),
			Vertex(positions[1], normals[2], colors[1], uvs[2]),
			Vertex(positions[0], normals[2], colors[0], uvs[3]), //7

			// Right face:
			Vertex(positions[3], normals[3], colors[3], uvs[1]), //8
			Vertex(positions[2], normals[3], colors[2], uvs[0]),
			Vertex(positions[6], normals[3], colors[6], uvs[2]),
			Vertex(positions[7], normals[3], colors[7], uvs[3]), //11

			// Top face:
			Vertex(positions[4], normals[4], colors[4], uvs[1]), //12
			Vertex(positions[0], normals[4], colors[0], uvs[0]),
			Vertex(positions[3], normals[4], colors[3], uvs[2]),
			Vertex(positions[7], normals[4], colors[7], uvs[3]), //15

			// Bottom face:
			Vertex(positions[1], normals[5], colors[1], uvs[1]), //16
			Vertex(positions[5], normals[5], colors[5], uvs[0]),
			Vertex(positions[6], normals[5], colors[6], uvs[2]),
			Vertex(positions[2], normals[5], colors[2], uvs[3]), //19

			// Back face:
			Vertex(positions[7], normals[1], colors[7], uvs[1]), //20
			Vertex(positions[6], normals[1], colors[6], uvs[0]),
			Vertex(positions[5], normals[1], colors[5], uvs[2]),
			Vertex(positions[4], normals[1], colors[4], uvs[3]), //23
		};

		int numIndices = 36;
		GLubyte* cubeIndices = new GLubyte[numIndices] // 6 faces * 2 tris * 3 indices 
		{
			// Front face:
			0, 1, 3,
			1, 2, 3,

			// Left face:
			4, 5, 7,
			7, 5, 6,

			// Right face:
			8, 9, 11,
			9, 10, 11,

			// Top face:
			12, 13, 15,
			13, 14, 15,

			// Bottom face:
			16, 17, 19,
			17, 18, 19,

			// Back face:
			20, 21, 23,
			21, 22, 23,
		};

		// Create a material and shader:
		unsigned int shaderIndex = GetShaderIndex(coreEngine->GetConfig()->shader.defaultShader);

		Material* newMaterial = new Material(shaderIndex);
		unsigned int materialIndex = AddMaterial(newMaterial);

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		Mesh mesh(cubeVerts, numVerts, cubeIndices, numIndices, materialIndex);
		currentScene->meshes.push_back(mesh);
		int meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address

		// Assemble a list of all meshes held by a Renderable:
		vector<Mesh*> viewMeshes;
		viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
		Renderable testRenderable(viewMeshes);

		// Construct a GameObject:
		GameObject* testObject = new GameObject("cubeObject", testRenderable);
		

		// Add test objects to scene:
		currentScene->gameObjects.push_back(testObject);
		int gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());




		// Assemble material lists:
		AssembleMaterialMeshLists();



		// Set up lights:
		currentScene->ambientLight = vec3(0.5, 0.5, 0.5);
		currentScene->keyLight = Light(LIGHT_DIRECTIONAL, vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
		currentScene->keyLight.GetTransform().Rotate(vec3(3.14f/4.0f, 3.14f/8.0f, 0)); // Rotation in radians
		currentScene->keyLight.SetColor(vec4(0,1,0,1));
		currentScene->keyLight.SetIntensity(2.0f);


		// Set up a player object:
		PlayerObject* player = new PlayerObject();
		currentScene->gameObjects.push_back(player);
		currentScene->mainCamera = player->GetCamera();
		currentScene->mainCamera->Initialize(
			vec3(0, 0, 0),
			(float)coreEngine->GetConfig()->renderer.windowXRes / (float)coreEngine->GetConfig()->renderer.windowYRes,
			coreEngine->GetConfig()->viewCam.fieldOfView, 
			coreEngine->GetConfig()->viewCam.near, 
			coreEngine->GetConfig()->viewCam.far
			);
		
	}

	unsigned int SceneManager::AddMaterial(Material * newMaterial)
	{
		/*for (int i = 0; i < currentMaterialCount; i++)
		{
			
		}*/
		// TO DO: Check if a material exists, and return it if it does

		// Otherwise, add a new material:
		if (currentMaterialCount == MAX_MATERIALS)
		{
			coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, new string("Cannot add any new materials: Max materials have been added! Returning material at index 0") });
			return 0; // Error: Return first material
		}

		materials[currentMaterialCount] = newMaterial;
		unsigned int newIndex = currentMaterialCount;
		currentMaterialCount++;

		return newIndex; // Return the previous index
	}






	void SceneManager::AssembleMaterialMeshLists()
	{
		materialMeshLists.clear();
		materialMeshLists.resize(MAX_MATERIALS);
		for (int i = 0; i < (int)currentScene->renderables.size(); i++)
		{
			for (int j = 0; j < (int)currentScene->renderables.at(i)->ViewMeshes()->size(); j++)
			{
				unsigned int materialIndex = currentScene->renderables.at(i)->ViewMeshes()->at(j)->GetMaterialIndex();
				Mesh* viewMesh = currentScene->renderables.at(i)->ViewMeshes()->at(j);
				materialMeshLists.at(materialIndex).emplace_back(viewMesh);
			}
		}

		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("Finished assembling material mesh lists") });
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
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("Loading shader: " + shaderName) });

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
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, new string("Creating shader failed while loading shader files") });
			return -1;
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


		Shader newShader(shaderName, shaderReference);

		this->shaders.push_back(newShader);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("Successfully loaded " + shaderName) });

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
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, new string("LoadShaderFile failed: Could not open shader " + filepath) });

			return "";
		}

		return output;
	}

	GLuint SceneManager::CreateGLShaderObject(const string& shaderCode, GLenum shaderType)
	{
		GLuint shader = glCreateShader(shaderType);
		if (shader == 0)
		{
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, new string("glCreateShader failed!") });
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

			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, new string("CheckShaderError failed: " + errorAsString) });

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


