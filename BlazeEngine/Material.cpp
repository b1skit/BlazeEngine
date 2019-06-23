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
}

