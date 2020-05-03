#pragma once

#include <GL/glew.h>
#include "glm.hpp"

#include <string>

using glm::vec4;
using std::string;


#define TEXTURE_ERROR_COLOR_VEC4	vec4(1.0f, 0.0f, 0.0f, 1.0f)


namespace BlazeEngine
{
	class Texture
	{
	public:
		Texture();
		Texture(int width, int height, string texturePath, bool doFill = true, vec4 fillColor = TEXTURE_ERROR_COLOR_VEC4);

		Texture(Texture const& rhs);

		void Destroy();	// Destroys this object. Typically called by the SceneManager

		Texture& operator=(Texture const& rhs);

		// Getters/Setters:
		inline unsigned int const&	Width() const		{ return width; }
		inline unsigned int const&	Height() const		{ return height; }

		// OpenGL Property getters/setters:
		inline GLuint const&		TextureID() const	{ return textureID; }

		inline GLenum&				TextureTarget()		{ return texTarget; }
		inline GLenum&				Format()			{ return format; }
		inline GLenum&				InternalFormat()	{ return internalFormat; }
		inline GLenum&				Type()				{ return type; }

		inline GLenum&				TextureWrap_S()		{ return textureWrapS; }
		inline GLenum&				TextureWrap_T()		{ return textureWrapT; }
		inline GLenum&				TextureWrap_R()		{ return textureWrapR; }

		inline GLenum&				TextureMinFilter()	{ return textureMinFilter; }
		inline GLenum&				TextureMaxFilter()	{ return textureMaxFilter; }

		inline GLuint&				Sampler()			{ return samplerID; }

		inline string&				TexturePath()		{ return texturePath; }

		// Get/set a texel value:
		// Returns texels[0] if u = [0, width - 1], v = [0, height - 1] are out of localBounds.
		vec4& Texel(unsigned int u, unsigned int v); // u == x == col, v == y == row

		// Fill texture with a solid color
		void Fill(vec4 color);

		// Fill texture with a color gradient
		void Fill(vec4 tl, vec4 bl, vec4 tr, vec4 br); 

		// Initialization:
		bool Buffer(int textureUnit);	// Upload a texture to the GPU. Returns true if successful, false otherwise

		// Bind the texture to its sampler for Shader sampling
		void Bind(int textureUnit, bool doBind); // NOTE: GL_TEXTURE0 + textureUnit is what is bound when calling glActiveTexture()

		vec4 TexelSize();

		// Generate mip maps for the texture:
		void GenerateMipMaps();


		// Public static functions:
		//-------------------------
		
		// Configure GPU frambuffer object for cube maps
		static bool BufferCubeMap(Texture** cubeFaces, int textureUnit); // Note: There must be EXACTLY 6 elements in cubeFaces

		// Load a texture object from a (relative) path. Returns nullptr if OpenGL binding fails
		// NOTE: Use SceneManager::FindLoadTextureByPath() instead of accessing this function directly, to ensure duplicate textures can be shared
		static Texture* LoadTextureFileFromPath(string texturePath, bool returnErrorTexIfNotFound = true, bool flipY = true);


	protected:
		GLuint textureID			= 0;
		
		GLenum texTarget			= GL_TEXTURE_2D;
		GLenum format				= GL_RGBA;		// NOTE: Currently, BlazeEngine assumes all textures contain 4-channel vec4's (except for depth). If format != GL_RGBA, buffer will be packed with the wrong stride
		GLenum internalFormat		= GL_RGBA32F;
		GLenum type					= GL_FLOAT;

		GLenum textureWrapS			= GL_REPEAT;
		GLenum textureWrapT			= GL_REPEAT;
		GLenum textureWrapR			= GL_REPEAT;

		GLenum textureMinFilter		= GL_NEAREST_MIPMAP_LINEAR;
		GLenum textureMaxFilter		= GL_LINEAR;

		GLuint samplerID			= 0;		// Name of a sampler

		unsigned int	width		= 1;		// # Cols
		unsigned int	height		= 1;		// # Rows

		vec4*			texels		= nullptr;
		unsigned int	numTexels	= 0;

		vec4 texelSize				= vec4(-1, -1, -1, -1);

		string texturePath			= "Uninitialized_Texture";


		bool resolutionHasChanged	= false; // Does OpenGL need to be notified of new texture dimensions the next time Buffer() is called?

	private:	

		// Private static functions:
		//--------------------------

		// Helper functions for loading Low/High Dynamic Range image formats in LoadTextureFileFromPath()
		// Note: targetTexture and imageData must be valid
		static void LoadLDRHelper(Texture& targetTexture, const unsigned char* imageData, int width, int height, int numChannels);
		static void LoadHDRHelper(Texture& targetTexture, const float* imageData, int width, int height, int numChannels);
	};
}


