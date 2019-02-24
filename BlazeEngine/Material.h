#pragma once

#include "Shader.h"

namespace BlazeEngine
{
	class Material
	{
	public:
		Material();
		~Material();


	protected:


	private:
		Shader* shader;
		//Texture* albedo;
		//Texture* normal;
		//Texture* roughness; // Or metalness?
		//Texture* ambientOcclusion;
	};
}


