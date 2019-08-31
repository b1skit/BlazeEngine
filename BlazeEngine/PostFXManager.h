#pragma once

#include "Material.h"

#include <string>


namespace BlazeEngine
{
	// Pre-declarations:
	class Mesh;
	class Shader;

	enum BLUR_PASS
	{
		BLUR_SHADER_LUMINANCE_THRESHOLD,
		BLUR_SHADER_HORIZONTAL,
		BLUR_SHADER_VERTICAL,

		BLUR_SHADER_COUNT			// RESERVED: Number of blur passes in total
	};

	class PostFXManager
	{
	public:
		PostFXManager() {} // Must call Initialize() before this object can be used

		~PostFXManager();

		// Initialize PostFX. Must be called after the scene has been loaded and the RenderManager has finished initializing OpenGL
		void Initialize(Material* outputMaterial);

		void ApplyPostFX();

		// Getters/Setters:
		Material* BloomMaterial();


	private:

		Material* outputMaterial		= nullptr;	// Recieved from RenderManager

		Material* pingPongMaterial0		= nullptr;	// Deallocated in destructor
		Material* pingPongMaterial1		= nullptr;	// Deallocated in destructor

		Shader* blitShader				= nullptr;	// Deallocated in destructor
		
		Shader* blurShaders[BLUR_SHADER_COUNT];		// Deallocated in destructor
		
		
		Mesh* screenAlignedQuad			= nullptr;	// Deallocated in destructor
	};
}


