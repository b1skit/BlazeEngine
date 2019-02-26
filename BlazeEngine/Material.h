#pragma once

#include "Shader.h"

namespace BlazeEngine
{
	class Material
	{
	public:
		Material(Shader* shader);
		~Material();

		// Getters/Setters:
		inline Shader* GetShader() { return shader; }


	protected:


	private:
		Shader* shader;
		//Texture* albedo;
		//Texture* normal;
		//Texture* roughness; // Or metalness?
		//Texture* ambientOcclusion;
	};
}


