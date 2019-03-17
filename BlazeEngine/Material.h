#pragma once

#include "Shader.h"

namespace BlazeEngine
{
	class Material
	{
	public:
		Material() { shaderIndex = 0; }
		Material(unsigned int shaderIndex);
		~Material();

		// Getters/Setters:
		inline unsigned int GetShaderIndex() { return shaderIndex; }


	protected:


	private:
		unsigned int shaderIndex;
		//Texture* albedo = nullptr;
		//Texture* normal = nullptr;
		//Texture* roughness = nullptr; // Or metalness?
		//Texture* ambientOcclusion = nullptr;
	};
}


