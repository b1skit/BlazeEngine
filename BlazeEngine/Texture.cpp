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
	// Constructor:
	Texture::Texture(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->numTexels = width * height;

		// Initialize the texture:
		texels = new vec4[numTexels];
		for (unsigned int i = 0; i < numTexels; i++)
		{
			texels[i] = vec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
		}


		// DEBUG: Fill texels with generated values
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, nullptr, new string("Filling texture with dummy values!") });

		// Fill with an interpolated gradient:
		vec4 tl(1, 1, 1, 1);
		vec4 tr(0, 0, 1, 1);
		vec4 bl(0, 1, 0, 1);
		vec4 br(1, 0, 0, 1);
		for (int row = 0; row < height; row++)
		{
			float vertDelta = (float)((float)row / (float)height);
			vec4 startCol = (vertDelta * bl) + ((1.0f - vertDelta) * tl);
			vec4 endCol = (vertDelta * br) + ((1.0f - vertDelta) * tr);

			for (int col = 0; col < width; col++)
			{
				float horDelta = (float)((float)col / (float)width);

				Texel(row, col) = (horDelta * endCol) + ((1.0f - horDelta) * startCol);
			}
		}
	}


	Texture::~Texture()
	{
		glDeleteTextures(1, &textureID);

		if (texels)
		{
			delete[] texels;
		}
	}

	Texture& BlazeEngine::Texture::operator=(Texture const& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		this->width		= rhs.width;
		this->height	= rhs.height;
		this->textureID = rhs.textureID;
		this->numTexels = rhs.numTexels;

		if (this->texels != nullptr)
		{
			delete[] texels;
		}

		this->texels = new vec4[this->numTexels];
		for (unsigned int i = 0; i < this->numTexels; i++)
		{
			this->texels[i] = rhs.texels[i];
		}
	}


	vec4& BlazeEngine::Texture::Texel(unsigned int u, unsigned int v)
	{
		if (u >= width || v >= height)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("Invalid texture access! Cannot access texel (" + to_string(u) + ", " + to_string(v) + " in a texture of size " + to_string(width) + "x" + to_string(height) ) });

			// Try and return the safest option:
			return texels[0];
		}

		return texels[(u * width) + v];
	}

	Texture* Texture::LoadTextureFromPath(string texturePath)
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Loading texture at " + texturePath) });

		int width = 256;
		int height = 256;

		Texture* texture = new Texture(width, height);

		glCreateTextures(texture->target, 1, &texture->textureID);
		if (!glIsTexture(texture->textureID))
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("OpenGL failed to create texture. Returning null") });
			return nullptr;
		}

		// Bind the texture, and specify its storage details:
		glBindTexture(texture->target, texture->textureID);
		/*glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, (GLsizei)width, (GLsizei)height);*/
		glTextureStorage2D(texture->textureID, 1, texture->internalFormat, (GLsizei)texture->width, (GLsizei)texture->height);

		// Upload to the GPU:
		glTexSubImage2D(texture->target, 0, 0, 0, texture->width, texture->height, texture->format, texture->type, &texture->Texel(0, 0).r);

		// Cleanup:
		glBindTexture(texture->target, 0);

		return texture;
	}
}


