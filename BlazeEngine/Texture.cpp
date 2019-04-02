// Blaze Engine texture object
// Contains everything needed to describe texture data

#include "Texture.h"
#include "CoreEngine.h"

#define STBI_FAILURE_USERMSG
#include "stb_image.h"				// STB image loader. No need to #define STB_IMAGE_IMPLEMENTATION, as it was already defined in SceneManager


// Debug:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	// Constructor:
	Texture::Texture(int width, int height, bool doFill, vec4 fillColor) // doFill == true, fillColor = (1.0, 0, 0, 1) by default
	{
		this->width = width;
		this->height = height;
		this->numTexels = width * height;

		// Initialize the texture:
		texels = new vec4[numTexels];
		if (doFill)
		{
			Fill(fillColor);
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

	void BlazeEngine::Texture::Fill(vec4 color, bool doBuffer)	// doBuffer == true by default
	{
		for (unsigned int row = 0; row < this->height; row++)
		{
			for (unsigned int col = 0; col < this->width; col++)
			{
				Texel(row, col) = color;
			}
		}

		// Upload the texture to the GPU, if required
		if (doBuffer)
		{
			Buffer();
		}
	}

	void BlazeEngine::Texture::Fill(vec4 tl, vec4 tr, vec4 bl, vec4 br, bool doBuffer)	// doBuffer == true by default
	{
		for (unsigned int row = 0; row < height; row++)
		{
			float vertDelta = (float)((float)row / (float)height);
			vec4 startCol = (vertDelta * bl) + ((1.0f - vertDelta) * tl);
			vec4 endCol = (vertDelta * br) + ((1.0f - vertDelta) * tr);

			for (unsigned int col = 0; col < width; col++)
			{
				float horDelta = (float)((float)col / (float)width);

				Texel(row, col) = (horDelta * endCol) + ((1.0f - horDelta) * startCol);
			}
		}

		if (doBuffer)
		{
			Buffer();
		}
	}





	// Static functions:
	//------------------

	Texture* Texture::LoadTextureFromPath(string texturePath)
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Loading texture at " + texturePath) });

		int width, height, numChannels;
		unsigned char* imageData = stbi_load(texturePath.c_str(), &width, &height, &numChannels, 0);

		if (imageData)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, nullptr, new string("Attempting to load texture with " + to_string(numChannels) + " channels") });

			Texture* texture = new Texture(width, height);
			texture->texturePath = texturePath;

			// Read texture values:
			unsigned char* currentElement = imageData;
			for (int row = 0; row < width; row++)
			{
				for (int col = 0; col < height; col++)
				{
					vec4 currentPixel(0, 0, 0, 1);
					currentPixel.r = (float)((float)((unsigned int)*currentElement) / 255.0f);
					currentElement++;
					currentPixel.g = (float)((float)((unsigned int)*currentElement) / 255.0f);
					currentElement++;
					currentPixel.b = (float)((float)((unsigned int)*currentElement) / 255.0f);
					currentElement++;
					if (numChannels == 4)
					{
						currentPixel.a = (float)((float)((unsigned int)*currentElement) / 255.0f);
						currentElement++;
					}
					else
					{
						currentPixel.a = 1.0f;
					}
					texture->Texel(row, col) = currentPixel;						
				}
			}

			texture->Buffer();

			// Cleanup:
			stbi_image_free(imageData);

			return texture;
		}

		// If we've made it this far, we couldn't load an image from a file:
		char const* failResult = stbi_failure_reason();
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("Could not load texture at \"" + texturePath + "\", error: \"" + string(failResult) + ".\" Returning solid red color!") });

		width = height = 1;
		Texture* texture = new Texture(width, height);
		texture->texturePath = "errorTexture";
		texture->Fill(vec4(1.0, 0, 0, 1));
		texture->Buffer();

		return texture;
	}


	bool Texture::Buffer()
	{
		// If the texture hasn't been created, create a new name:
		if (!glIsTexture(this->textureID))
		{
			glGenTextures(1, &this->textureID);
			glBindTexture(this->target, this->textureID);
			if (glIsTexture(this->textureID) != GL_TRUE)
			{
				CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("OpenGL failed to create texture") });
				return false;
			}

			// Set the texture properties:
			glTexStorage2D(this->target, 1, this->internalFormat, (GLsizei)this->width, (GLsizei)this->height);
		}
		else
		{
			glBindTexture(this->target, this->textureID);
		}

		//glActiveTexture(0); // Set the active texture unit [0, 80+]
		// Don't think we need this if we're using glBindTexture?

		// Upload to the GPU:
		glTexSubImage2D(this->target, 0, 0, 0, this->width, this->height, this->format, this->type, &this->Texel(0, 0).r);

		// Cleanup:
		glBindTexture(this->target, 0);

		return true;
	}
}


