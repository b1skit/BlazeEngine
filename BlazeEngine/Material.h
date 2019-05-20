#pragma once

#include "Shader.h"
#include "Texture.h"

#include "glm.hpp"

using glm::vec3;


namespace BlazeEngine
{
	enum TEXTURE_TYPE
	{
		TEXTURE_ALBEDO,				// Contains transparency in the alpha channel
		TEXTURE_NORMAL,
		TEXTURE_RMAO,				// Packed Roughness, Metalic, AmbientOcclusion (RGB) + unused A
		TEXTURE_EMISSIVE,			

		TEXTURE_COUNT // Reserved: Number of textures a material can hold
	};

	enum MATERIAL_PROPERTY_INDEX
	{
		MATERIAL_PROPERTY_0,		// Shader's matProperty0
		//MATERIAL_PROPERTY_1,		// Shader's matProperty1
		//MATERIAL_PROPERTY_2,		// Shader's matProperty2
		//MATERIAL_PROPERTY_3,		// Shader's matProperty3
		//MATERIAL_PROPERTY_4,		// Shader's matProperty4
		//MATERIAL_PROPERTY_5,		// Shader's matProperty5
		//MATERIAL_PROPERTY_6,		// Shader's matProperty6
		//MATERIAL_PROPERTY_7,		// Shader's matProperty7

		MATERIAL_PROPERTY_COUNT		// Reserved: Number of properties a material can hold
	};

	// 

	class Material
	{
	public:
		Material(string materialName, string shaderName);
		~Material();

		// Getters/Setters:
		inline string const&	Name()									{ return name; }
		inline Texture*			GetTexture(TEXTURE_TYPE textureIndex)	{ return textures[textureIndex]; }
		inline GLuint const&	Samplers(unsigned int textureType)		{ return samplers[textureType]; }
		inline Shader*			GetShader()								{ return shader; }
		inline vec3&			Property(MATERIAL_PROPERTY_INDEX index) { return properties[index]; }

		void SetTexture(Texture* texture, TEXTURE_TYPE textureIndex);

		
	protected:


	private:
		string name;								// Must be unique: Identifies this material

		Shader* shader		= nullptr;				// Deallocated up in SceneManager.Shutdown()

		Texture** textures = nullptr;				// Pointers to textures held (and deallocated) by the scene manager

		GLuint samplers[TEXTURE_COUNT];

		vec3 properties[MATERIAL_PROPERTY_COUNT];	// Generic material properties
	};
}


