#include "PlayerObject.h"


namespace BlazeEngine
{
	//PlayerObject::PlayerObject()
	//{
	//}


	//PlayerObject::~PlayerObject()
	//{
	//}

	PlayerObject::PlayerObject() : GameObject::GameObject("Player Object")
	{
		this->playerCam.GetTransform()->SetParent(&this->transform);

		this->transform.Translate(vec3(-2,0, -8)); // DEBUG!!
		// This isn't moving in the right way...
	}

}
