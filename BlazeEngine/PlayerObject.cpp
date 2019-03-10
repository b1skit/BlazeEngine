#include "PlayerObject.h"
#include "TimeManager.h"
#include "InputManager.h"

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


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
		GameObject::Update();

		// TEMP: Hard code some stuff:
		
		if (InputManager::GetInput(INPUT_BUTTON_LEFT))
		{
			this->transform.Translate(vec3(-1,0,0));
		}
		if (InputManager::GetInput(INPUT_BUTTON_RIGHT))
		{
			this->transform.Translate(vec3(1, 0, 0));
		}

		/*vec3 direction(0, 0, 0);

		direction += vec3 movementSpeed * (unsigned short)InputManager::GetInput(INPUT_BUTTON_FORWARD)*/

		//this->transform.Translate(vec3(1,0,0) * (float)( movementSpeed * TimeManager::DeltaTime()));
		
	}

	//// EventListener interface:
	//void PlayerObject::HandleEvent(EventInfo const* eventInfo) 
	//{
	//	switch (eventInfo)
	//	{

	//	}
	//}
}
