#pragma once

#include "Texture.h"

#include <GL/glew.h>
#include "glm.hpp"


#define DEFAULT_RENDERTEXTURE_NAME "UNNAMED_RenderTexture"

namespace BlazeEngine
{
	class RenderTexture : public Texture
	{
	public:
		RenderTexture();
		RenderTexture(int width, int height, string name = DEFAULT_RENDERTEXTURE_NAME, bool doBuffer = false, int textureUnit = -1);

		RenderTexture(RenderTexture const& rhs, bool doBuffer = false);

		RenderTexture& operator=(RenderTexture const& rhs);
		

		void AttachAdditionalRenderTexturesToFramebuffer(RenderTexture** additionalRTs, int numRTs, bool isDepth = false);

		
		GLuint& FBO()						{ return frameBufferObject; }

		GLuint& AttachmentPoint()			{ return attachmentPoint; }
		GLuint& DrawBuffer()				{ return drawBuffer; }
		GLuint& ReadBuffer()				{ return readBuffer; }

		vec4& ClearColor()					{ return clearColor; }

		void Destroy();

		// Configure GPU framebuffer object. Returns true if successful, false otherwise. Also calls Texture.Buffer()
		bool Buffer();

		// Configure GPU frambuffer object for cube maps
		bool BufferCubeMap(RenderTexture** cubeFaceRTs); // Note: There must be exactly 6 cubeFaceRTs


	protected:


	private:
		GLuint frameBufferObject	= 0;

		GLuint attachmentPoint		= GL_DEPTH_ATTACHMENT;

		GLuint drawBuffer			= GL_NONE;	// Which of the 4 color buffers should be drawn into for the DEFAULT framebuffer
		GLuint readBuffer			= GL_NONE;	// Which color buffer to use for subsequent reads

		vec4 clearColor				= vec4(0, 0, 0, 1);
	};
}


