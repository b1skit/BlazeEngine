#pragma once

#include <GL/glew.h>
//#include "glm.hpp"

#include <string>

using std::string;


namespace BlazeEngine
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		// Getters/Setters:
		inline int const& Width() { return width; }
		inline int const& Height() { return height; }

		static Texture* LoadTexture(string texturePath);

	protected:


	private:
		int width;
		int height;

		GLuint textureID;
	};
}


