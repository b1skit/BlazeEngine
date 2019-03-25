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

		// TO DO: Deep copy copy constructor, equals operator...

		// Getters/Setters:
		inline unsigned int const& Width() const { return width; }
		inline unsigned int const& Height() const { return height; }
		inline GLuint const& TextureID() const { return textureID; }
		inline vec4 const* const* Texels() const { return texels; }

		// Static functions:
		// 
		static Texture* LoadTextureFromPath(string texturePath);

	protected:
		inline void SetTextureID(GLuint textureID) { this->textureID = textureID; }


	private:
		unsigned int width;	// # Cols
		unsigned int height; // # Rows
		// TO DO: Initialize with some starting value? ^^

		GLuint textureID;

		vec4** texels; // Accessed by [height][width] = [row][col] (Is this best??)
	};
}


