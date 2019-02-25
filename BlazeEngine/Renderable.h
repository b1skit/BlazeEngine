// Renderable interface: For objects drawn by the RenderManager 
#pragma once

#include <vector>
#include "Mesh.h"

using std::vector;

namespace BlazeEngine
{
	class Renderable
	{
	public:
		Renderable() {} // This should never be called, since we always need a view mesh...
		Renderable(vector<Mesh*> viewMeshes)
		{
			this->viewMeshes = viewMeshes;

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

		inline vector<Mesh*> const* ViewMeshes() const
		{
			return &viewMeshes;
		}

	protected:


	private:
		vector<Mesh*> viewMeshes;
		/*Mesh* boundsMesh;*/

		/*bool isStatic;*/
	};
}