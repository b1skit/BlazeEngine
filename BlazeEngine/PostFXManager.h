#pragma once

#include "Material.h" // REMOVE THIS????
#include "RenderTexture.h"

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

		// Apply post processing. Modifies finalFrameMaterial and finalFrameShader to contain the material & shader required to blit the final image to screen
		void ApplyPostFX(Material*& finalFrameMaterial, Shader*& finalFrameShader);

		// Getters/Setters:



	private:

		Material* outputMaterial		= nullptr;	// Recieved from RenderManager

		RenderTexture* pingPongTextures = nullptr;	// Deallocated in destructor
		const int NUM_DOWN_SAMPLES		= 2;		// Scaling factor: We half the frame size this many times
		const int NUM_BLUR_PASSES		= 3;		// How many pairs of horizontal + vertical blur passes to perform

		Shader* blitShader				= nullptr;	// Deallocated in destructor
		Shader* toneMapShader			= nullptr;	// Deallocated in destructor
		Shader* blurShaders[BLUR_SHADER_COUNT];		// Deallocated in destructor
		
		
		Mesh* screenAlignedQuad			= nullptr;	// Deallocated in destructor
	};
}


