#include "Material.h"

namespace BlazeEngine
{
	Material::Material(unsigned int shaderIndex)
	{
		this->shaderIndex = shaderIndex;
	}

	Material::~Material()
	{
		if (albedo)
		{
			delete albedo;
		}
		if (normal)
		{
			delete normal;
		}
		if (roughness)
		{
			delete roughness;
		}
		if (ambientOcclusion)
		{
			delete ambientOcclusion;
		}
	}
}

