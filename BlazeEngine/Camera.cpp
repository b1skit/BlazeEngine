#include "BuildConfiguration.h"
#include "Camera.h"
#include "CoreEngine.h"

#include "glm.hpp"


namespace BlazeEngine
{
	// Default constructor
	Camera::Camera(string cameraName) : SceneObject::SceneObject(cameraName)
	{
		Initialize();	// Initialize with default values

		/*isDirty = false;*/
	}


	Camera::Camera(string cameraName, CameraConfig camConfig, Transform* parent /*= nullptr*/) : SceneObject::SceneObject(cameraName)
	{
		this->cameraConfig = camConfig;

		this->transform.Parent(parent);

		Initialize();
	}


	void Camera::Initialize()
	{
		if (this->cameraConfig.isOrthographic)
		{
			this->cameraConfig.fieldOfView = 0.0f;

			this->projection = glm::ortho
			(
				this->cameraConfig.orthoLeft, 
				this->cameraConfig.orthoRight, 
				this->cameraConfig.orthoBottom, 
				this->cameraConfig.orthoTop, 
				this->cameraConfig.near, 
				this->cameraConfig.far
			);
		}
		else
		{
			this->cameraConfig.orthoLeft	= 0.0f;
			this->cameraConfig.orthoRight	= 0.0f;
			this->cameraConfig.orthoBottom	= 0.0f;
			this->cameraConfig.orthoTop		= 0.0f;

			this->projection = glm::perspective
			(
				glm::radians(this->cameraConfig.fieldOfView), 
				this->cameraConfig.aspectRatio, 
				this->cameraConfig.near, 
				this->cameraConfig.far
			);
		}

		View();						// Internally initializes the view matrix

		this->viewProjection = projection * view;
	}


	void Camera::Destroy()
	{
		if (this->renderMaterial != nullptr)
		{
			for (int i = 0; i < TEXTURE_COUNT; i++)
			{
				Texture* currentTexture = renderMaterial->AccessTexture((TEXTURE_TYPE)i);
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

