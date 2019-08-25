#include "ShadowMap.h"
#include "CoreEngine.h"


namespace BlazeEngine
{
	ShadowMap::ShadowMap()
	{
		this->shadowCam		= new Camera("Unnamed_ShadowMapCam");

		RenderTexture* depthRenderTexture = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapWidth,
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapHeight
		);

		depthRenderTexture->TextureUnit() = RENDER_TEXTURE_0 + RENDER_TEXTURE_DEPTH;

		InitializeShadowCam(depthRenderTexture);
	}


	ShadowMap::ShadowMap(string lightName, int xRes, int yRes, CameraConfig shadowCamConfig, Transform* shadowCamParent /*= nullptr*/, vec3 shadowCamPosition /* = vec3(0.0f, 0.0f, 0.0f)*/, bool useCubeMap /*= false*/)
	{
		this->shadowCam = new Camera(lightName + "_ShadowMapCam", shadowCamConfig, shadowCamParent);
		this->shadowCam->GetTransform()->SetWorldPosition(shadowCamPosition);

		// Omni-directional (Cube map) shadowmap setup:
		if (useCubeMap)
		{
			this->shadowCam->RenderMaterial() = new Material(shadowCam->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->shader.cubeDepthShaderName, CUBE_MAP_COUNT, true);
			
			RenderTexture* cubeFaces[6];

			// Attach a texture to each slot:
			for (int i = 0; i < CUBE_MAP_COUNT; i++)
			{
				RenderTexture* cubeRenderTexture = new RenderTexture
				(
					xRes,
					yRes,
					lightName + "_CubeMap_" + to_string(i),
					false,
					CUBE_MAP_0 + CUBE_MAP_0_RIGHT	// We always use the same sampler for all cube map faces
				);


				// Configure the texture:
				cubeRenderTexture->TextureTarget()		= GL_TEXTURE_CUBE_MAP;
				
				cubeRenderTexture->TextureWrap_S()		= GL_CLAMP_TO_EDGE;
				cubeRenderTexture->TextureWrap_T()		= GL_CLAMP_TO_EDGE;
				cubeRenderTexture->TextureWrap_R()		= GL_CLAMP_TO_EDGE;

				cubeRenderTexture->TextureMinFilter()	= GL_NEAREST;
				cubeRenderTexture->TextureMaxFilter()	= GL_NEAREST;

				cubeRenderTexture->InternalFormat()		= GL_DEPTH_COMPONENT32F;
				cubeRenderTexture->Format()				= GL_DEPTH_COMPONENT;
				cubeRenderTexture->Type()				= GL_FLOAT;

				cubeRenderTexture->AttachmentPoint()	= GL_DEPTH_ATTACHMENT;
				cubeRenderTexture->DrawBuffer()			= GL_NONE;
				cubeRenderTexture->ReadBuffer()			= GL_NONE;

				this->shadowCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)i) = cubeRenderTexture;

				// Cache off the texture for buffering when we're done
				cubeFaces[i] = cubeRenderTexture;
			}

			// Buffer the cube map:
			((RenderTexture*)this->shadowCam->RenderMaterial()->AccessTexture((TEXTURE_TYPE)0))->BufferCubeMap(cubeFaces);

			CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
		}
		else // Single texture shadowmap setup:
		{
			RenderTexture* depthRenderTexture = new RenderTexture // Deallocated by Camera.Destroy()
			(
				xRes,
				yRes,
				lightName + "_RenderTexture",
				true,
				RENDER_TEXTURE_0 + RENDER_TEXTURE_DEPTH
			);

			InitializeShadowCam(depthRenderTexture);
		}		
	}


	// Helper function: Reduces some duplicate code for non-cube map depth textures
	void ShadowMap::InitializeShadowCam(RenderTexture* renderTexture)
	{
		this->shadowCam->RenderMaterial() = new Material(shadowCam->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->shader.depthShaderName, RENDER_TEXTURE_COUNT, true);
		this->shadowCam->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH) = renderTexture;

		CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
	}
}


