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
		RenderTexture(int width, int height, string name = DEFAULT_RENDERTEXTURE_NAME, bool doBuffer = false);

		RenderTexture(RenderTexture const& rhs, bool doBuffer);

		RenderTexture& operator=(RenderTexture const& rhs);
		

		void AttachAdditionalRenderTexturesToFramebuffer(RenderTexture** additionalRTs, int numRTs, bool isDepth = false);

		
		GLuint& FBO()						{ return frameBufferObject; }

		GLuint& AttachmentPoint()			{ return attachmentPoint; }
		GLuint& DrawBuffer()				{ return drawBuffer; }
		GLuint& ReadBuffer()				{ return readBuffer; }

		void Destroy();

		// Configure GPU framebuffer object. Returns true if successful, false otherwise. Also calls Texture.Buffer()
		bool Buffer();

	protected:


	private:
		GLuint frameBufferObject	= 0;

		GLuint attachmentPoint		= GL_DEPTH_ATTACHMENT; // OR, GL_COLOR_ATTACHMENTi....?

		GLuint drawBuffer			= GL_NONE;	// Which of the 4 color buffers should be drawn into for the DEFAULT framebuffer
		GLuint readBuffer			= GL_NONE;	// Which color buffer to use for subsequent reads
		// ^^ OR, GL_COLOR_ATTACHMENTi....? (Same as the attachment point)
	};
}


