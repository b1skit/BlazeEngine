#include "Camera.h"

namespace BlazeEngine
{
	/*Camera::Camera()
	{
	}


	Camera::~Camera()
	{
	}*/

	void Camera::Initialize(vec3 position, float aspectRatio, float fieldOfView, float near, float far)
	{
		this->fieldOfView = fieldOfView;
		this->aspectRatio = aspectRatio;
		this->near = near;
		this->far = far;

		this->transform.SetPosition(position);

		projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, near, far);
		View(); // Internally initialize the view matrix
		viewProjection = projection * view;
	}

	void Camera::Update()
	{
	}

	void Camera::HandleEvent(EventInfo const * eventInfo)
	{
	}

}

