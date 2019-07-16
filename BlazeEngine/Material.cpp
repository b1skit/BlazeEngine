#include "Material.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Texture.h"
#include "BuildConfiguration.h"


#include <string>
using std::to_string;


namespace BlazeEngine
{
	// Static members:
	const string Material::TEXTURE_SAMPLER_NAMES[TEXTURE_COUNT] =
	{
		"albedo",			// TEXTURE_ALBEDO
		"normal",			// TEXTURE_NORMAL
		"RMAO",				// TEXTURE_EMISSIVE
		"emissive",			// TEXTURE_RMAO
	};

	const string Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_COUNT] = 
	{
		"GBuffer_Albedo",		// RENDER_TEXTURE_ALBEDO
		"GBuffer_WorldNormal",	// RENDER_TEXTURE_WORLD_NORMAL
		"GBuffer_RMAO",			// RENDER_TEXTURE_RMAO
		"GBuffer_Emissive",		// RENDER_TEXTURE_EMISSIVE

		"GBuffer_WorldPos",		// RENDER_TEXTURE_WORLD_POSITION
		"GBuffer_MatProp0"		// RENDER_TEXTURE_MATERIAL_PROPERTY_0

		"shadowDepth",			// RENDER_TEXTURE_DEPTH
	};

	const string Material::MATERIAL_PROPERTY_NAMES[MATERIAL_PROPERTY_COUNT] =
	{
		"matProperty0",
		//"matProperty1",
		//"matProperty2",
		//"matProperty3",
		//"matProperty4",
		//"matProperty5",
		//"matProperty6",
		//"matProperty7"
	};

	Material::Material(string materialName, string shaderName, TEXTURE_TYPE textureCount /*= TEXTURE_COUNT*/, bool isRenderMaterial /*= false*/)
	{
		this->name				= materialName;

		this->shader			= Shader::CreateShader(shaderName, &shaderKeywords);

		this->numTextures		= (int)textureCount;

		this->isRenderMaterial	= isRenderMaterial;

		textures = new Texture*[this->numTextures];
		for (int i = 0; i < this->numTextures; i++)
		{
			textures[i] = nullptr;
		}

		for (int i = 0; i < MATERIAL_PROPERTY_COUNT; i++)
		{
			properties[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}


	Texture*& Material::AccessTexture(TEXTURE_TYPE textureType)
	{
		return textures[textureType];
	}
}

