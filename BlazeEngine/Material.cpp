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
		"emissive",			// TEXTURE_RMAO
		"RMAO",				// TEXTURE_EMISSIVE
	};

	const string Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_COUNT] = 
	{
		"GBuffer_Albedo",	// RENDER_TEXTURE_ALBEDO
		"GBuffer_Normal",	// RENDER_TEXTURE_WORLD_NORMAL
		"GBuffer_RMAO",		// RENDER_TEXTURE_RMAO
		"GBuffer_Emissive",	// RENDER_TEXTURE_EMISSIVE
		"GBuffer_Position",	// RENDER_TEXTURE_WORLD_POSITION

		"shadowDepth",		// RENDER_TEXTURE_DEPTH
	};

	const string Material::MATERIAL_PROPERTY_NAMES[MATERIAL_PROPERTY_COUNT] =
	{
		"matProperty0",
	};

	Material::Material(string materialName, string shaderName, TEXTURE_TYPE textureCount /*= TEXTURE_COUNT*/)
	{
		this->name = materialName;

		this->shader = Shader::CreateShader(shaderName);

		this->numTextures = (int)textureCount;

		textures = new Texture*[this->numTextures];
		for (int i = 0; i < this->numTextures; i++)
		{
			textures[i] = nullptr;
		}

		// Create samplers:
		samplers = new GLuint[this->numTextures];
		glGenSamplers(this->numTextures, &samplers[0]);
		for (int i = 0; i < this->numTextures; i++)
		{
			glBindSampler(i, samplers[i]);
			if (!glIsSampler(samplers[i]))
			{
				LOG_ERROR("Material could not create sampler #" + to_string(i));
			}
			glBindSampler(i, 0);
		}

		for (int i = 0; i < MATERIAL_PROPERTY_COUNT; i++)
		{
			properties[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}


	Material::~Material()
	{
		if (this->samplers != nullptr)
		{
			glDeleteSamplers(this->numTextures, samplers);

			delete[] samplers;
			samplers = nullptr;
		}
	}
}

