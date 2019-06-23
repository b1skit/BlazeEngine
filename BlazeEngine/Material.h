#pragma once

#include "Texture.h"

#include <vector>
#include <string>


#include "glm.hpp"

using glm::vec3;
using std::vector;
using std::string;


namespace BlazeEngine
{
	// Pre-declarations
	class Shader;
	class Material;

	enum TEXTURE_TYPE
	{
		TEXTURE_ALBEDO			= 0,				// Contains transparency in the alpha channel
		TEXTURE_NORMAL			= 1,
		TEXTURE_RMAO			= 2,				// Packed Roughness, Metalic, AmbientOcclusion (RGB) + unused A
		TEXTURE_EMISSIVE		= 3,

		TEXTURE_COUNT			= 4,				// Reserved: Number of texture slots a material has
		RENDER_TEXTURE_0		= 4,				// Reserved: Starting offset for binding RenderTextures (eg. In RenderManager::BindTextures())

		// Alternative names for render textures:
		RENDER_TEXTURE_ALBEDO	= 0,
		RENDER_TEXTURE_NORMAL	= 1,
		RENDER_TEXTURE_RMAO		= 2,
		RENDER_TEXTURE_EMSSIVE	= 3,
		RENDER_TEXTURE_DEPTH	= 4,

		RENDER_TEXTURE_COUNT	= 5	// Note: If new enums are added, don't forget to update Material::RENDER_TEXTURE_SAMPLER_NAMES[] as well!
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


	class Material
	{
	public:
		Material(string materialName, string shaderName, TEXTURE_TYPE textureCount = TEXTURE_COUNT);
		~Material();

		// TODO: Copy constructor, assignment operator

		// Getters/Setters:
		inline string const&	Name()									{ return name; }
		inline GLuint const&	Samplers(unsigned int textureType)		{ return samplers[textureType]; }
		inline Shader*			GetShader()								{ return shader; }
		inline vec3&			Property(MATERIAL_PROPERTY_INDEX index) { return properties[index]; }

		inline Texture*& AccessTexture(TEXTURE_TYPE textureType)		{ return textures[textureType]; }
		inline int const& NumTextures()									{ return numTextures; }

		void AddShaderKeyword(string const& newKeyword)
		{
			shaderKeywords.emplace_back(newKeyword);
		}

		// RenderTexture sampler names:
		const static string RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_COUNT];
		const static string TEXTURE_SAMPLER_NAMES[TEXTURE_COUNT];
		
	protected:
		

	private:
		string		name;									// Must be unique: Identifies this material

		Shader*		shader		= nullptr;					// Deallocated up in SceneManager.Shutdown()
		Texture**	textures	= nullptr;					// Deallocated when object is destroyed in SceneManager.Shutdown()
		int			numTextures = 0;

		GLuint*		samplers;								// Deallocated up in destructor
		
		vec3		properties[MATERIAL_PROPERTY_COUNT];	// Generic material properties

		vector<string> shaderKeywords;
	};
}


