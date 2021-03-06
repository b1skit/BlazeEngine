#pragma once

#include "Texture.h"	// Base class

#include <GL/glew.h>
#include "glm.hpp"


#define DEFAULT_RENDERTEXTURE_NAME "UNNAMED_RenderTexture"


namespace BlazeEngine
{
	class RenderTexture : public Texture
	{
	public:
		RenderTexture();
		RenderTexture(int width, int height, string name = DEFAULT_RENDERTEXTURE_NAME);

		RenderTexture(RenderTexture const& rhs);

		RenderTexture& operator=(RenderTexture const& rhs);
		

		void AttachAdditionalRenderTexturesToFramebuffer(RenderTexture** additionalRTs, int numRTs, bool isDepth = false);

		
		GLuint& FBO()						{ return frameBufferObject; }

		GLuint& AttachmentPoint()			{ return attachmentPoint; }
		GLuint& DrawBuffer()				{ return drawBuffer; }
		GLuint& ReadBuffer()				{ return readBuffer; }

		vec4& ClearColor()					{ return clearColor; }

		void Destroy();

		// Configure GPU framebuffer object. Returns true if successful, false otherwise. Also calls Texture.Buffer()
		bool Buffer(int textureUnit);

		// Configure GPU frambuffer object for cube maps
		static bool BufferCubeMap(RenderTexture** cubeFaceRTs, int textureUnit); // Note: There must be exactly 6 cubeFaceRTs

		// Helper function: Create an array of 6 cube map textures (for a depth map by default). Must be configured/buffered before use
		static RenderTexture** CreateCubeMap(int xRes, int yRes, string name = "UNNAMMED");

		// Frame buffers helper functions:
		void BindFramebuffer(bool doBind);
		void AttachToFramebuffer(GLenum textureTarget, int mipLevel = 0);

		// Render buffer helper functions:
		void CreateRenderbuffer(bool leaveBound = true, int xRes = -1, int yRes = -1);
		void BindRenderbuffer(bool doBind);
		void DeleteRenderbuffer(bool unbind = true);


	protected:


	private:
		GLuint frameBufferObject	= 0;

		GLuint attachmentPoint		= GL_DEPTH_ATTACHMENT;

		GLuint drawBuffer			= GL_NONE;	// Which of the 4 color buffers should be drawn into for the DEFAULT framebuffer
		GLuint readBuffer			= GL_NONE;	// Which color buffer to use for subsequent reads

		vec4 clearColor				= vec4(0, 0, 0, 1);
	};
}


