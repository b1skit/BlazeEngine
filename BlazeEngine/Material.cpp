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
	const string Material::SamplerNames[RENDER_TEXTURE_COUNT] = 
	{
		"shadowDepth",		// RENDER_TEXTURE_DEPTH

	};



	Material::Material(string materialName, string shaderName)
	{
		this->name = materialName;

		this->shader = Shader::CreateShader(shaderName);

		textures = new Texture*[TEXTURE_COUNT];
		for (int i = 0; i < (int)TEXTURE_COUNT; i++)
		{
			textures[i] = nullptr;
		}

		// Create samplers:
		samplers = new GLuint[(int)TEXTURE_COUNT];
		glGenSamplers((int)TEXTURE_COUNT, &samplers[0]);
		for (int i = 0; i < (int)TEXTURE_COUNT; i++)
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
			properties[i] = vec3(0.0f, 0.0f, 0.0f);
		}
	}


	Material::~Material()
	{
		glDeleteSamplers((int)TEXTURE_COUNT, samplers);

		delete[] samplers;
		samplers = nullptr;
	}


	Texture* Material::GetTexture(TEXTURE_TYPE textureIndex)
	{
		if (textureIndex < (int)TEXTURE_COUNT)
		{
			return textures[textureIndex];
		}
		else
		{
			LOG_ERROR("Cannot get texture #" + to_string(textureIndex) + " for material \"" + this->name + "\". Returning nullptr!");
			return nullptr;
		}		
	}

	void Material::SetTexture(Texture* texture, TEXTURE_TYPE textureIndex)
	{
		if (textureIndex < (int)TEXTURE_COUNT)
		{
			if (textures[textureIndex] != nullptr)
			{
				LOG("Replacing material \"" + this->name + "\" texture #" + std::to_string(textureIndex));

				delete textures[textureIndex];
				textures[textureIndex] = nullptr;
			}

			textures[textureIndex] = texture;

			LOG("Set texture #" + to_string(textureIndex) + " for material \"" + this->name + "\"");
		}
		else
		{
			LOG_ERROR("Failed to set texture #" + to_string(textureIndex) + " for material \"" + this->name + "\"");
			return;
		}
	}
}

