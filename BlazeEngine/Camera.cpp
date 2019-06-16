#include "BuildConfiguration.h"
#include "Camera.h"
#include "CoreEngine.h"

#include "glm.hpp"


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


	// Perspective constructor
	Camera::Camera(string cameraName, float aspectRatio, float fieldOfView, float near, float far, Transform* parent /*= nullptr*/, vec3 position /*= vec3(0.0f, 0.0f, 0.0f)*/) : SceneObject::SceneObject(cameraName)
	{
		Initialize(aspectRatio, fieldOfView, near, far, parent, position);
	}


	// Orthographic constructor:
	Camera::Camera(string cameraName, float near, float far, Transform* parent /*= nullptr*/, vec3 position /*= vec3(0.0f, 0.0f, 0.0f)*/, float orthoLeft /*= -5*/, float orthoRight /*= 5*/, float orthoBottom /*= -5*/, float orthoTop /*= 5*/) : SceneObject::SceneObject(cameraName)
	{
		Initialize
		(
			glm::abs((orthoRight - orthoLeft) / (orthoTop - orthoBottom)), 
			0.0f, 
			near, 
			far, 
			parent, 
			position, 
			true, 
			orthoLeft, 
			orthoRight, 
			orthoBottom, 
			orthoTop
		);
	}


	void Camera::Destroy()
	{
		if (this->renderMaterial != nullptr)
		{
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				Texture* currentTexture = renderMaterial->GetTexture((TEXTURE_TYPE)i);
				if (currentTexture != nullptr)
				{
					currentTexture->Destroy();
					delete currentTexture;
					currentTexture = nullptr;
				}
			}

			delete renderMaterial;
			renderMaterial = nullptr;
		}
	}


	void Camera::Initialize(float aspectRatio, float fieldOfView, float near, float far, Transform* parent /*= nullptr*/, vec3 position /*= vec3(0.0f, 0.0f, 0.0f)*/, bool isOrthographic /*= false*/, float orthoLeft /*= -5*/, float orthoRight /*= 5*/, float orthoBottom /*= -5*/, float orthoTop /*= 5*/)
	{
		this->near					= near;
		this->far					= far;
		this->aspectRatio			= aspectRatio;

		this->transform.SetPosition(position);

		this->GetTransform()->Parent(parent);

		this->isOrthographic		= isOrthographic;
		if (isOrthographic)
		{	
			this->fieldOfView		= 0.0f;

			this->orthoLeft			= orthoLeft;
			this->orthoRight		= orthoRight;
			this->orthoBottom		= orthoBottom;
			this->orthoTop			= orthoTop;

			this->projection		= glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, this->near, this->far);
		}
		else
		{
			this->fieldOfView		= fieldOfView;

			this->orthoLeft			= 0.0f;
			this->orthoRight		= 0.0f;
			this->orthoBottom		= 0.0f;
			this->orthoTop			= 0.0f;

			this->projection		= glm::perspective(glm::radians(fieldOfView), aspectRatio, near, far);
		}

		View();						// Internally initialize the view matrix

		this->viewProjection		= projection * view;
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

	void Camera::DebugPrint()
	{
		#if defined(DEBUG_TRANSFORMS)
			LOG("\n[CAMERA DEBUG: " + this->GetName() + "]");
			LOG("Field of view: " + to_string(fieldOfView));
			LOG("Near: " + to_string(near));
			LOG("Far: " + to_string(far));
			LOG("Aspect ratio: " + to_string(aspectRatio));

			LOG("Position: " + to_string(transform.Position().x) + " " + to_string(transform.Position().y) + " " + to_string(transform.Position().z));
			LOG("Euler Rotation: " + to_string(transform.GetEulerRotation().x) + " " + to_string(transform.GetEulerRotation().y) + " " + to_string(transform.GetEulerRotation().z));
			
			// NOTE: OpenGL matrics are stored in column-major order
			LOG("\nView Matrix:\n\t" + to_string(view[0][0]) + " " + to_string(view[1][0]) + " " + to_string(view[2][0]) + " " + to_string(view[3][0]) );
			LOG("\t" + to_string(view[0][1]) + " " + to_string(view[1][1]) + " " + to_string(view[2][1]) + " " + to_string(view[3][1]));
			LOG("\t" + to_string(view[0][2]) + " " + to_string(view[1][2]) + " " + to_string(view[2][2]) + " " + to_string(view[3][2]));
			LOG("\t" + to_string(view[0][3]) + " " + to_string(view[1][3]) + " " + to_string(view[2][3]) + " " + to_string(view[3][3]));

			LOG("\nProjection Matrix:\n\t" + to_string(projection[0][0]) + " " + to_string(projection[1][0]) + " " + to_string(projection[2][0]) + " " + to_string(projection[3][0]));
			LOG("\t" + to_string(projection[0][1]) + " " + to_string(projection[1][1]) + " " + to_string(projection[2][1]) + " " + to_string(projection[3][1]));
			LOG("\t" + to_string(projection[0][2]) + " " + to_string(projection[1][2]) + " " + to_string(projection[2][2]) + " " + to_string(projection[3][2]));
			LOG("\t" + to_string(projection[0][3]) + " " + to_string(projection[1][3]) + " " + to_string(projection[2][3]) + " " + to_string(projection[3][3]));

			LOG("\nView Projection Matrix:\n\t" + to_string(viewProjection[0][0]) + " " + to_string(viewProjection[1][0]) + " " + to_string(viewProjection[2][0]) + " " + to_string(viewProjection[3][0]));
			LOG("\t" + to_string(viewProjection[0][1]) + " " + to_string(viewProjection[1][1]) + " " + to_string(viewProjection[2][1]) + " " + to_string(viewProjection[3][1]));
			LOG("\t" + to_string(viewProjection[0][2]) + " " + to_string(viewProjection[1][2]) + " " + to_string(viewProjection[2][2]) + " " + to_string(viewProjection[3][2]));
			LOG("\t" + to_string(viewProjection[0][3]) + " " + to_string(viewProjection[1][3]) + " " + to_string(viewProjection[2][3]) + " " + to_string(viewProjection[3][3]));
		#else
			return;
		#endif
	}
}

