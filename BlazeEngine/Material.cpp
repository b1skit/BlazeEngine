#include "Material.h"
#include "CoreEngine.h"

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

