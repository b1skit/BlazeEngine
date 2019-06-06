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

		if (name != DEFAULT_RENDERTEXTURE_NAME)
		{
			this->texturePath = name;
		}
		else
		{
			this->texturePath = name;
		}		

		this->texels				= nullptr;
		this->resolutionHasChanged	= true;


		// Override default values:
		//-------------------------
		this->internalFormat		= GL_DEPTH_COMPONENT;
		this->format				= GL_DEPTH_COMPONENT;
		this->type					= GL_FLOAT;
		
		this->textureWrapS			= GL_REPEAT;
		this->textureWrapT			= GL_REPEAT;

		this->textureMinFilter		= GL_NEAREST;
		this->textureMaxFilter		= GL_NEAREST;

		if (doBuffer)
		{
			Buffer();
		}
	}


	void RenderTexture::Destroy()
	{
		Texture::Destroy();

		glDeleteFramebuffers(1, &frameBufferObject);
	}


	bool RenderTexture::Buffer()
	{
		if (Texture::Buffer())
		{
			if (!glIsFramebuffer(frameBufferObject))
			{
				glGenFramebuffers(1, &frameBufferObject);

				glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

				if (!glIsFramebuffer(frameBufferObject))
				{
					LOG_ERROR("Failed to create framebuffer object");
					return false;
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, this->attachmentPoint, this->target, this->textureID, 0);
			}
			else
			{
				// Attach the texture as the framebuffer's depth buffer:
				glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
			}


			//glDrawBuffer(GL_NONE); // TODO: Store these as a member variable?
			//glReadBuffer(GL_NONE);
			// ^^^^^THESE WILL NEED TO BE SET/UNSET, DEPENDING ON WHAT'S BEING RENDERED....


			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Render texture setup complete!");
			#endif

			// Cleanup:
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

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


