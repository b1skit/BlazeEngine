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
		Renderable(vector<Mesh*> viewMeshes)
		{
			this->viewMeshes = viewMeshes;

			/*bool isStatic = false;*/
		}
		
		/*~Renderable()
		{
		}*/


	protected:


	private:
		vector<Mesh*> viewMeshes;
		/*Mesh* boundsMesh;*/

		/*bool isStatic;*/
	};
}