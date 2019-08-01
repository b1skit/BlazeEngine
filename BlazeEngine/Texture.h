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
		Texture(int width, int height, string texturePath, bool doFill = true, vec4 fillColor = TEXTURE_ERROR_COLOR_VEC4, bool doBuffer = false, int textureUnit = -1);

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
		inline GLenum&				TextureWrap_T()		{ return textureWrapS; }

		inline GLenum&				TextureMinFilter()	{ return textureMinFilter; }
		inline GLenum&				TextureMaxFilter()	{ return textureMaxFilter; }

		inline GLuint&				Sampler()			{ return samplerID; }

		inline string&				TexturePath()		{ return texturePath; }
		inline int&					TextureUnit()		{ return textureUnit; }

		// Get/set a texel value:
		// Returns texels[0] if u = [0, width - 1], v = [0, height - 1] are out of localBounds.
		vec4& Texel(unsigned int u, unsigned int v);

		// Fill texture with a solid color
		void Fill(vec4 color, bool doBuffer = false);

		// Fill texture with a color gradient
		void Fill(vec4 tl, vec4 bl, vec4 tr, vec4 br, bool doBuffer = false); 

		// Initialization:
		bool Buffer();	// Upload a texture to the GPU. Returns true if successful, false otherwise

		// Bind the texture to its sampler for Shader sampling
		void Bind(GLuint const& shaderReference = 0, int textureUnitOverride = -1); // NOTE: GL_TEXTURE0 + textureUnit is what is bound when calling glActiveTexture()


		// Public static functions:
		//-------------------------
		
		// Load a texture object from a (relative) path. Note: Returns nullptr if OpenGL binding fails
		static Texture* LoadTextureFileFromPath(string texturePath, bool doBuffer = false);
		


	protected:
		GLuint textureID			= 0;
		
		GLenum texTarget			= GL_TEXTURE_2D;
		GLenum format				= GL_RGBA;
		GLenum internalFormat		= GL_RGBA32F;
		GLenum type					= GL_FLOAT;

		GLenum textureWrapS			= GL_REPEAT;
		GLenum textureWrapT			= GL_REPEAT;

		GLenum textureMinFilter		= GL_NEAREST_MIPMAP_LINEAR;
		GLenum textureMaxFilter		= GL_LINEAR;

		GLuint samplerID			= 0;		// Name of a sampler
		int textureUnit				= -1;		// Index to which the texture unit is bound. Must be set before Buffer() is called (ie. via constructor, or manually)
		// TODO: Move textureUnit to material level to allow textures to be shared among different slots?

		unsigned int	width		= 1;		// # Cols
		unsigned int	height		= 1;		// # Rows

		vec4*			texels		= nullptr;
		unsigned int	numTexels	= 0;

		string texturePath			= "Uninitialized_Texture";


		bool resolutionHasChanged	= false; // Does OpenGL need to be notified of new texture dimensions the next time Buffer() is called?

	private:	

		
	};
}


