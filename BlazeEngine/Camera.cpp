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


	void Camera::AttachGBuffer()
	{
		Material* gBufferMaterial	= new Material(this->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->shader.gBufferShaderName, RENDER_TEXTURE_COUNT);
		this->RenderMaterial()		= gBufferMaterial;

		// We use the albedo texture as a basis for the others
		RenderTexture* gBuffer_albedo = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes,
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
			this->GetName() + "_" + Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_ALBEDO],
			false
		);
		gBuffer_albedo->InternalFormat()	= GL_RGBA32F;
		gBuffer_albedo->Format()			= GL_RGBA;		// NOTE: Using 4 channels for future flexibility
		gBuffer_albedo->TextureMinFilter()	= GL_NEAREST_MIPMAP_LINEAR;
		gBuffer_albedo->TextureMaxFilter()	= GL_LINEAR;

		gBuffer_albedo->AttachmentPoint()	= GL_COLOR_ATTACHMENT0 + 0; // Need to increment by 1 each new textuer we attach. Used in RenderTexture.Buffer()->glFramebufferTexture2D()

		gBuffer_albedo->ReadBuffer()		= GL_COLOR_ATTACHMENT0 + 0;
		gBuffer_albedo->DrawBuffer()		= GL_COLOR_ATTACHMENT0 + 0;

		gBuffer_albedo->Buffer();

		GLuint gBufferFBO = gBuffer_albedo->FBO(); // Cache off the FBO for the other GBuffer textures

		// Store references to our additonal RenderTextures:
		int numAdditionalRTs = (int)RENDER_TEXTURE_COUNT - 2;
		RenderTexture** additionalRTs = new RenderTexture* [numAdditionalRTs];

		int insertIndex = 0;
		int attachmentIndexOffset = 1;
		for (int currentType = 1; currentType < (int)RENDER_TEXTURE_COUNT; currentType++)	 // -1 b/c we'll add the depth texture last
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

			currentRT->Texture::Buffer();	// Generate a texture without bothering to attempt to bind a framebuffer

			gBufferMaterial->AccessTexture((TEXTURE_TYPE)currentType)	= currentRT;
			additionalRTs[insertIndex]									= currentRT;

			insertIndex++;
			attachmentIndexOffset++;
		}

		// Attach the textures to the existing FBO
		gBuffer_albedo->AttachAdditionalRenderTexturesToFramebuffer(additionalRTs, numAdditionalRTs);
		delete additionalRTs;	// Cleanup

		// Configure the depth buffer:
		RenderTexture* depth = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes,
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
			this->GetName() + "_" + Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_DEPTH],
			false
		);

		depth->TexturePath()	= this->GetName() + "_" + Material::RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_DEPTH];
		depth->FBO()			= gBufferFBO;

		depth->Texture::Buffer();

		gBuffer_albedo->AttachAdditionalRenderTexturesToFramebuffer(&depth, 1);
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

