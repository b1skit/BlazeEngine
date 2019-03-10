// Renderable interface: For objects drawn by the RenderManager 
#pragma once

#include <vector>
#include "Mesh.h"
#include "Transform.h"

using std::vector;

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;



namespace BlazeEngine
{
	class Renderable
	{
	public:
		Renderable() { transform = nullptr; }
		
		Renderable(vector<Mesh*> viewMeshes)
		{
			this->viewMeshes = viewMeshes;
			this->transform = transform;

			/*bool isStatic = false;*/
		}

		// Copy constructor:
		Renderable(const Renderable& renderable)
		{
			this->viewMeshes = renderable.viewMeshes;
			this->transform = renderable.transform;
			

			/*this->isStatic = renderable.isStatic;*/
		}

		// Assignment operator:
		/*Renderable& operator=(const Renderable& rhs) = default;*/
		Renderable& operator=(const Renderable& rhs)
		{
			this->viewMeshes = rhs.viewMeshes;
			this->transform = rhs.transform;

			return *this;
		}
		

		~Renderable()
		{

		}

		// Getters/Setters:
		inline vector<Mesh*> const* ViewMeshes() const { return &viewMeshes; }

		inline Transform* GetTransform() const { return transform; }
		inline void SetTransform(Transform* transform) { this->transform = transform; }


	protected:


	private:
		vector<Mesh*> viewMeshes;	// Pointers to statically allocated Mesh objects held by the scene manager
		Transform* transform;		// The SceneObject that owns this Renderable must set the transform

		/*Mesh* boundsMesh;*/

		/*bool isStatic;*/
	};
}