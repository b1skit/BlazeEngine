#pragma once

#include "GameObject.h"
#include "Camera.h"

#include "glm.hpp"

using glm::vec3;


namespace BlazeEngine
{
	class PlayerObject : public GameObject
	{
	public:
		PlayerObject();
		/*~PlayerObject();*/


		// Getters/Setters:
		inline Camera* GetCamera() { return &playerCam; }

		// BlazeObject interface:
		void Update() override;

		//// EventListener interface:
		//void HandleEvent(EventInfo const* eventInfo) override;

	protected:


	private:
		Camera playerCam;

		// Control configuration:
		float movementSpeed = 0.01f;

		// Saved positions
		vec3 savedPosition = vec3(0.0f, 0.0f, 0.0f);
		vec3 savedEulerRotation = vec3(0.0f, 0.0f, 0.0f);
	};

}

