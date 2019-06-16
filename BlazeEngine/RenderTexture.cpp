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


	RenderTexture::RenderTexture(int width, int height, string name /*= DEFAULT_RENDERTEXTURE_NAME*/, bool doBuffer /*= false*/)
	{
		this->width					= CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapWidth;
		this->height				= CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapHeight;
		numTexels					= width * height;

		this->texturePath			= name;

		this->texels				= nullptr;
		this->resolutionHasChanged	= true;


		// Override default values:
		//-------------------------
		this->internalFormat		= GL_DEPTH_COMPONENT;
		this->format				= GL_DEPTH_COMPONENT;
		this->type					= GL_FLOAT;
		
		this->textureWrapS			= GL_CLAMP_TO_EDGE;
		this->textureWrapT			= GL_CLAMP_TO_EDGE;

		this->textureMinFilter		= GL_NEAREST;
		this->textureMaxFilter		= GL_NEAREST;

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

		this->frameBufferObject = rhs.frameBufferObject;

		this->attachmentPoint = rhs.attachmentPoint;

		this->drawBuffer = rhs.drawBuffer;
		this->readBuffer = rhs.readBuffer;

		return *this;
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
				/*glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_LAYERS, );
				glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, );
				glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS, );*/

				// Attach our texture to the framebuffer as a render buffer:
				glFramebufferTexture2D(GL_FRAMEBUFFER, this->attachmentPoint, this->target, this->textureID, 0);
			}

			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Render texture setup complete!");
			#endif

			// Cleanup:
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindTexture(this->target, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				LOG_ERROR("Framebuffer is not complete!");
				return false;
			}

			return true;
		}
		else
		{
			LOG_ERROR("Texture buffer failed. Render texture could not be buffered either");

			return false;
		}		
	}
}


