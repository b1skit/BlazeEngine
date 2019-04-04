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
		Material(string materialName, string shaderName);
		~Material();

		// Getters/Setters:
		inline string const&	Name()									{ return name; }
		inline Texture*			GetTexture(TEXTURE_TYPE textureIndex)	{ return textures[textureIndex]; }
		inline GLuint const&	Samplers(unsigned int textureType)		{ return samplers[textureType]; }
		inline Shader const*	GetShader()								{ return shader; }

		void SetTexture(Texture* texture, TEXTURE_TYPE textureIndex);

		

	protected:


	private:
		Shader* shader = nullptr;		// Deallocated  up in SceneManager.Shutdown()

		string name; // Must be unique: Identifies this material

		Texture** textures = nullptr;

		GLuint samplers[TEXTURE_COUNT];
	};
}


