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
		float movementSpeed = 1.0f;

	};

}

