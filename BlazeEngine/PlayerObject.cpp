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

		// Handle view orientation: (pitch + yaw)
		if (InputManager::GetInputState(INPUT_MOUSE_AXIS))
		{
			// neg pitchup, pos pitch down
			vec3 rotation = vec3(0.0f, 0.0f, 0.0f);
			float yAxis = (float)InputManager::GetMouseAxisInput(INPUT_MOUSE_Y);
			float currentXRotation = this->transform.GetEulerRotation().x;

			// BUG HERE: This doesn't work correctly!
			if (abs(this->transform.GetEulerRotation().x) < glm::half_pi<float>() || glm::sign<float>(currentXRotation) != glm::sign<float>(yAxis)) // pitch down
			{
				rotation.x = yAxis * mousePitchXSensitivity * (float)TimeManager::DeltaTime();	// Pitch
			}

			rotation.y = (float)InputManager::GetMouseAxisInput(INPUT_MOUSE_X) * mouseYawYSensitivity * (float)TimeManager::DeltaTime();			// Yaw
	
			this->transform.Rotate(rotation);
		}


		// Handle direction:
		vec3 direction = vec3(0.0f, 0.0f, 0.0f);
		bool readInput = false;

		if (InputManager::GetInputState(INPUT_BUTTON_FORWARD))
		{
			direction += this->transform.Forward();
			readInput = true;
		}
		if (InputManager::GetInputState(INPUT_BUTTON_BACKWARD))
		{
			direction += this->transform.Forward() * -1.0f;
			readInput = true;
		}
		if (InputManager::GetInputState(INPUT_BUTTON_LEFT))
		{
			direction += this->transform.Right() * -1.0f;
			readInput = true;
		}
		if (InputManager::GetInputState(INPUT_BUTTON_RIGHT))
		{
			direction += this->transform.Right();
			readInput = true;
		}
		if (InputManager::GetInputState(INPUT_BUTTON_UP))
		{
			direction += this->transform.Up();
			readInput = true;
		}
		if (InputManager::GetInputState(INPUT_BUTTON_DOWN))
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

		



		//// test rota:
		//vec3 direction = vec3(0.0f, 0.0f, 0.0f);
		//bool readInput = false;

		//if (InputManager::GetInputState(INPUT_BUTTON_FORWARD))
		//{
		//	direction += Transform::WORLD_FORWARD;
		//	readInput = true;
		//}
		//if (InputManager::GetInputState(INPUT_BUTTON_BACKWARD))
		//{
		//	direction += Transform::WORLD_FORWARD * -1.0f;
		//	readInput = true;
		//}
		//if (InputManager::GetInputState(INPUT_BUTTON_LEFT))
		//{
		//	direction += Transform::WORLD_RIGHT * -1.0f;
		//	readInput = true;
		//}
		//if (InputManager::GetInputState(INPUT_BUTTON_RIGHT))
		//{
		//	direction += Transform::WORLD_RIGHT;
		//	readInput = true;
		//}
		//if (InputManager::GetInputState(INPUT_BUTTON_UP))
		//{
		//	direction += Transform::WORLD_UP;
		//	readInput = true;
		//}
		//if (InputManager::GetInputState(INPUT_BUTTON_DOWN))
		//{
		//	direction += Transform::WORLD_UP * -1.0f;
		//	readInput = true;
		//}

		//if (readInput)
		//{
		//	cout << "Direction 1 = " << direction.x << " " << direction.y << " " << direction.z << "\n";
		//	direction = glm::normalize(direction);
		//	direction *= (float)(movementSpeed * 0.1 * TimeManager::DeltaTime());

		//	cout << "Direction 2 = " << direction.x << " " << direction.y << " " << direction.z << "\n";

		//	this->transform.Rotate(direction);
		//}
	}

	//// EventListener interface:
	//void PlayerObject::HandleEvent(EventInfo const* eventInfo) 
	//{
	//	switch (eventInfo)
	//	{

	//	}
	//}
}
