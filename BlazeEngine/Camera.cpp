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

		if (this->cubeView != nullptr)
		{
			delete[] cubeView;
			cubeView = nullptr;
		}

		if (this->cubeViewProjection != nullptr)
		{
			delete[] cubeViewProjection;
			cubeViewProjection = nullptr;
		}
	}


	mat4 const& Camera::View()
	{
		view = inverse(transform.Model());
		return view;
	}

	mat4 const* Camera::CubeView()
	{
		// If we've never allocated cubeView, do it now:
		if (cubeView == nullptr)
		{
			cubeView = new mat4[6];
			
			cubeView[0] = glm::lookAt(this->transform.WorldPosition(), this->transform.WorldPosition() + Transform::WORLD_X, -Transform::WORLD_Y);
			cubeView[1] = glm::lookAt(this->transform.WorldPosition(), this->transform.WorldPosition() - Transform::WORLD_X, -Transform::WORLD_Y);

			cubeView[2] = glm::lookAt(this->transform.WorldPosition(), this->transform.WorldPosition() + Transform::WORLD_Y, Transform::WORLD_Z);
			cubeView[3] = glm::lookAt(this->transform.WorldPosition(), this->transform.WorldPosition() - Transform::WORLD_Y, -Transform::WORLD_Z);

			cubeView[4] = glm::lookAt(this->transform.WorldPosition(), this->transform.WorldPosition() + Transform::WORLD_Z, -Transform::WORLD_Y);
			cubeView[5] = glm::lookAt(this->transform.WorldPosition(), this->transform.WorldPosition() - Transform::WORLD_Z, -Transform::WORLD_Y);
		}

		// TODO: Recalculate this if the camera has moved

		return cubeView;
	}

	mat4 const* Camera::CubeViewProjection()
	{
		// If we've never allocated cubeViewProjection, do it now:
		if (cubeViewProjection == nullptr)
		{
			cubeViewProjection = new mat4[6];

			mat4 const* ourCubeViews = this->CubeView(); // Call this to ensure cubeView has been initialized

			cubeViewProjection[0] = this->projection * ourCubeViews[0];
			cubeViewProjection[1] = this->projection * ourCubeViews[1];
			cubeViewProjection[2] = this->projection * ourCubeViews[2];
			cubeViewProjection[3] = this->projection * ourCubeViews[3];
			cubeViewProjection[4] = this->projection * ourCubeViews[4];
			cubeViewProjection[5] = this->projection * ourCubeViews[5];
		}

		return cubeViewProjection;
	}


	void Camera::AttachGBuffer()
	{
		Material* gBufferMaterial	= new Material(this->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("gBufferFillShaderName"), RENDER_TEXTURE_COUNT, true);
		this->RenderMaterial()		= gBufferMaterial;

		// We use the albedo texture as a basis for the others
		RenderTexture* gBuffer_albedo = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowXRes"),
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowYRes"),
			this->GetName() + "_" + Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_ALBEDO],
			false,
			RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO
		);
		gBuffer_albedo->Format()			= GL_RGBA;		// Note: Using 4 channels for future flexibility
		gBuffer_albedo->InternalFormat()	= GL_RGBA32F;		
		
		gBuffer_albedo->TextureMinFilter()	= GL_LINEAR;	// Note: Output is black if this is GL_NEAREST_MIPMAP_LINEAR
		gBuffer_albedo->TextureMaxFilter()	= GL_LINEAR;

		gBuffer_albedo->AttachmentPoint()	= GL_COLOR_ATTACHMENT0 + 0; // Need to increment by 1 each new textuer we attach. Used in RenderTexture.Buffer()->glFramebufferTexture2D()

		gBuffer_albedo->ReadBuffer()		= GL_COLOR_ATTACHMENT0 + 0;
		gBuffer_albedo->DrawBuffer()		= GL_COLOR_ATTACHMENT0 + 0;

		gBuffer_albedo->Buffer();

		GLuint gBufferFBO = gBuffer_albedo->FBO(); // Cache off the FBO for the other GBuffer textures

		gBufferMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO) = gBuffer_albedo;

		// Store references to our additonal RenderTextures:
		int numAdditionalRTs			= (int)RENDER_TEXTURE_COUNT - 2; // -2 b/c we already have 1, and we'll add the depth texture last
		RenderTexture** additionalRTs	= new RenderTexture*[numAdditionalRTs];

		int insertIndex = 0;
		int attachmentIndexOffset = 1;
		for (int currentType = 1; currentType < (int)RENDER_TEXTURE_COUNT; currentType++)	 
		{
			if ((TEXTURE_TYPE)currentType == RENDER_TEXTURE_DEPTH)
			{
				continue;
			}

			RenderTexture* currentRT		= new RenderTexture(*gBuffer_albedo);	// We're creating the same type of textures, attached to the same framebuffer
			currentRT->TexturePath()		= this->GetName() + "_" + Material::RENDER_TEXTURE_SAMPLER_NAMES[(TEXTURE_TYPE)currentType];

			currentRT->FBO()				= gBufferFBO;
			currentRT->AttachmentPoint()	= GL_COLOR_ATTACHMENT0 + attachmentIndexOffset;
			currentRT->ReadBuffer()			= GL_COLOR_ATTACHMENT0 + attachmentIndexOffset;
			currentRT->DrawBuffer()			= GL_COLOR_ATTACHMENT0 + attachmentIndexOffset;
			
			currentRT->TextureUnit()		= RENDER_TEXTURE_0 + currentType;

			currentRT->Texture::Buffer();	// Generate a texture without bothering to attempt to bind a framebuffer

			glBindTexture(currentRT->TextureTarget(), 0); // Cleanup: Texture was never unbound in Texture::Buffer, so we must unbind it here

			gBufferMaterial->AccessTexture((TEXTURE_TYPE)currentType)	= currentRT;
			additionalRTs[insertIndex]									= currentRT;

			insertIndex++;
			attachmentIndexOffset++;
		}

		// Attach the textures to the existing FBO
		gBuffer_albedo->AttachAdditionalRenderTexturesToFramebuffer(additionalRTs, numAdditionalRTs);
		delete [] additionalRTs;	// Cleanup

		// Configure the depth buffer:
		RenderTexture* depth = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowXRes"),
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowYRes"),
			this->GetName() + "_" + Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_DEPTH],
			false,
			RENDER_TEXTURE_0 + RENDER_TEXTURE_DEPTH
		);

		// Add the new texture to our material:
		gBufferMaterial->AccessTexture(RENDER_TEXTURE_DEPTH) = depth;

		depth->TexturePath()	= this->GetName() + "_" + Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_DEPTH];
		depth->FBO()			= gBufferFBO;

		depth->Texture::Buffer();

		glBindTexture(depth->TextureTarget(), 0); // Cleanup: Texture was never unbound in Texture::Buffer, so we must unbind it here

		gBuffer_albedo->AttachAdditionalRenderTexturesToFramebuffer(&depth, 1, true);
	}


	void Camera::DebugPrint()
	{
		#if defined(DEBUG_TRANSFORMS)
			LOG("\n[CAMERA DEBUG: " + this->GetName() + "]");
			LOG("Field of view: " + to_string(fieldOfView));
			LOG("Near: " + to_string(near));
			LOG("Far: " + to_string(far));
			LOG("Aspect ratio: " + to_string(aspectRatio));

			LOG("Position: " + to_string(transform.WorldPosition().x) + " " + to_string(transform.WorldPosition().y) + " " + to_string(transform.WorldPosition().z));
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

