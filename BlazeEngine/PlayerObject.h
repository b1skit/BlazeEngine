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

	protected:


	private:
		Camera playerCam;


	};

}

