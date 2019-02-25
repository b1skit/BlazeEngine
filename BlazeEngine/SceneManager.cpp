#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"

//class CoreEngine;
//using BlazeEngine::CoreEngine;

namespace BlazeEngine
{
	//SceneManager::~SceneManager()
	//{
	//}

	SceneManager& SceneManager::Instance()
	{
		static SceneManager* instance = new SceneManager();
		return *instance;
	}

	void SceneManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Scene manager started!" });
	}

	void SceneManager::Shutdown()
	{

		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Scene manager shutting down..." });
	}

	void SceneManager::Update()
	{
		// TO DO: Update every game object
	}

	void SceneManager::HandleEvent(EventInfo const * eventInfo)
	{

		return;
	}

	void SceneManager::LoadScene(string scenePath)
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "Could not load " + scenePath + ", as SceneManager.LoadScene() is not implemented. Using a debug hard coded path for now!" });

		// Flush any existing scene objects: (NOTE: Any objects that access these must be shut down first!)
		//gameObjects.clear();
		renderables.clear();
		meshes.clear();
		/*materials.clear();*/
		shaders.clear();
		/*lights.clear();
		mainCamera = Camera();*/
		
		// Load our .FBX:
		// ...

		// Create a new game object for every item in the .FBX:
		// ...

		// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:

		// Allocate vertices: (Normally, we'll do this when loading a .FBX)
		Vertex* vertices = new Vertex[3];
		vertices[0] = Vertex(vec3(-0.5f, -0.5f, 0.0f));
		vertices[1] = Vertex(vec3(0.5f, -0.5f, 0.0f));
		vertices[2] = Vertex(vec3(0.0f, 0.5f, 0.0f));

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		Mesh mesh(vertices, 3);
		this->meshes.push_back(mesh);
		int meshIndex = (int)this->meshes.size() - 1; // Store the index so we can pass the address

		// Assemble a list of all meshes held by a Renderable:
		vector<Mesh*> viewMeshes;
		viewMeshes.push_back( &(this->meshes.at(meshIndex)) ); // Store the address of our mesh to pass to our Renderable
		Renderable testRenderable(viewMeshes);
		
		// Construct a GameObject:
		GameObject testObject("testObject", testRenderable);
		
		// Add test objects to scene:
		this->gameObjects.emplace_back(testObject);
		int gameObjectIndex = (int)this->gameObjects.size() - 1;
		
		this->renderables.emplace_back(gameObjects[gameObjectIndex].GetRenderable()); // Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
	}
}


