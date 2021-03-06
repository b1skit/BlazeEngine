// Blaze Engine texture object
// Contains everything needed to describe texture data

#include "Texture.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"
#include "Material.h"


#define STBI_FAILURE_USERMSG
#include "stb_image.h"				// STB image loader. No need to #define STB_IMAGE_IMPLEMENTATION, as it was already defined in SceneManager

#include <string>

using std::to_string;

#define ERROR_TEXTURE_NAME "RedErrorTexture"
#define DEFAULT_ALPHA 1.0f			// Default alpha value when loading texture data, if no alpha exists


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

		this->texturePath		= texturePath;

		// Initialize the texture:
		texels					= new vec4[numTexels];
		resolutionHasChanged	= true;
		if (doFill)
		{
			Fill(fillColor);
		}
	}


	// Copy constructor:
	Texture::Texture(Texture const& rhs)
	{
		// Cleanup:
		if (this->texels != nullptr)
		{
			delete[] texels;
			this->texels				= nullptr;
			this->numTexels				= 0;
			this->resolutionHasChanged	= true;
		}

		// Copy properties:
		this->textureID				= 0;	// NOTE: This could potentially result in some textures never being destroyed (ie. If this is called on a non-duplicated texture) TODO: Investigate this

		this->texTarget				= rhs.texTarget;
		this->format				= rhs.format;
		this->internalFormat		= rhs.internalFormat;
		this->type					= rhs.type;

		this->textureWrapS			= rhs.textureWrapS;
		this->textureWrapT			= rhs.textureWrapT;
		this->textureWrapR			= rhs.textureWrapR;

		this->textureMinFilter		= rhs.textureMinFilter;
		this->textureMaxFilter		= rhs.textureMaxFilter;

		this->samplerID				= rhs.samplerID;

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
		this->textureWrapR		= rhs.textureWrapR;

		this->textureMinFilter	= rhs.textureMinFilter;
		this->textureMaxFilter	= rhs.textureMaxFilter;

		this->samplerID			= rhs.samplerID;

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

		return texels[(v * this->width) + u]; // Number of elements in v rows, + uth element in next row
	}


	void BlazeEngine::Texture::Fill(vec4 color)
	{
		for (unsigned int row = 0; row < this->height; row++)
		{
			for (unsigned int col = 0; col < this->width; col++)
			{
				Texel(row, col) = color;
			}
		}
	}


	void BlazeEngine::Texture::Fill(vec4 tl, vec4 tr, vec4 bl, vec4 br)
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
	}



	// Static functions:
	//------------------

	Texture* Texture::LoadTextureFileFromPath(string texturePath, bool returnErrorTexIfNotFound /*= false*/, bool flipY /*= true*/)
	{
		stbi_set_flip_vertically_on_load(flipY);	// Set stb_image to flip the y-axis on loading to match OpenGL's style (So pixel (0,0) is in the bottom-left of the image)

		LOG("Attempting to load texture \"" + texturePath + "\"");

		int width, height, numChannels;
		void* imageData = nullptr;
		bool isHDR		= false;

		// Handle HDR images:
		if (stbi_is_hdr(texturePath.c_str()))
		{
			imageData	= stbi_loadf(texturePath.c_str(), &width, &height, &numChannels, 0);
			isHDR		= true;
		}
		else
		{
			imageData = stbi_load(texturePath.c_str(), &width, &height, &numChannels, 0);
		}
		

		if (imageData)
		{
			LOG("Found " + to_string(width) + "x" + to_string(height) + (isHDR?" HDR ":" LDR ") + "texture with " + to_string(numChannels) + " channels");

			Texture* texture = new Texture(width, height, texturePath, false);

			if (isHDR)
			{
				float* castImageData = static_cast<float*>(imageData);
				LoadHDRHelper(*texture, castImageData, width, height, numChannels);
			}
			else
			{
				unsigned char* castImageData = static_cast<unsigned char*>(imageData);
				LoadLDRHelper(*texture, castImageData, width, height, numChannels);
			}

			// Cleanup:
			stbi_image_free(imageData);

			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Completed loading texture: " + texturePath);
			#endif

			return texture;
		}
		else if (!returnErrorTexIfNotFound)
		{
			return nullptr;
		}

		// If we've made it this far, we couldn't load an image from a file:
		char const* failResult = stbi_failure_reason();
		LOG_ERROR("Could not load texture at \"" + texturePath + "\", error: \"" + string(failResult) + ".\" Returning solid red color!");

		width = height = 1;
		Texture* texture = new Texture(width, height, ERROR_TEXTURE_NAME, true, TEXTURE_ERROR_COLOR_VEC4);

		return texture;
	}


	void Texture::LoadLDRHelper(Texture& targetTexture, const unsigned char* imageData, int width, int height, int numChannels)
	{
		// Read texel values:
		const unsigned char* currentElement = imageData;
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				vec4 currentPixel(0.0f, 0.0f, 0.0f, DEFAULT_ALPHA);

				for (int channel = 0; channel < numChannels; channel++)
				{
					currentPixel[channel] = (float)((float)((unsigned int)*currentElement) / 255.0f);
					currentElement++;
				}

				targetTexture.Texel(col, row) = currentPixel;
			}
		}
	}

	void Texture::LoadHDRHelper(Texture& targetTexture, const float* imageData, int width, int height, int numChannels)
	{
		// Read texel values:
		const float* currentElement = imageData;
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				vec4 currentPixel(0.0f, 0.0f, 0.0f, DEFAULT_ALPHA);

				for (int channel = 0; channel < numChannels; channel++)
				{
					currentPixel[channel] = *currentElement;

					currentElement++;
				}

				targetTexture.Texel(col, row) = currentPixel;
			}
		}		
	}


	bool Texture::Buffer(int textureUnit)
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
			glTexParameteri(this->texTarget, GL_TEXTURE_MIN_FILTER, this->textureMinFilter);
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

				// Compute storage properties for our texture:
				int largestDimension = glm::max(this->width, this->height);
				int numMipLevels = (int)glm::log2((float)largestDimension) + 1;

				glTexStorage2D(this->texTarget, numMipLevels, this->internalFormat, this->width, this->height);

				resolutionHasChanged = false;
			}

			glTexSubImage2D(this->texTarget, 0, 0, 0, this->width, this->height, this->format, this->type, &this->Texel(0, 0).r);
			//glTexImage2D(this->texTarget, 0, this->internalFormat, this->width, this->height, 0, this->format, this->type, &this->Texel(0, 0).r); // Won't work if glTexStorage2D has been called

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
		glBindSampler(textureUnit, this->samplerID);
		if (!glIsSampler(this->samplerID))
		{
			glGenSamplers(1, &this->samplerID);
			glBindSampler(textureUnit, this->samplerID);
		}

		glSamplerParameteri(this->samplerID, GL_TEXTURE_WRAP_S, this->textureWrapS);
		glSamplerParameteri(this->samplerID, GL_TEXTURE_WRAP_T, this->textureWrapT);				

		glSamplerParameteri(this->samplerID, GL_TEXTURE_MIN_FILTER, this->textureMinFilter);
		glSamplerParameteri(this->samplerID, GL_TEXTURE_MAG_FILTER, this->textureMaxFilter);

		glBindSampler(textureUnit, 0);

		return true;
	}


	bool Texture::BufferCubeMap(Texture** cubeFaces, int textureUnit) // Note: There must be EXACTLY 6 elements in cubeFaces
	{
		// NOTE: This function uses the paramters of cubeFaces[0]

		LOG("Buffering cube map: \"" + cubeFaces[0]->TexturePath() + "\"");

		// Bind Texture:
		glBindTexture(cubeFaces[0]->texTarget, cubeFaces[0]->textureID);
		if (!glIsTexture(cubeFaces[0]->textureID))
		{
			glGenTextures(1, &cubeFaces[0]->textureID);
			glBindTexture(cubeFaces[0]->texTarget, cubeFaces[0]->textureID);

			if (!glIsTexture(cubeFaces[0]->textureID))
			{
				LOG_ERROR("OpenGL failed to generate new cube map texture name. Texture buffering failed");
				glBindTexture(cubeFaces[0]->texTarget, 0);
				return false;
			}
		}

		// Set texture params:
		glTexParameteri(cubeFaces[0]->texTarget, GL_TEXTURE_WRAP_S, cubeFaces[0]->textureWrapS);
		glTexParameteri(cubeFaces[0]->texTarget, GL_TEXTURE_WRAP_T, cubeFaces[0]->textureWrapT);
		glTexParameteri(cubeFaces[0]->texTarget, GL_TEXTURE_WRAP_R, cubeFaces[0]->textureWrapR);

		glTexParameteri(cubeFaces[0]->texTarget, GL_TEXTURE_MAG_FILTER, cubeFaces[0]->textureMaxFilter);
		glTexParameteri(cubeFaces[0]->texTarget, GL_TEXTURE_MIN_FILTER, cubeFaces[0]->textureMinFilter);

		// Bind sampler:
		glBindSampler(textureUnit, cubeFaces[0]->samplerID);
		if (!glIsSampler(cubeFaces[0]->samplerID))
		{
			glGenSamplers(1, &cubeFaces[0]->samplerID);
			glBindSampler(textureUnit, cubeFaces[0]->samplerID);

			if (!glIsSampler(cubeFaces[0]->samplerID))
			{
				LOG_ERROR("Could not create cube map sampler");
				return false;
			}
		}

		// Set sampler params:
		glSamplerParameteri(cubeFaces[0]->samplerID, GL_TEXTURE_WRAP_S, cubeFaces[0]->textureWrapS);
		glSamplerParameteri(cubeFaces[0]->samplerID, GL_TEXTURE_WRAP_T, cubeFaces[0]->textureWrapT);

		glSamplerParameteri(cubeFaces[0]->samplerID, GL_TEXTURE_MIN_FILTER, cubeFaces[0]->textureMinFilter);
		glSamplerParameteri(cubeFaces[0]->samplerID, GL_TEXTURE_MAG_FILTER, cubeFaces[0]->textureMaxFilter);

		glBindSampler(textureUnit, 0);


		// Texture cube map specific setup:
		if (cubeFaces[0]->texels != nullptr)
		{
			// Generate faces:
			for (int i = 0; i < CUBE_MAP_NUM_FACES; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, cubeFaces[0]->internalFormat, cubeFaces[0]->width, cubeFaces[0]->height, 0, cubeFaces[0]->format, cubeFaces[0]->type, &cubeFaces[i]->Texel(0, 0).r);
			}


			// Ensure all of the textures have the correct information stored in them:
			for (int i = 1; i < CUBE_MAP_NUM_FACES; i++)
			{
				cubeFaces[i]->textureID = cubeFaces[0]->textureID;
				cubeFaces[i]->samplerID = cubeFaces[0]->samplerID;
			}

			// Cleanup:
			glBindTexture(cubeFaces[0]->texTarget, 0); // Otherwise, we leave the texture bound for the remaining RenderTexture BufferCubeMap()
		}

		return true;
	}


	void Texture::Bind(int textureUnit, bool doBind)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);

		// Handle unbinding:
		if (doBind == false)
		{
			glBindTexture(this->texTarget, 0);
			glBindSampler(textureUnit, 0); // Assign to index/unit 0

		}
		else // Handle binding:
		{			
			glBindTexture(this->texTarget, this->textureID);
			glBindSampler(textureUnit, this->samplerID); // Assign our named sampler to the texture
		}
	}


	vec4 Texture::TexelSize()
	{
		// Check: Have the dimensions changed vs what we have cached?
		if (this->texelSize.z != this->width || this->texelSize.w != this->height)
		{
			this->texelSize = vec4(1.0f / this->width, 1.0f / this->height, this->width, this->height);
		}

		return this->texelSize;
	}


	void Texture::GenerateMipMaps()
	{
		glBindTexture(this->texTarget, this->textureID);
		glGenerateMipmap(this->texTarget);
		glBindTexture(this->texTarget, 0);
	}
}


