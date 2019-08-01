#include "PlayerObject.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "CoreEngine.h"

#include "gtc/constants.hpp"


namespace BlazeEngine
{
	PlayerObject::PlayerObject(Camera* playerCam) : GameObject::GameObject("Player Object")
	{
		this->playerCam = playerCam;
		this->playerCam->GetTransform()->Parent(&this->transform);

		// Move the yaw (ie. about Y) rotation from the camera to the PlayerObject's transform:

		Transform* playerCamTransform = this->playerCam->GetTransform();
		vec3 camRotation = playerCamTransform->GetEulerRotation();
		vec3 camPosition = playerCamTransform->WorldPosition();

		playerCamTransform->SetWorldRotation(vec3(camRotation.x, 0.0f, 0.0f));	// Set pitch
		playerCamTransform->SetWorldPosition(vec3(0.0f, 0.0f, 0.0f));			// Relative to PlayerObject parent
		
		this->transform.SetWorldRotation(vec3(0.0f, camRotation.y, 0.0f));		// Set yaw
		this->transform.SetWorldPosition(camPosition);
	}


	void PlayerObject::Update()
	{
		GameObject::Update();

		// Handle first person view orientation: (pitch + yaw)
		if (InputManager::GetInputState(INPUT_MOUSE_AXIS))
		{
			vec3 yaw(0.0f, 0.0f, 0.0f);
			vec3 pitch(0.0f, 0.0f, 0.0f);

			// Compute rotation amounts, in radians:
			yaw.y	= (float)InputManager::GetMouseAxisInput(INPUT_MOUSE_X) * (float)TimeManager::DeltaTime();
			pitch.x = (float)InputManager::GetMouseAxisInput(INPUT_MOUSE_Y) * (float)TimeManager::DeltaTime();

			this->transform.Rotate(yaw);
			this->playerCam->GetTransform()->Rotate(pitch);
		}

		// Handle direction:
		vec3 direction = vec3(0.0f, 0.0f, 0.0f);

		if (InputManager::GetInputState(INPUT_BUTTON_FORWARD))
		{
			vec3 forward = this->transform.Forward();
			Transform::RotateVector(forward, this->playerCam->GetTransform()->GetEulerRotation().x, this->transform.Right());

			direction += forward  * -1.0f;			
		}
		if (InputManager::GetInputState(INPUT_BUTTON_BACKWARD))
		{
			vec3 forward = this->transform.Forward();
			Transform::RotateVector(forward, this->playerCam->GetTransform()->GetEulerRotation().x, this->transform.Right());

			direction += forward;
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


		// Reset the cam back to the saved position
		if (InputManager::GetInputState(INPUT_MOUSE_LEFT))
		{
			this->transform.SetWorldPosition(savedPosition);
			this->transform.SetWorldRotation(vec3(0, savedEulerRotation.y, 0));
			this->playerCam->GetTransform()->SetWorldRotation(vec3(savedEulerRotation.x, 0, 0));
		}

		// Save the current position/rotation:
		if (InputManager::GetInputState(INPUT_MOUSE_RIGHT))
		{
			this->savedPosition = transform.WorldPosition();
			this->savedEulerRotation = vec3(this->playerCam->GetTransform()->GetEulerRotation().x, this->transform.GetEulerRotation().y, 0 );
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
