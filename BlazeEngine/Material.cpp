#include "Material.h"
#include "CoreEngine.h"

#include <string>
using std::to_string;


namespace BlazeEngine
{
	Material::Material(string name, unsigned int shaderIndex)
	{
		this->name = name;
		this->shaderIndex = shaderIndex;

		textures = new Texture*[TEXTURE_COUNT];
		for (int i = 0; i < TEXTURE_COUNT; i++)
		{
			textures[i] = nullptr;
		}

		// Create samplers:
		glGenSamplers(TEXTURE_COUNT, &samplers[0]); // TO DO: Use a differnt array to contain sampler objects...
		for (int i = 0; i < TEXTURE_COUNT; i++)
		{
			glBindSampler(i, samplers[i]);
			if (!glIsSampler(samplers[i]))
			{
				CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("Material could not create sampler #" + to_string(i)) });
			}
			glBindSampler(i, 0);
		}
	}

	Material::~Material()
	{
		if (textures)
		{
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				if (textures[i])
				{
					delete textures[i];
				}
			}
			delete[] textures;
		}

		glDeleteSamplers(TEXTURE_COUNT, samplers);		
	}

	void Material::SetTexture(Texture* texture, TEXTURE_TYPE textureIndex)
	{
		if (textures[textureIndex])
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Replacing material \"" + this->name + "\" texture #" + std::to_string(textureIndex)) });

			delete textures[textureIndex];
		}

		textures[textureIndex] = texture;
	}
}

