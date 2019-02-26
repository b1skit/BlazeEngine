#include "Material.h"

namespace BlazeEngine
{
	Material::Material(Shader* shader)
	{
		this->shader = shader;
	}

	Material::~Material()
	{

	}
}

