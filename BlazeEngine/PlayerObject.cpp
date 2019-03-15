#include "PlayerObject.h"
#include "TimeManager.h"
#include "InputManager.h"

#include "gtc/constants.hpp"

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	PlayerObject::PlayerObject() : GameObject::GameObject("Player Object")
	{
		this->playerCam.GetTransform()->SetParent(&this->transform);
	}

	//PlayerObject::~PlayerObject()
	//{
	//}

	void PlayerObject::Update()
	{
		GameObject::Update();

		// Handle first person view orientation: (pitch + yaw)
		if (InputManager::GetInputState(INPUT_MOUSE_AXIS))
		{
			vec3 yaw(0.0f, 0.0f, 0.0f);
			vec3 pitch(0.0f, 0.0f, 0.0f);

			yaw.y = (float)InputManager::GetMouseAxisInput(INPUT_MOUSE_X) * (float)TimeManager::DeltaTime();
			pitch.x = (float)InputManager::GetMouseAxisInput(INPUT_MOUSE_Y) * (float)TimeManager::DeltaTime();

			this->transform.Rotate(yaw);
			this->playerCam.GetTransform()->Rotate(pitch);
		}

		// Handle direction:
		vec3 direction = vec3(0.0f, 0.0f, 0.0f);

		if (InputManager::GetInputState(INPUT_BUTTON_FORWARD))
		{
			direction += this->transform.Forward();
		}
		if (InputManager::GetInputState(INPUT_BUTTON_BACKWARD))
		{
			direction += this->transform.Forward() * -1.0f;
		}
		if (InputManager::GetInputState(INPUT_BUTTON_LEFT))
		{
			direction += this->transform.Right() * -1.0f;
		}
		if (InputManager::GetInputState(INPUT_BUTTON_RIGHT))
		{
			direction += this->transform.Right();
		}
		if (InputManager::GetInputState(INPUT_BUTTON_UP))
		{
			direction += this->transform.Up();
		}
		if (InputManager::GetInputState(INPUT_BUTTON_DOWN))
		{
			direction += this->transform.Up() * -1.0f;
		}

		if (glm::length(direction) != 0.0f)
		{
			direction = glm::normalize(direction);
			direction *= (float)(movementSpeed * TimeManager::DeltaTime());

			this->transform.Translate(direction);
		}

		if (InputManager::GetInputState(INPUT_MOUSE_LEFT))
		{
			this->transform.SetPosition(vec3(0, 0, 0));
			this->transform.SetEulerRotation(vec3(0, 0, 0));
			this->playerCam.GetTransform()->SetEulerRotation(vec3(0, 0, 0));
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
