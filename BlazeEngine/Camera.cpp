#include "Camera.h"
#include "CoreEngine.h"


namespace BlazeEngine
{
	// Default constructor
	Camera::Camera(string cameraName) : SceneObject::SceneObject(cameraName)
	{
		Initialize	// Initialize with default values:
		(		
			CoreEngine::GetCoreEngine()->GetConfig()->GetAspectRatio(),
			CoreEngine::GetCoreEngine()->GetConfig()->mainCam.defaultFieldOfView,
			CoreEngine::GetCoreEngine()->GetConfig()->mainCam.defaultNear,
			CoreEngine::GetCoreEngine()->GetConfig()->mainCam.defaultFar,
			nullptr,
			vec3(0,0,0),
			false,
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultOrthoHalfWidth,
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultOrthoHalfHeight
		); 

		/*isDirty = false;*/
	}

	Camera::Camera(string cameraName, vec3 position, float fieldOfView, float near, float far, float aspectRatio, Transform* parent /*= nullptr*/, bool isOrthographic /*= false*/, float orthoHalfWidth /*= 5*/, float orthoHalfHeight /*= 5*/) : SceneObject::SceneObject(cameraName)
	{
		Initialize(fieldOfView, aspectRatio, near, far, parent, position, isOrthographic, orthoHalfWidth, orthoHalfHeight);
	}

	
	//Camera::~Camera()
	//{
	//}*/


	void Camera::Initialize(float aspectRatio, float fieldOfView, float near, float far, Transform* parent /*= nullptr*/, vec3 position /*= vec3(0.0f, 0.0f, 0.0f)*/, bool isOrthographic /*= false*/, float orthoHalfWidth /*= 5*/, float orthoHalfHeight /*= 5*/)
	{
		this->aspectRatio		= aspectRatio;
		this->fieldOfView		= fieldOfView;
		this->near				= near;
		this->far				= far;

		this->transform.SetPosition(position);

		this->GetTransform()->SetParent(parent);

		this->isOrthographic	= isOrthographic;
		this->orthoHalfWidth	= orthoHalfWidth;
		this->orthoHalfHeight	= orthoHalfHeight;

		if (isOrthographic)
		{
			this->projection = glm::ortho((float)-orthoHalfWidth, (float)orthoHalfWidth, (float)-orthoHalfHeight, (float)orthoHalfHeight, this->near, this->far);
		}
		else
		{
			this->projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, near, far);
		}

		View();					// Internally initialize the view matrix

		this->viewProjection	= projection * view;
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

