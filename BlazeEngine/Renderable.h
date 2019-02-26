// Renderable interface: For objects drawn by the RenderManager 
#pragma once

#include <vector>
#include "Mesh.h"
#include "Transform.h"

using std::vector;

namespace BlazeEngine
{
	class Renderable
	{
	public:
		Renderable() { transform = nullptr; } // This should never be called, since we always need a view mesh...
		
		Renderable(vector<Mesh*> viewMeshes)
		{
			this->viewMeshes = viewMeshes;
			this->transform = nullptr;

			/*bool isStatic = false;*/
		}

		// Copy constructor:
		Renderable(const Renderable& renderable)
		{
			this->viewMeshes = renderable.viewMeshes;

			/*this->isStatic = renderable.isStatic;*/
		}
		
		/*~Renderable()
		{
		}*/

		// Getters/Setters:
		inline vector<Mesh*> const* ViewMeshes() const { return &viewMeshes; }

		inline Transform* GetTransform() const { return transform; }
		inline void SetTransform(Transform* transform) { this->transform = transform; }


	protected:


	private:
		vector<Mesh*> viewMeshes;
		Transform* transform; // The SceneObject that owns this Renderable must set the transform

		/*Mesh* boundsMesh;*/

		/*bool isStatic;*/
	};
}