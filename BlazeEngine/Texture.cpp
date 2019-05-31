// Blaze Engine texture object
// Contains everything needed to describe texture data

#include "Texture.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#define STBI_FAILURE_USERMSG		// TO DO: Move this to BuildConfiguration.h ???
#include "stb_image.h"				// STB image loader. No need to #define STB_IMAGE_IMPLEMENTATION, as it was already defined in SceneManager

#include <string>

using std::to_string;

#define ERROR_TEXTURE_NAME "RedErrorTexture"


namespace BlazeEngine
{
	Texture::Texture()
	{
		numTexels				= width * height;
		texels					= new vec4[numTexels];	// Allocate the default size
		resolutionHasChanged	= true;
		
		Fill(TEXTURE_ERROR_COLOR_VEC4);
	}

	// Constructor:
	Texture::Texture(int width, int height, string texturePath, bool doFill /* = true */, vec4 fillColor /* = (1.0, 0.0, 0.0, 1.0) */)
	{
		this->width				= width;
		this->height			= height;
		this->numTexels			= width * height;

		this->texturePath = texturePath;

		// Initialize the texture:
		texels					= new vec4[numTexels];
		resolutionHasChanged	= true;
		if (doFill)
		{
			Fill(fillColor);
		}		
	}


	void Texture::Destroy()
	{
		if (glIsTexture(textureID))
		{
			glDeleteTextures(1, &textureID);
		}
		if (texels)
		{
			delete[] texels;
			numTexels = 0;
			resolutionHasChanged = true;
		}
	}


	Texture& BlazeEngine::Texture::operator=(Texture const& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		this->width				= rhs.width;
		this->height			= rhs.height;
		this->textureID			= rhs.textureID;
		this->numTexels			= rhs.numTexels;

		if (this->texels != nullptr)
		{
			delete[] texels;
			numTexels = 0;
		}

		this->texels			= new vec4[this->numTexels];
		resolutionHasChanged	= true;
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
			LOG_ERROR("Invalid texture access! Cannot access texel (" + to_string(u) + ", " + to_string(v) + " in a texture of size " + to_string(width) + "x" + to_string(height));

			// Try and return the safest option:
			return texels[0];
		}

		return texels[(v * height) + u];
	}

	void BlazeEngine::Texture::Fill(vec4 color, bool doBuffer /*= true*/)
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

	void BlazeEngine::Texture::Fill(vec4 tl, vec4 tr, vec4 bl, vec4 br, bool doBuffer /*= true*/)
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
		LOG("Loading texture at " + texturePath);

		int width, height, numChannels;
		unsigned char* imageData = stbi_load(texturePath.c_str(), &width, &height, &numChannels, 0);

		if (imageData)
		{
			LOG("Attempting to load " + to_string(width) + "x" + to_string(height) + " texture with " + to_string(numChannels) + " channels");

			Texture* texture = new Texture(width, height, texturePath, false);

			// Read texture values:
			unsigned char* currentElement = imageData;
			for (int row = 0; row < height; row++)
			{
				for (int col = 0; col < width; col++)
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
		
					texture->Texel(col, row) = currentPixel;
				}
			}

			if (!texture->Buffer())
			{
				LOG_ERROR("Texture buffering failed");
			}

			// Cleanup:
			stbi_image_free(imageData);

			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Completed loading texture: " + texturePath);
			#endif

			return texture;
		}

		// If we've made it this far, we couldn't load an image from a file:
		char const* failResult = stbi_failure_reason();
		LOG_ERROR("Could not load texture at \"" + texturePath + "\", error: \"" + string(failResult) + ".\" Returning solid red color!");

		width = height = 1;
		Texture* texture = new Texture(width, height, ERROR_TEXTURE_NAME, true, TEXTURE_ERROR_COLOR_VEC4);

		return texture;
	}


	bool Texture::Buffer()
	{
		LOG("Buffering texture...");

		glBindTexture(this->target, this->textureID);

		// If the texture hasn't been created, create a new name:
		if (!glIsTexture(this->textureID))
		{
			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Texture has never been bound before!");
			#endif

			glGenTextures(1, &this->textureID);
			glBindTexture(this->target, this->textureID);
			if (glIsTexture(this->textureID) != GL_TRUE)
			{
				LOG_ERROR("OpenGL failed to generate new texture name. Texture buffering failed");
				return false;
			}

			//// Specify a 2D image:
			//glTexImage2D(this->target, 0, this->internalFormat, this->width, this->height, 0, this->format, this->type, &this->Texel(0,0).r);

			//// Set the texture properties:
			//glTexStorage2D(this->target, 1, this->internalFormat, (GLsizei)this->width, (GLsizei)this->height);

			//resolutionHasChanged = false;

			// UV wrap mode:
			glTexParameteri(this->target, GL_TEXTURE_WRAP_S, GL_REPEAT);	// u
			glTexParameteri(this->target, GL_TEXTURE_WRAP_T, GL_REPEAT);	// v

			// Mip map min/maximizing:
			glTexParameteri(this->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);	// Linear interpolation, nearest neighbour sampling
			glTexParameteri(this->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
			else
			{
				LOG("Updating existing texture");
			}
		#endif

		if (resolutionHasChanged)
		{
			// Specify a 2D image:
			glTexImage2D(this->target, 0, this->internalFormat, this->width, this->height, 0, this->format, this->type, &this->Texel(0, 0).r);

			// Set the texture properties:
			glTexStorage2D(this->target, 1, this->internalFormat, (GLsizei)this->width, (GLsizei)this->height);

			resolutionHasChanged = false;
		}

		// Upload to the GPU:
		glTexSubImage2D(this->target, 0, 0, 0, this->width, this->height, this->format, this->type, &this->Texel(0, 0).r); // <-- GL_INVALID_ENUM ???
		glGenerateMipmap(this->target);

		// Cleanup:
		glBindTexture(this->target, 0);

		#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
			LOG("Texture buffering complete!");
		#endif

		return true;
	}
}


