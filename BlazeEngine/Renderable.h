// Renderable interface: For objects drawn by the RenderManager 
#pragma once


#include "Mesh.h"
#include "Transform.h"
#include <vector>

using std::vector;


namespace BlazeEngine
{
	class Renderable
	{
	public:
		Renderable() {}
		
		Renderable(vector<Mesh*> viewMeshes)
		{
			this->viewMeshes = viewMeshes;
		}

		~Renderable()
		{

		}

		// Getters/Setters:
		inline vector<Mesh*> const* ViewMeshes() const { return &viewMeshes; }

		inline Transform* GetTransform() const { return gameObjectTransform; }
		void SetTransform(Transform* transform);

		void AddViewMeshAsChild(Mesh* mesh);


	protected:


	private:
		vector<Mesh*> viewMeshes;					// Pointers to statically allocated Mesh objects held by the scene manager
		Transform* gameObjectTransform = nullptr;	// The SceneObject that owns this Renderable must set the transform

		/*Mesh* boundsMesh;*/

		/*bool isStatic;*/
	};
}