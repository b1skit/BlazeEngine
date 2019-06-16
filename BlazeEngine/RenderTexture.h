#pragma once

#include "Texture.h"

#define DEFAULT_RENDERTEXTURE_NAME "UNNAMED_RenderTexture"

namespace BlazeEngine
{
	class RenderTexture : public Texture
	{
	public:
		RenderTexture();
		RenderTexture(int width, int height, string name = DEFAULT_RENDERTEXTURE_NAME, bool doBuffer = false);

		RenderTexture& operator=(RenderTexture const& rhs);
		

		// TODO: COPY CONSTRUCTOR (INVOKE SUPERCLASS COPY CONSTRUCTOR!!!!!!!!!!!!!!!!!)

		
		GLuint FBO() const &			{ return frameBufferObject; }

		GLuint DrawBuffer() const&		{ return drawBuffer; }
		GLuint ReadBuffer() const&		{ return readBuffer; }
		GLuint AttachmentPoint() const&	{ return attachmentPoint; }


		void Destroy();

		// Configure GPU framebuffer object. Returns true if successful, false otherwise. Also calls Texture.Buffer()
		bool Buffer();

	protected:


	private:
		GLuint frameBufferObject	= 0;

		GLuint attachmentPoint		= GL_DEPTH_ATTACHMENT;

		GLuint drawBuffer			= GL_NONE;	// Which of the 4 color buffers should be drawn into for the DEFAULT framebuffer
		GLuint readBuffer			= GL_NONE;	// Which color buffer to use for subsequent reads
	};
}


