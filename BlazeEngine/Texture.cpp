// Blaze Engine texture object
// Contains everything needed to describe texture data

#include "Texture.h"
#include "CoreEngine.h"

// Debug:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	Texture::Texture(int width, int height)
	{
		this->width = width;
		this->height = height;

		texels = new vec4*[height];
		for (int row = 0; row < height; row++)
		{
			texels[row] = new vec4[width];
		}

		// DEBUG: Fill texels with generated values
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, nullptr, new string("Filling texture with dummy values!") });

		// Simple debug: Fill with 100% red
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				texels[row][col] = vec4(1, 0, 0, 1);
			}
		}

		//// Fill with an interpolated gradient:
		//vec4 tl(1, 1, 1, 1);
		//vec4 tr(0, 0, 1, 1);
		//vec4 bl(0, 1, 0, 1);
		//vec4 br(1, 0, 0, 1);
		//for (int row = 0; row < height; row++)
		//{
		//	float vertDelta = (float)((float)row / (float)height);
		//	vec4 startCol = (vertDelta * bl) + ((1.0f - vertDelta) * tl);
		//	vec4 endCol = (vertDelta * br) + ((1.0f - vertDelta) * tr);

		//	for (int col = 0; col < width; col++)
		//	{
		//		float horDelta = (float)((float)col / (float)width);

		//		texels[row][col] = (horDelta * endCol) + ((1.0f - horDelta) * startCol);
		//	}
		//}
	}


	Texture::~Texture()
	{
		glDeleteTextures(1, &textureID);

		if (texels)
		{
			for (unsigned int row = 0; row < height; row++)
			{
				if (texels[row])
				{
					delete texels[row];
				}				
			}
			delete [] texels;
		}
	}


	Texture* Texture::LoadTextureFromPath(string texturePath)
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Loading texture at " + texturePath) });

		int width = 256;
		int height = 256;

		Texture* texture = new Texture(width, height);

		GLuint textureID;
		//glGenTextures(1, &textureID);
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
		if (!glIsTexture(textureID))
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("OpenGL failed to create texture. Returning null") });
			return nullptr;
		}

		texture->SetTextureID(textureID);

		// Bind the texture, and specify its storage details:
		glBindTexture(GL_TEXTURE_2D, textureID);
		/*glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, (GLsizei)width, (GLsizei)height);*/
		glTextureStorage2D(textureID, 1, GL_RGBA32F, (GLsizei)width, (GLsizei)height);

		// Cleanup:
		glBindTexture(GL_TEXTURE_2D, 0);		

		return texture;
	}
}


