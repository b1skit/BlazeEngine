#include "RenderTexture.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"
#include "Material.h"


namespace BlazeEngine
{
	RenderTexture::RenderTexture() 
		: RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("defaultShadowMapWidth"), 
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("defaultShadowMapHeight"),
			DEFAULT_RENDERTEXTURE_NAME
		)
	{}	// Do nothing else


	RenderTexture::RenderTexture(int width, int height, string name /*= DEFAULT_RENDERTEXTURE_NAME*/)
	{
		this->width					= width;
		this->height				= height;
		numTexels					= width * height;

		this->texturePath			= name;

		this->texels				= nullptr;
		this->resolutionHasChanged	= true;

		// Override default values:
		//-------------------------
		this->internalFormat		= GL_DEPTH_COMPONENT32F;
		this->format				= GL_DEPTH_COMPONENT;
		this->type					= GL_FLOAT; // Same as Texture...
		
		this->textureWrapS			= GL_CLAMP_TO_EDGE; // NOTE: Mandatory for non-power-of-two textures
		this->textureWrapT			= GL_CLAMP_TO_EDGE;


		this->textureMinFilter		= GL_LINEAR;
		this->textureMaxFilter		= GL_LINEAR;
	}


	RenderTexture::RenderTexture(RenderTexture const& rhs) : Texture(rhs)
	{
		this->frameBufferObject = 0;	// NOTE: We set the frame buffer to 0, since we don't want to stomp any existing ones

		this->attachmentPoint	= rhs.attachmentPoint;

		this->drawBuffer		= rhs.drawBuffer;
		this->readBuffer		= rhs.readBuffer;
	}


	RenderTexture& RenderTexture::operator=(RenderTexture const& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		Texture::operator=(rhs);

		//this->frameBufferObject = 0;	// NOTE: We set the frame buffer to 0, since we don't want to stomp any existing ones
		this->frameBufferObject	= rhs.frameBufferObject;

		this->attachmentPoint	= rhs.attachmentPoint;

		this->drawBuffer = rhs.drawBuffer;
		this->readBuffer = rhs.readBuffer;

		return *this;
	}


	// NOTE: additionalRTs must be cleaned up by the caller
	// NOTE: The correct attachment points must already be configured for each RenderTexture
	// NOTE: The additionalRT's must have already successfully called Texture::Buffer()
	void RenderTexture::AttachAdditionalRenderTexturesToFramebuffer(RenderTexture** additionalRTs, int numRTs, bool isDepth /*=false*/)
	{
		if (isDepth && numRTs != 1)
		{
			LOG_ERROR("Cannot add more than 1 depth buffer to a framebuffer. Returning without attaching");
			return;
		}

		this->BindFramebuffer(true);

		if (isDepth)
		{
			(*additionalRTs)->AttachToFramebuffer((*additionalRTs)->TextureTarget());
		}
		else
		{
			for (int currentRT = 0; currentRT < numRTs; currentRT++)
			{
				additionalRTs[currentRT]->AttachToFramebuffer(additionalRTs[currentRT]->TextureTarget());
			}

			// Assemble a list of attachment points
			int totalRTs		= numRTs + 1;
			GLenum* drawBuffers = new GLenum[totalRTs];
			drawBuffers[0]		= this->attachmentPoint;
			for (int i = 1; i < totalRTs; i++)
			{
				drawBuffers[i] = additionalRTs[i - 1]->attachmentPoint;
			}

			glDrawBuffers(totalRTs, drawBuffers);
		}		

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG_ERROR("Attaching additional buffers failed")
		}

		// Cleanup:
		this->BindFramebuffer(false);
	}


	void RenderTexture::Destroy()
	{
		Texture::Destroy();

		glDeleteFramebuffers(1, &frameBufferObject);
	}


	bool RenderTexture::Buffer(int textureUnit)
	{
		if (Texture::Buffer(textureUnit)) // Makes required calls to glTexParameteri, binds textureID etc 
		{
			this->BindFramebuffer(true);

			if (!glIsFramebuffer(frameBufferObject))
			{
				glGenFramebuffers(1, &frameBufferObject);
				this->BindFramebuffer(true);
				if (!glIsFramebuffer(frameBufferObject))
				{
					LOG_ERROR("Failed to create framebuffer object");
					return false;
				}

				glDrawBuffer(this->drawBuffer); // Sets the color buffer to draw too (eg. GL_NONE for a depth map)
				glReadBuffer(this->readBuffer);

				// Configure framebuffer parameters:
				glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, this->width);
				glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, this->height);

				// Attach our texture to the framebuffer as a render buffer:
				this->AttachToFramebuffer(this->texTarget);
			}

			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Render texture setup complete!");
			#endif

			bool result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
			if (!result)
			{
				LOG_ERROR("Framebuffer is not complete: " + to_string(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
			}

			// Cleanup:
			this->BindFramebuffer(false);
			glBindTexture(this->texTarget, 0);

			return result;
		}
		else
		{
			LOG_ERROR("Texture buffer failed. Render texture could not be buffered either");

			return false;
		}		
	}
	

	bool RenderTexture::BufferCubeMap(RenderTexture** cubeFaceRTs, int textureUnit)
	{
		// NOTE: This function uses the paramters of cubeFaceRTs[0]

		if (!Texture::BufferCubeMap((Texture**)cubeFaceRTs, textureUnit))
		{
			return false;
		}

		// RenderTexture specific setup:
		LOG("Configuring cube map as RenderTexture: \"" + cubeFaceRTs[0]->TexturePath() + "\"");

		// Generate faces:
		for (int i = 0; i < CUBE_MAP_NUM_FACES; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cubeFaceRTs[0]->internalFormat, cubeFaceRTs[0]->width, cubeFaceRTs[0]->height, 0, cubeFaceRTs[0]->format, cubeFaceRTs[0]->type, NULL);
		}

		// Ensure all of the other textures have the same ID, sampler, and FBO info:
		for (int i = 1; i < CUBE_MAP_NUM_FACES; i++)
		{
			cubeFaceRTs[i]->textureID			= cubeFaceRTs[0]->textureID;
			cubeFaceRTs[i]->samplerID			= cubeFaceRTs[0]->samplerID;
			cubeFaceRTs[i]->frameBufferObject	= cubeFaceRTs[0]->frameBufferObject;
		}

		// Bind framebuffer:
		bool result = true;
		cubeFaceRTs[0]->BindFramebuffer(true);
		if (!glIsFramebuffer(cubeFaceRTs[0]->frameBufferObject))
		{
			glGenFramebuffers(1, &cubeFaceRTs[0]->frameBufferObject);
			cubeFaceRTs[0]->BindFramebuffer(true);
			if (!glIsFramebuffer(cubeFaceRTs[0]->frameBufferObject))
			{
				LOG_ERROR("Failed to create framebuffer object");
				return false;
			}

			// Attach framebuffer as a cube map render buffer:
			glFramebufferTexture(GL_FRAMEBUFFER, cubeFaceRTs[0]->attachmentPoint, cubeFaceRTs[0]->textureID, 0);		

			glDrawBuffer(cubeFaceRTs[0]->drawBuffer);
			glReadBuffer(cubeFaceRTs[0]->readBuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				LOG_ERROR("Cube map framebuffer is not complete!");
				result = false;
			}
		}

		// Cleanup:
		glBindTexture(cubeFaceRTs[0]->texTarget, 0); // Was still bound from Texture::BufferCubeMap()
		cubeFaceRTs[0]->BindFramebuffer(false);

		return result;
	}


	RenderTexture** RenderTexture::CreateCubeMap(int xRes, int yRes, string name /*="UNNAMMED"*/)
	{
		RenderTexture** cubeFaces = new RenderTexture*[CUBE_MAP_NUM_FACES];

		// Attach a texture to each slot:
		for (int i = 0; i < CUBE_MAP_NUM_FACES; i++)
		{
			RenderTexture* cubeRenderTexture = new RenderTexture
			(
				xRes,
				yRes,
				name + "_CubeMap"
			);

			// Configure the texture:
			cubeRenderTexture->TextureTarget()		= GL_TEXTURE_CUBE_MAP;

			cubeRenderTexture->TextureWrap_S()		= GL_CLAMP_TO_EDGE;
			cubeRenderTexture->TextureWrap_T()		= GL_CLAMP_TO_EDGE;
			cubeRenderTexture->TextureWrap_R()		= GL_CLAMP_TO_EDGE;

			cubeRenderTexture->TextureMinFilter()	= GL_NEAREST;
			cubeRenderTexture->TextureMaxFilter()	= GL_NEAREST;

			cubeRenderTexture->InternalFormat()		= GL_DEPTH_COMPONENT32F;
			cubeRenderTexture->Format()				= GL_DEPTH_COMPONENT;
			cubeRenderTexture->Type()				= GL_FLOAT;

			cubeRenderTexture->AttachmentPoint()	= GL_DEPTH_ATTACHMENT;	// Preparing a shadow map by default...
			cubeRenderTexture->DrawBuffer()			= GL_NONE;
			cubeRenderTexture->ReadBuffer()			= GL_NONE;

			// Cache off the texture for buffering when we're done
			cubeFaces[i] = cubeRenderTexture;
		}

		return cubeFaces;
	}


	void RenderTexture::BindFramebuffer(bool doBind)
	{
		if (doBind)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferObject);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}


	void RenderTexture::AttachToFramebuffer(GLenum textureTarget, int mipLevel /*= 0*/)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, this->attachmentPoint, textureTarget, this->textureID, mipLevel);
	}


	void RenderTexture::CreateRenderbuffer(bool leaveBound /*= true*/, int xRes /*= -1*/, int yRes /*= -1*/)
	{
		if (xRes <= 0 || yRes <= 0)
		{
			xRes = this->width;
			yRes = this->height;
		}

		if (glIsRenderbuffer(this->frameBufferObject) == GL_FALSE)
		{
			glGenRenderbuffers(1, &this->frameBufferObject);
		}

		this->BindRenderbuffer(true);

		// Allocate storage: 
		// NOTE: For now, we hard code internalFormat == GL_DEPTH_COMPONENT24, as it's all we ever use...
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, xRes, yRes);

		if (leaveBound == false)
		{
			this->BindRenderbuffer(false);
		}
	}


	void RenderTexture::BindRenderbuffer(bool doBind)
	{
		if (doBind)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, this->frameBufferObject);
		}
		else
		{
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}


	void RenderTexture::DeleteRenderbuffer(bool unbind /*= true*/)
	{
		if (unbind == true)
		{
			this->BindRenderbuffer(false);
		}		

		glDeleteRenderbuffers(1, &this->frameBufferObject);
	}
}


