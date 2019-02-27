#pragma once

#include "Shader.h"

namespace BlazeEngine
{
	class Material
	{
	public:
		Material(unsigned int shaderIndex);
		~Material();

		// Getters/Setters:
		inline unsigned int GetShaderIndex() { return shaderIndex; }


	protected:


	private:
		unsigned int shaderIndex;
		//Texture* albedo;
		//Texture* normal;
		//Texture* roughness; // Or metalness?
		//Texture* ambientOcclusion;
	};
}


