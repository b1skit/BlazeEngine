#pragma once

#include "GameObject.h"
#include "Camera.h"


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
	};

}

