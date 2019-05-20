#pragma once

#include <GL/glew.h>
#include "glm.hpp"

#include <string>

using glm::vec4;
using std::string;


namespace BlazeEngine
{
	class Texture
	{
	public:
		Texture(int width, int height, string texturePath, bool doFill = true, vec4 fillColor = vec4(1.0, 0.0, 0.0, 1.0));
		~Texture();

		Texture& operator=(Texture const& rhs);

		// Getters/Setters:
		inline unsigned int const& Width() const { return width; }
		inline unsigned int const& Height() const { return height; }
		inline GLuint const& TextureID() const { return textureID; }
		inline GLenum const& Target() const { return target; }
		string const& TexturePath() { return texturePath; }

		// Get/set a texel value:
		// Returns texels[0] if u = [0, width - 1], v = [0, height - 1] are out of bounds.
		vec4& Texel(unsigned int u, unsigned int v);

		// Fill texture with a solid color
		void Fill(vec4 color, bool doBuffer = true);

		// Fill texture with a color gradient
		void Fill(vec4 tl, vec4 bl, vec4 tr, vec4 br, bool doBuffer = true); 


		// Static functions:
		//------------------
		
		// Load a texture object from a (relative) path
		// Returns nullptr if OpenGL binding fails
		static Texture* LoadTextureFromPath(string texturePath);

	protected:
		/*inline void SetTextureID(GLuint textureID) { this->textureID = textureID; }*/ // Unnecessary?


	private:
		unsigned int width	= 1;		// # Cols
		unsigned int height = 1;		// # Rows

		GLuint textureID	= 0;

		// TO DO: Make these configurable/dynamically set based on loaded file??
		/*GLint level;
		GLint xoffset;
		GLint yoffset;*/
		GLenum target			= GL_TEXTURE_2D;
		GLenum internalFormat	= GL_RGBA32F;		// TO DO: Is this worth specifying per-texture?
		GLenum format			= GL_RGBA;
		GLenum type				= GL_FLOAT;
		// Wrap modes?

		vec4* texels = nullptr;
		unsigned int numTexels;

		string texturePath		= "UnloadedTexture";

		// Upload a texture to the GPU. Returns true if successful, false otherwise
		bool Buffer();
	};
}


