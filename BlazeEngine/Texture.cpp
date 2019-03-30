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

		return *this;
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

	void BlazeEngine::Texture::Fill(vec4 color, bool sendToGPU)
	{
		for (unsigned int row = 0; row < this->height; row++)
		{
			for (unsigned int col = 0; col < this->width; col++)
			{
				Texel(row, col) = color;
			}
		}

		// Upload the texture to the GPU, if required
		if (sendToGPU)
		{
			glBindTexture(target, textureID);
			glTexSubImage2D(this->target, 0, 0, 0, this->width, this->height, this->format, this->type, &this->Texel(0, 0).r);
			glBindTexture(target, 0);
		}
	}

	Texture* Texture::LoadTextureFromPath(string texturePath)
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Loading texture at " + texturePath) });

		int width = 256;
		int height = 256;

		Texture* texture = new Texture(width, height);

		// Create and bind the texture:
		glGenTextures(1, &texture->textureID);
		glBindTexture(texture->target, texture->textureID);
		if (glIsTexture(texture->textureID) != GL_TRUE)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("OpenGL failed to create texture. Returning null") });
			return nullptr;
		}	

		glTexStorage2D(texture->target, 1, texture->internalFormat, (GLsizei)texture->width, (GLsizei)texture->height);

		//glActiveTexture(0); // Set the active texture unit [0, 80+]
		// Don't think we need this if we're using glBindTexture?

		// Upload to the GPU:
		glTexSubImage2D(texture->target, 0, 0, 0, texture->width, texture->height, texture->format, texture->type, &texture->Texel(0, 0).r);
		

		// Cleanup:
		glBindTexture(texture->target, 0);

		return texture;
	}
}


