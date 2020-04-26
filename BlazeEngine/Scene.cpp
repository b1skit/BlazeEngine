#include "Scene.h"
#include "Light.h"
#include "Camera.h"
#include "Skybox.h"
#include "GameObject.h"
#include "Renderable.h"
#include "BuildConfiguration.h"


namespace BlazeEngine
{
	Scene::Scene(string sceneName)
	{
		this->sceneName = sceneName;

		gameObjects.reserve(GAMEOBJECTS_RESERVATION_AMT);
		renderables.reserve(RENDERABLES_RESERVATION_AMT);
		meshes.reserve(MESHES_RESERVATION_AMT);

		sceneCameras.reserve(CAMERA_TYPE_COUNT);
		for (int i = 0; i < CAMERA_TYPE_COUNT; i++)
		{
			sceneCameras.push_back(vector<Camera*>());
		}
		sceneCameras.at(CAMERA_TYPE_SHADOW).reserve(CAMERA_TYPE_SHADOW_ARRAY_SIZE);
		sceneCameras.at(CAMERA_TYPE_REFLECTION).reserve(CAMERA_TYPE_REFLECTION_ARRAY_SIZE);

		sceneCameras.at(CAMERA_TYPE_MAIN).reserve(1); // Only 1 main camera

		deferredLights.reserve(DEFERRED_LIGHTS_RESERVATION_AMT);
	}


	Scene::~Scene()
	{
		DeleteMeshes();

		for (int i = 0; i < (int)gameObjects.size(); i++)
		{
			if (gameObjects.at(i))
			{
				delete gameObjects.at(i);
				gameObjects.at(i) = nullptr;
			}
		}


		for (int i = 0; i < (int)deferredLights.size(); i++)
		{
			if (deferredLights.at(i) != nullptr)
			{
				deferredLights.at(i)->Destroy();
				delete deferredLights.at(i);
				deferredLights.at(i) = nullptr;
			}
		}
		deferredLights.clear();

		ClearCameras();

		if (skybox != nullptr)
		{
			delete skybox;
			skybox = nullptr;
		}
	}


	void Scene::InitMeshArray()
	{
		DeleteMeshes();
		meshes.reserve(MESHES_RESERVATION_AMT);
	}


	int Scene::AddMesh(Mesh* newMesh)
	{
		// Update scene (world) bounds to contain the new mesh:
		Bounds meshWorldBounds(newMesh->localBounds.GetTransformedBounds(newMesh->GetTransform().Model()));

		if (meshWorldBounds.xMin < sceneWorldBounds.xMin)
		{
			sceneWorldBounds.xMin = meshWorldBounds.xMin;
		}
		if (meshWorldBounds.xMax > sceneWorldBounds.xMax)
		{
			sceneWorldBounds.xMax = meshWorldBounds.xMax;
		}

		if (meshWorldBounds.yMin < sceneWorldBounds.yMin)
		{
			sceneWorldBounds.yMin = meshWorldBounds.yMin;
		}
		if (meshWorldBounds.yMax > sceneWorldBounds.yMax)
		{
			sceneWorldBounds.yMax = meshWorldBounds.yMax;
		}

		if (meshWorldBounds.zMin < sceneWorldBounds.zMin)
		{
			sceneWorldBounds.zMin = meshWorldBounds.zMin;
		}
		if (meshWorldBounds.zMax > sceneWorldBounds.zMax)
		{
			sceneWorldBounds.zMax = meshWorldBounds.zMax;
		}

		// Add the mesh to our array:
		int meshIndex = (int)meshes.size();
		meshes.push_back(newMesh);
		return meshIndex;

	}


	void Scene::DeleteMeshes()
	{
		for (int i = 0; i < (int)meshes.size(); i++)
		{
			if (meshes.at(i) != nullptr)
			{
				delete meshes[i];
				meshes.at(i) = nullptr;
			}
		}

		meshes.clear();
	}


	Mesh* Scene::GetMesh(int meshIndex)
	{
		if (meshIndex >= (int)meshes.size())
		{
			LOG_ERROR("Invalid mesh index received: " + to_string(meshIndex) + " > " + to_string((int)meshes.size()) + ". Returning nullptr");
			return nullptr;
		}

		return meshes.at(meshIndex);
	}


	void Scene::RegisterCamera(CAMERA_TYPE cameraType, Camera* newCamera)
	{
		if (newCamera != nullptr && (int)cameraType < (int)sceneCameras.size())
		{
			sceneCameras.at((int)cameraType).push_back(newCamera);

			LOG("Registered new camera \"" + newCamera->GetName() + "\"");
		}
		else
		{
			LOG_ERROR("Failed to register new camera!");
		}
	}


	vector<Camera*> const& Scene::GetCameras(CAMERA_TYPE cameraType)
	{
		return sceneCameras.at(cameraType);
	}


	void Scene::ClearCameras()
	{
		if (sceneCameras.empty())
		{
			return;
		}

		for (int i = 0; i < (int)sceneCameras.size(); i++)
		{
			for (int j = 0; j < (int)sceneCameras.at(i).size(); j++)
			{
				if (sceneCameras.at(i).at(j) != nullptr)
				{
					sceneCameras.at(i).at(j)->Destroy();
					delete sceneCameras.at(i).at(j);
					sceneCameras.at(i).at(j) = nullptr;
				}
			}
		}
	}

	void Scene::AddLight(Light* newLight)
	{
		switch (newLight->Type())
		{
			// Check if we've got any existing ambient or directional lights:
		case LIGHT_AMBIENT_COLOR:
		case LIGHT_AMBIENT_IBL:
		case LIGHT_DIRECTIONAL:
		{
			bool foundExisting = false;
			for (int currentLight = 0; currentLight < (int)deferredLights.size(); currentLight++)
			{
				if (deferredLights.at(currentLight)->Type() == newLight->Type())
				{
					foundExisting = true;
					LOG_ERROR("Found an existing light with type " + to_string((int)newLight->Type()) + ". New light will not be added");
					break;
				}
			}
			if (!foundExisting)
			{
				deferredLights.push_back(newLight);

				if (newLight->Type() == LIGHT_DIRECTIONAL)
				{
					this->keyLight = newLight;
				}
				else
				{
					this->ambientLight = newLight;
				}
			}

			break;
		}

		// Don't need to do anything special with other light types
		case LIGHT_POINT:
		case LIGHT_SPOT:
		case LIGHT_AREA:
		case LIGHT_TUBE:
		default:
			deferredLights.push_back(newLight);
			break;
		}
	}	
}