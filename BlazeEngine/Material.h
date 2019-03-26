#pragma once

#include "Shader.h"
#include "Texture.h"

namespace BlazeEngine
{
	enum TEXTURE_TYPE
	{
		TEXTURE_ALBEDO,
		TEXTURE_NORMAL,
		TEXTURE_ROUGHNESS,			// Or metalness?
		TEXTURE_METALLIC,
		TEXTURE_AMBIENT_OCCLUSION,

		TEXTURE_COUNT // Reserved: Number of textures a material can hold
	};


	class Material
	{
	public:
		Material() { shaderIndex = 0; }
		Material(string name, unsigned int shaderIndex);
		~Material();

		// Getters/Setters:
		inline unsigned int ShaderIndex() { return shaderIndex; }
		inline string const& Name() { return name; }
		inline Texture* GetTexture(TEXTURE_TYPE textureIndex) { return textures[textureIndex]; }
		
		void SetTexture(Texture* texture, TEXTURE_TYPE textureIndex);

	protected:


	private:
		unsigned int shaderIndex;
		string name; // Must be unique: Identifies this material

		Texture** textures = nullptr;
	};
}


