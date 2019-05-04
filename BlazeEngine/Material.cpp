#include "Material.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"


#include <string>
using std::to_string;


namespace BlazeEngine
{
	Material::Material(string materialName, string shaderName)
	{
		this->name = materialName;

		this->shader = Shader::CreateShader(shaderName);

		textures = new Texture*[TEXTURE_COUNT];
		for (int i = 0; i < TEXTURE_COUNT; i++)
		{
			textures[i] = nullptr;
		}

		// Create samplers:
		glGenSamplers(TEXTURE_COUNT, &samplers[0]);
		for (int i = 0; i < TEXTURE_COUNT; i++)
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
		glDeleteSamplers(TEXTURE_COUNT, samplers);		
	}


	void Material::SetTexture(Texture* texture, TEXTURE_TYPE textureIndex)
	{
		if (textures[textureIndex])
		{
			LOG("Replacing material \"" + this->name + "\" texture #" + std::to_string(textureIndex));

			delete textures[textureIndex];
		}

		textures[textureIndex] = texture;
	}
}

