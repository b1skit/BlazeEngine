#pragma once

#include "Mesh.h"

#include <string>
#include <vector>

using std::string;
using std::vector;


// Initial allocation amounts
#define GAMEOBJECTS_RESERVATION_AMT				100		// TODO: Set these with more carefully selected values...
#define RENDERABLES_RESERVATION_AMT				100
#define MESHES_RESERVATION_AMT					100

#define DEFERRED_LIGHTS_RESERVATION_AMT			25

#define CAMERA_TYPE_SHADOW_ARRAY_SIZE			10
#define CAMERA_TYPE_REFLECTION_ARRAY_SIZE		10


namespace BlazeEngine
{
	// Predeclarations:
	class Light;
	class Camera;
	class Skybox;
	class GameObject;
	class Renderable;
	

	// Indexes for scene cameras used for different rendering roles
	// Note: Cameras are rendered in the order defined here
	enum CAMERA_TYPE 
	{
		CAMERA_TYPE_SHADOW,
		CAMERA_TYPE_REFLECTION,
		CAMERA_TYPE_MAIN,			// The primary scene camera

		CAMERA_TYPE_COUNT			// Reserved: The number of camera types
	};


	// Container for all scene data:
	struct Scene
	{
		Scene(string sceneName);
		~Scene();

		// Meshes:
		//--------
		// Allocate an empty mesh array. Clears any existing mesh array
		void	InitMeshArray();

		int		AddMesh(Mesh* newMesh);
		void	DeleteMeshes();
		Mesh*	GetMesh(int meshIndex);
		inline vector<Mesh*> const& GetMeshes() { return meshes; }

		// Cameras:
		//---------
		vector<Camera*> const&	GetCameras(CAMERA_TYPE cameraType);
		Camera*					GetMainCamera()		{ return sceneCameras[CAMERA_TYPE_MAIN].at(0); }
		void					RegisterCamera(CAMERA_TYPE cameraType, Camera* newCamera);
		void					ClearCameras();	// Destroys the scene's cameras

		void					AddLight(Light* newLight);

		// Scene object containers:
		//-------------------------
		vector<GameObject*> gameObjects;	// Pointers to dynamically allocated GameObjects
		vector<Renderable*> renderables;	// Pointers to statically allocated renderables held by GameObjects


		// Duplicate pointers to lights contained in deferredLights
		Light* ambientLight = nullptr;
		Light* keyLight		= nullptr;

		vector<Light*> const& GetDeferredLights() const		{ return deferredLights; }

		// Skybox object:
		Skybox* skybox		= nullptr;

		inline Bounds const& WorldSpaceSceneBounds() const	{ return sceneWorldBounds; }

		string GetSceneName() const							{ return this->sceneName; }

	private:
		vector<vector<Camera*>> sceneCameras;

		vector<Mesh*> meshes;				// Pointers to dynamically allocated Mesh objects

		Bounds sceneWorldBounds;

		// Lights:
		//--------
		vector<Light*> deferredLights;

		string sceneName;
	};
}


