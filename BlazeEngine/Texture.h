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
		Texture(int width, int height);
		~Texture();

		Texture& operator=(Texture const& rhs);

		// Getters/Setters:
		inline unsigned int const& Width() const { return width; }
		inline unsigned int const& Height() const { return height; }
		inline GLuint const& TextureID() const { return textureID; }

		// Get/set a texel value:
		// Returns texels[0] if u = [0, width - 1], v = [0, height - 1] are out of bounds.
		vec4& Texel(unsigned int u, unsigned int v);

		// Static functions:
		//------------------
		
		// Load a texture object from a (relative) path:
		static Texture* LoadTextureFromPath(string texturePath);

	protected:
		inline void SetTextureID(GLuint textureID) { this->textureID = textureID; }


	private:
		unsigned int width;	// # Cols
		unsigned int height; // # Rows
		// TO DO: Initialize with some starting value? ^^

		GLuint textureID;

		vec4* texels = nullptr;
		unsigned int numTexels;
	};
}


