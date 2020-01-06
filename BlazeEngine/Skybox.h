#pragma once

#include <string>

using std::string;


namespace BlazeEngine
{
	class Material;
	class Mesh;

	class Skybox
	{
	public:
		Skybox(Material* skyMaterial, Mesh* skyMesh);
		Skybox(string sceneName);
		~Skybox();

		Material*	GetSkyMaterial()		{ return skyMaterial; }
		Mesh*		GetSkyMesh()			{ return skyMesh; }


	private:
		Material* skyMaterial	= nullptr;	// Deallocated in destructor
		Mesh* skyMesh			= nullptr;	// Deallocated in destructor
	};
}


