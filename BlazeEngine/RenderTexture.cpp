#include "RenderTexture.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"


namespace BlazeEngine
{
	RenderTexture::RenderTexture() 
		: RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapWidth, 
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapHeight,
			DEFAULT_RENDERTEXTURE_NAME
		)
	{}	// Do nothing else


	RenderTexture::RenderTexture(int width, int height, string name /*= DEFAULT_RENDERTEXTURE_NAME*/, bool doBuffer /*= false*/, int textureUnit /*= -1*/)
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

		if (textureUnit >= 0)
		{
			this->textureUnit = textureUnit;
		}

		if (doBuffer)
		{
			if (textureUnit < 0)
			{
				LOG_ERROR("Cannot buffer RenderTexture \"" + name + "\" before the textureUnit has been set");
			}
			else
			{
				this->Buffer();
			}
		}
	}


	RenderTexture::RenderTexture(RenderTexture const& rhs, bool doBuffer /*= false */) : Texture(rhs)
	{
		this->frameBufferObject = 0;	// NOTE: We set the frame buffer to 0, since we don't want to stomp any existing ones

		this->attachmentPoint	= rhs.attachmentPoint;

		this->drawBuffer		= rhs.drawBuffer;
		this->readBuffer		= rhs.readBuffer;

		if (doBuffer)
		{
			Buffer();
		}
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

		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferObject); // Reattach the FBO

		if (isDepth)
		{
			/*glFramebufferTexture2D(GL_FRAMEBUFFER, additionalRTs[0]->AttachmentPoint(), additionalRTs[0]->TextureTarget(), additionalRTs[0]->TextureID(), 0);*/
			glFramebufferTexture2D(GL_FRAMEBUFFER, (*additionalRTs)->AttachmentPoint(), (*additionalRTs)->TextureTarget(), (*additionalRTs)->TextureID(), 0);
		}
		else
		{
			for (int currentRT = 0; currentRT < numRTs; currentRT++)
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, additionalRTs[currentRT]->AttachmentPoint(), additionalRTs[currentRT]->TextureTarget(), additionalRTs[currentRT]->TextureID(), 0);
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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void RenderTexture::Destroy()
	{
		Texture::Destroy();

		glDeleteFramebuffers(1, &frameBufferObject);
	}


	bool RenderTexture::Buffer()
	{
		if (Texture::Buffer()) // Makes required calls to glTexParameteri, binds textureID etc 
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

			if (!glIsFramebuffer(frameBufferObject))
			{
				glGenFramebuffers(1, &frameBufferObject);
				glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
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
				glFramebufferTexture2D(GL_FRAMEBUFFER, this->attachmentPoint, this->texTarget, this->textureID, 0);
			}

			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Render texture setup complete!");
			#endif

			bool result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
			if (!result)
			{
				LOG_ERROR("Framebuffer is not complete!");
			}

			// Cleanup:
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindTexture(this->texTarget, 0);

			return result;
		}
		else
		{
			LOG_ERROR("Texture buffer failed. Render texture could not be buffered either");

			return false;
		}		
	}
	

	bool RenderTexture::BufferCubeMap(RenderTexture** cubeFaceRTs)
	{
		// NOTE: This function uses the paramters of the object its called from

		if (!Texture::BufferCubeMap((Texture * *)cubeFaceRTs))
		{
			return false;
		}
		
		// RenderTexture specific setup:
		LOG("Configuring cube map as RenderTexture: \"" + this->TexturePath() + "\"");

		// Generate faces:
		for (int i = 0; i < CUBE_MAP_COUNT; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->internalFormat, this->width, this->height, 0, this->format, this->type, NULL);
		}

		// Ensure all of the textures have the correct information stored in them:
		for (int i = 0; i < CUBE_MAP_COUNT; i++)
		{
			if (cubeFaceRTs[i] != this)
			{
				cubeFaceRTs[i]->textureID = this->textureID;
				cubeFaceRTs[i]->samplerID = this->samplerID;
				cubeFaceRTs[i]->frameBufferObject = this->frameBufferObject;
			}
		}

		// Bind framebuffer:
		bool result = true;
		glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferObject);
		if (!glIsFramebuffer(this->frameBufferObject))
		{
			glGenFramebuffers(1, &this->frameBufferObject);
			glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferObject);
			if (!glIsFramebuffer(this->frameBufferObject))
			{
				LOG_ERROR("Failed to create framebuffer object");
				return false;
			}

			// Attach framebuffer as a cube map render buffer:
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->textureID, 0);

			glDrawBuffer(this->drawBuffer);
			glReadBuffer(this->readBuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				LOG_ERROR("Cube map framebuffer is not complete!");
				result = false;
			}
		}
 
		// Cleanup:
		glBindTexture(this->texTarget, 0); // Was still bound from Texture::BufferCubeMap()
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 

		return result;
	}
}


