// Blaze Engine texture object
// Contains everything needed to describe texture data

#include "Texture.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#define STBI_FAILURE_USERMSG
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
		
		Fill(TEXTURE_ERROR_COLOR_VEC4, false);
	}

	// Constructor:
	Texture::Texture(int width, int height, string texturePath, bool doFill /* = true */, vec4 fillColor /* = (1.0, 0.0, 0.0, 1.0) */, bool doBuffer /*= false*/, int textureUnit /*= -1*/)
	{
		this->width				= width;
		this->height			= height;
		this->numTexels			= width * height;

		this->texturePath		= texturePath;

		// Initialize the texture:
		texels					= new vec4[numTexels];
		resolutionHasChanged	= true;
		if (doFill)
		{
			Fill(fillColor);
		}

		if (textureUnit != -1)
		{
			this->textureUnit = textureUnit;
		}

		if (doBuffer)
		{
			if (textureUnit == -1)
			{
				LOG_ERROR("Cannot buffer texture \"" + texturePath + "\" before the textureUnit has been set");
			}
			else
			{
				this->Buffer();
			}
		}
	}

	Texture::Texture(Texture const& rhs)
	{
		// Cleanup:
		if (this->texels != nullptr)
		{
			delete[] texels;
			texels = nullptr;
			numTexels = 0;
			resolutionHasChanged = true;
		}

		// Copy properties:
		this->textureID				= 0;	// NOTE: This could potentially result in some textures never being destroyed (ie. If this is called on a non-duplicated texture) TODO: Investigate this

		this->texTarget				= rhs.texTarget;
		this->format				= rhs.format;
		this->internalFormat		= rhs.internalFormat;
		this->type					= rhs.type;

		this->textureWrapS			= rhs.textureWrapS;
		this->textureWrapT			= rhs.textureWrapT;

		this->textureMinFilter		= rhs.textureMinFilter;
		this->textureMaxFilter		= rhs.textureMaxFilter;

		this->samplerID				= rhs.samplerID;
		this->textureUnit			= rhs.textureUnit;

		this->width					= rhs.width;
		this->height				= rhs.height;

		this->numTexels				= rhs.numTexels;

		if (rhs.texels != nullptr && this->numTexels > 0)
		{
			this->texels				= new vec4[this->numTexels];
			this->resolutionHasChanged	= true;

			for (unsigned int i = 0; i < this->numTexels; i++)
			{
				this->texels[i] = rhs.texels[i];
			}
		}

		this->texturePath = rhs.texturePath;
	}


	void Texture::Destroy()
	{
		if (glIsTexture(textureID))
		{
			glDeleteTextures(1, &textureID);
		}

		if (texels != nullptr)
		{
			delete[] texels;
			texels = nullptr;
			numTexels = 0;
			resolutionHasChanged = true;
		}

		glDeleteSamplers(1, &this->samplerID);
	}


	Texture& BlazeEngine::Texture::operator=(Texture const& rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		// Cleanup:
		if (this->texels != nullptr)
		{
			Destroy();
		}


		// Copy properties:
		this->textureID			= 0;	// NOTE: Texture.Buffer() must be called before this texture can be used

		this->texTarget			= rhs.texTarget;
		this->format			= rhs.format;
		this->internalFormat	= rhs.internalFormat;
		this->type				= rhs.type;

		this->textureWrapS		= rhs.textureWrapS;
		this->textureWrapT		= rhs.textureWrapT;

		this->textureMinFilter	= rhs.textureMinFilter;
		this->textureMaxFilter	= rhs.textureMaxFilter;


		this->width				= rhs.width;
		this->height			= rhs.height;

		this->numTexels			= rhs.numTexels;

		if (rhs.texels != nullptr && numTexels > 0)
		{
			this->texels				= new vec4[this->numTexels];
			this->resolutionHasChanged	= true;

			for (unsigned int i = 0; i < this->numTexels; i++)
			{
				this->texels[i] = rhs.texels[i];
			}
		}

		this->texturePath = rhs.texturePath;

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

	void BlazeEngine::Texture::Fill(vec4 color, bool doBuffer /*= false*/)
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

	void BlazeEngine::Texture::Fill(vec4 tl, vec4 tr, vec4 bl, vec4 br, bool doBuffer /*= false*/)
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

	Texture* Texture::LoadTextureFileFromPath(string texturePath, bool doBuffer /*= false*/)
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

			if (doBuffer && !texture->Buffer())
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
		LOG("Buffering texture: \"" + this->TexturePath() + "\"");

		glBindTexture(this->texTarget, this->textureID);

		// If the texture hasn't been created, create a new name:
		if (!glIsTexture(this->textureID))
		{
			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Texture has never been bound before!");
			#endif

			glGenTextures(1, &this->textureID);
			glBindTexture(this->texTarget, this->textureID);
			if (glIsTexture(this->textureID) != GL_TRUE)
			{
				LOG_ERROR("OpenGL failed to generate new texture name. Texture buffering failed");
				glBindTexture(this->texTarget, 0);
				return false;
			}

			// UV wrap mode:
			glTexParameteri(this->texTarget, GL_TEXTURE_WRAP_S, this->textureWrapS);	// u
			glTexParameteri(this->texTarget, GL_TEXTURE_WRAP_T, this->textureWrapT);	// v

			// Mip map min/maximizing:
			glTexParameteri(this->texTarget, GL_TEXTURE_MIN_FILTER, this->textureMinFilter);	// Linear interpolation, nearest neighbour sampling
			glTexParameteri(this->texTarget, GL_TEXTURE_MAG_FILTER, this->textureMaxFilter);
		}
		#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
		else
		{
			LOG("Found existing texture");
		}
		#endif


		// Upload to the GPU:
		if (texels != nullptr) // I.e. Texture:
		{
			if (resolutionHasChanged)
			{
				#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
					LOG("Buffering texture values");
				#endif

				// Specify storage properties for our texture:
				glTexStorage2D(this->texTarget, 1, this->internalFormat, (GLsizei)this->width, (GLsizei)this->height);

				resolutionHasChanged = false;
			}

			glTexSubImage2D(this->texTarget, 0, 0, 0, this->width, this->height, this->format, this->type, &this->Texel(0, 0).r);
			glGenerateMipmap(this->texTarget);

			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Texture buffering complete!");
			#endif

			// Cleanup:
			glBindTexture(this->texTarget, 0);
		}
		else // I.e. RenderTexture:
		{			
			if (resolutionHasChanged)	// We don't really care about the resolution of render textures changing, for now...
			{
				resolutionHasChanged = false;
			}
			glTexImage2D(this->texTarget, 0, this->internalFormat, this->width, this->height, 0, this->format, this->type, nullptr);

			// Note: We don't unbind the texture here so RenderTexture::Buffer() doesn't have to rebind it
		}

		// Configure the Texture sampler:
		if (this->textureUnit < 0)
		{
			LOG_ERROR("Cannot bind Texture sampler as textureUnit has not been set");
			return false;
		}
		glBindSampler(this->textureUnit, this->samplerID);
		if (!glIsSampler(this->samplerID))
		{
			glGenSamplers(1, &this->samplerID);
			glBindSampler(this->textureUnit, this->samplerID);
		}

		glSamplerParameteri(this->samplerID, GL_TEXTURE_WRAP_S, this->textureWrapS);
		glSamplerParameteri(this->samplerID, GL_TEXTURE_WRAP_T, this->textureWrapT);				

		glSamplerParameteri(this->samplerID, GL_TEXTURE_MIN_FILTER, this->textureMinFilter);
		glSamplerParameteri(this->samplerID, GL_TEXTURE_MAG_FILTER, this->textureMaxFilter);

		glBindSampler(this->textureUnit, 0);

		return true;
	}


	void Texture::Bind(GLuint const& shaderReference /*= 0*/, int textureUnitOverride /*= -1*/)
	{
		int targetTextureUnit = this->textureUnit;
		if (textureUnitOverride != -1)
		{
			targetTextureUnit = textureUnitOverride;
		}

		// Handle unbinding:
		if (shaderReference == 0)
		{
			glActiveTexture(GL_TEXTURE0 + targetTextureUnit);
			glBindTexture(this->texTarget, 0);
			glBindSampler(targetTextureUnit, 0); // Assign to index/unit 0

		}
		else // Handle binding:
		{
			glActiveTexture(GL_TEXTURE0 + targetTextureUnit);
			glBindTexture(this->texTarget, this->textureID);
			glBindSampler(targetTextureUnit, this->samplerID); // Assign our named sampler to the texture
		}
	}
}


