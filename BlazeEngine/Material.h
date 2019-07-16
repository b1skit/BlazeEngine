#pragma once

#include "Texture.h"
#include "Shader.h"

#include <vector>
#include <string>


#include "glm.hpp"

using glm::vec3;
using std::vector;
using std::string;


namespace BlazeEngine
{
	enum TEXTURE_TYPE
	{
		TEXTURE_ALBEDO			= 0,				// Contains transparency in the alpha channel
		TEXTURE_NORMAL			= 1,
		TEXTURE_RMAO			= 2,				// Packed Roughness, Metalic, AmbientOcclusion (RGB) + unused A
		TEXTURE_EMISSIVE		= 3,

		TEXTURE_COUNT			= 4,				// RESERVED: Number of Texture slots a material has
		RENDER_TEXTURE_0		= 4,				// RESERVED: Starting offset for binding RenderTextures (eg. In RenderManager::BindTextures())

		// Alternative RenderTexture names:
		RENDER_TEXTURE_ALBEDO			= 0,
		RENDER_TEXTURE_WORLD_NORMAL		= 1,
		RENDER_TEXTURE_RMAO				= 2,
		RENDER_TEXTURE_EMISSIVE			= 3,

		RENDER_TEXTURE_WORLD_POSITION		= 4,
		RENDER_TEXTURE_MATERIAL_PROPERTY_0	= 5,	// MATERIAL_PROPERTY_0

		RENDER_TEXTURE_DEPTH				= 6,	// Make this the last element

		RENDER_TEXTURE_COUNT				= 7
	}; // Note: If new enums are added, don't forget to update Material::RENDER_TEXTURE_SAMPLER_NAMES[] as well!


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
	}; // Note: If new enums are added, don't forget to update Material::MATERIAL_PROPERTY_NAMES[] as well!


	class Material
	{
	public:
		Material(string materialName, string shaderName, TEXTURE_TYPE textureCount = TEXTURE_COUNT, bool isRenderMaterial = false);

		void Destroy()
		{
			if (this->shader != nullptr)
			{
				this->shader->Destroy();
				delete this->shader;
			}
		}

		// TODO: Copy constructor, assignment operator

		// Getters/Setters:
		inline string const&	Name()									{ return name; }
		inline Shader*&			GetShader()								{ return shader; }
		inline vec4&			Property(MATERIAL_PROPERTY_INDEX index) { return properties[index]; }

		Texture*&				AccessTexture(TEXTURE_TYPE textureType);// { return textures[textureType]; }
		inline int const&		NumTextureSlots()						{ return numTextures; }

		void AddShaderKeyword(string const& newKeyword)
		{
			shaderKeywords.emplace_back(newKeyword);
		}

		vector<string> const&	ShaderKeywords() const					{ return shaderKeywords; }

		inline bool&			IsRenderMaterial()						{ return isRenderMaterial; }

		// RenderTexture sampler names:
		//-----------------------------
		const static string RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_COUNT];
		const static string TEXTURE_SAMPLER_NAMES[TEXTURE_COUNT];
		const static string MATERIAL_PROPERTY_NAMES[MATERIAL_PROPERTY_COUNT];
		
	protected:
		

	private:
		string		name;									// Must be unique: Identifies this material

		Shader*		shader		= nullptr;					// Deallocated up in SceneManager.Shutdown()
		Texture**	textures	= nullptr;					// Deallocated when object is destroyed in SceneManager.Shutdown()
		int			numTextures = 0;
		
		vec4		properties[MATERIAL_PROPERTY_COUNT];	// Generic material properties

		vector<string> shaderKeywords;

		bool isRenderMaterial	= false;
	};
}


