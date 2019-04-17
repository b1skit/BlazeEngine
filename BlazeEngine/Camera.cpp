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
		this->transform.SetPosition(position);

		Initialize(aspectRatio, fieldOfView, near, far);
	}

	void Camera::Initialize(float aspectRatio, float fieldOfView, float near, float far)
	{
		this->fieldOfView = fieldOfView;
		this->aspectRatio = aspectRatio;
		this->near = near;
		this->far = far;

		projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, near, far);
		View(); // Internally initialize the view matrix
		viewProjection = projection * view;
	}

	mat4 const& Camera::View()
	{
		view = inverse(transform.Model());
		return view;
	}

	void Camera::Update()
	{
	}


	void Camera::HandleEvent(EventInfo const * eventInfo)
	{
	}
}

