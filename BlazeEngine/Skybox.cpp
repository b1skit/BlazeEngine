#include "Skybox.h"
#include "Material.h"
#include "Mesh.h"


namespace BlazeEngine
{
	Skybox::Skybox(Material* skyMaterial, Mesh* skyMesh)
	{
		this->skyMaterial	= skyMaterial;
		this->skyMesh		= skyMesh;
	}


	Skybox::~Skybox()
	{
		if (skyMaterial != nullptr)
		{
			delete skyMaterial;
			skyMaterial = nullptr;
		}

		if (skyMesh != nullptr)
		{
			delete skyMesh;
			skyMesh = nullptr;
		}
	}
}


