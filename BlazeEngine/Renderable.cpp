#include "Renderable.h"

void BlazeEngine::Renderable::SetTransform(Transform * transform)
{
	this->transform = transform;
	for (unsigned int i = 0; i < (unsigned int)viewMeshes.size(); i++)
	{
		viewMeshes.at(i)->SetTransform(transform);
	}
}
