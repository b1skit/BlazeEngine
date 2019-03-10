#pragma once

#include "Camera.h"
#include "GameObject.h"

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
		float movementSpeed = 0.2f;
		
		// TO DO: Initialize from core engine config
		float mousePitchXSensitivity = -0.001f; 
		float mouseYawYSensitivity = -0.001f;
	};

}

