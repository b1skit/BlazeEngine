#include "PlayerObject.h"
#include "TimeManager.h"


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
	}

	void PlayerObject::Update()
	{
		/*BlazeEngine::TimeManager::GetDeltaTimeMs();*/
		
	}

}
