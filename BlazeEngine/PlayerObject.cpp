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
		vec3 direction = vec3(0.0f, 0.0f, 0.0f);
		bool readInput = false;

		if (InputManager::GetInput(INPUT_BUTTON_FORWARD))
		{
			direction += this->transform.Forward();
			readInput = true;
		}
		if (InputManager::GetInput(INPUT_BUTTON_BACKWARD))
		{
			direction += this->transform.Forward() * -1.0f;
			readInput = true;
		}
		if (InputManager::GetInput(INPUT_BUTTON_LEFT))
		{
			direction += this->transform.Right() * -1.0f;
			readInput = true;
		}
		if (InputManager::GetInput(INPUT_BUTTON_RIGHT))
		{
			direction += this->transform.Right();
			readInput = true;
		}
		if (InputManager::GetInput(INPUT_BUTTON_UP))
		{
			direction += this->transform.Up();
			readInput = true;
		}
		if (InputManager::GetInput(INPUT_BUTTON_DOWN))
		{
			direction += this->transform.Up() * -1.0f;
			readInput = true;
		}

		if (readInput)
		{
			direction = glm::normalize(direction);
			direction *= (float)(movementSpeed * TimeManager::DeltaTime());

			this->transform.Translate(direction);
		}
	}

	//// EventListener interface:
	//void PlayerObject::HandleEvent(EventInfo const* eventInfo) 
	//{
	//	switch (eventInfo)
	//	{

	//	}
	//}
}
