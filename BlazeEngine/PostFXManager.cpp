// Member class of the RenderManager. Handles PostFX work

#include "PostFXManager.h"
#include "BuildConfiguration.h"
#include "CoreEngine.h"
#include "Mesh.h"
#include "RenderTexture.h"
#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "RenderTexture.h"

#include <vector>


namespace BlazeEngine
{
	PostFXManager::~PostFXManager()
	{
		if (pingPongTextures != nullptr)
		{
			for (int i = 0; i < NUM_DOWN_SAMPLES; i++)
			{
				pingPongTextures[i].Destroy();
			}
			delete [] pingPongTextures;
			pingPongTextures	= nullptr;
		}

		if (blitShader != nullptr)
		{
			blitShader->Destroy();
			delete blitShader;
			blitShader = nullptr;
		}

		if (toneMapShader != nullptr)
		{
			toneMapShader->Destroy();
			delete toneMapShader;
			toneMapShader = nullptr;
		}

		if (blurShaders != nullptr)
		{
			for (int i = 0; i < BLUR_SHADER_COUNT; i++)
			{
				if (blurShaders[i] != nullptr)
				{
					blurShaders[i]->Destroy();
					delete blurShaders[i];
					blurShaders[i] = nullptr;
				}				
			}
		}

		if (screenAlignedQuad != nullptr)
		{
			screenAlignedQuad->Destroy();
			delete screenAlignedQuad;
			screenAlignedQuad = nullptr;
		}		
	}

	void PostFXManager::Initialize(Material* outputMaterial)
	{
		// Cache the output material
		this->outputMaterial = outputMaterial;

		// Configure render buffers:
		this->pingPongTextures = new RenderTexture[NUM_DOWN_SAMPLES + 1]; // +1 so we have an extra RenderTexture to pingpong between at the lowest res

		int currentXRes = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowXRes") / 2;
		int currentYRes = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("windowYRes") / 2;

		for (int i = 0; i <= NUM_DOWN_SAMPLES; i++)
		{
			pingPongTextures[i] = RenderTexture
			(
				currentXRes,
				currentYRes,
				"PostFX_PingPongTexture_" + to_string(currentXRes) + "x" + to_string(currentYRes)
			);

			pingPongTextures[i].Format() = GL_RGBA;		// Note: Using 4 channels for future flexibility
			pingPongTextures[i].InternalFormat() = GL_RGBA32F;

			pingPongTextures[i].TextureMinFilter() = GL_LINEAR;
			pingPongTextures[i].TextureMaxFilter() = GL_LINEAR;

			pingPongTextures[i].AttachmentPoint() = GL_COLOR_ATTACHMENT0 + 0;

			pingPongTextures[i].ReadBuffer() = GL_COLOR_ATTACHMENT0 + 0;
			pingPongTextures[i].DrawBuffer() = GL_COLOR_ATTACHMENT0 + 0;

			// Assign and buffer texture:
			pingPongTextures[i].Buffer(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);

			// Don't halve the resolution for the last 2 iterations:
			if (i < NUM_DOWN_SAMPLES - 1)
			{
				currentXRes /= 2;
				currentYRes /= 2;
			}
		}


		// Configure shaders:
		vector<string> luminanceThresholdKeywords(1,	"BLUR_SHADER_LUMINANCE_THRESHOLD");
		vector<string> horizontalBlurKeywords(1,		"BLUR_SHADER_HORIZONTAL");
		vector<string> verticalBlurKeywords(1,			"BLUR_SHADER_VERTICAL");
		
		blurShaders[BLUR_SHADER_LUMINANCE_THRESHOLD]	= Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("blurShader"), &luminanceThresholdKeywords);
		blurShaders[BLUR_SHADER_HORIZONTAL]				= Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("blurShader"), &horizontalBlurKeywords);
		blurShaders[BLUR_SHADER_VERTICAL]				= Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("blurShader"), &verticalBlurKeywords);

		blitShader										= Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("blitShader"));
		toneMapShader									= Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("toneMapShader"));


		// Upload Shader parameters:
		toneMapShader->UploadUniform("exposure", &CoreEngine::GetSceneManager()->GetMainCamera()->Exposure(), UNIFORM_Float);

		// Upload the texel size for the SMALLEST pingpong textures:
		vec4 texelSize = this->pingPongTextures[NUM_DOWN_SAMPLES].TexelSize();
		blurShaders[BLUR_SHADER_HORIZONTAL]->UploadUniform("texelSize", &texelSize.x, UNIFORM_Vec4fv);
		blurShaders[BLUR_SHADER_VERTICAL]->UploadUniform("texelSize", &texelSize.x, UNIFORM_Vec4fv);


		screenAlignedQuad = new Mesh	// TODO: Use the RenderManager's instead of duplicating it here?
		(
			Mesh::CreateQuad
			(
				vec3(-1.0f, 1.0f, 0.0f),	// TL
				vec3(1.0f, 1.0f, 0.0f),		// TR
				vec3(-1.0f, -1.0f, 0.0f),	// BL
				vec3(1.0f, -1.0f, 0.0f)		// BR
			)
		);
	}


	void PostFXManager::ApplyPostFX(Material*& finalFrameMaterial, Shader*& finalFrameShader)
	{
		// Pass 1: Apply luminance threshold: Finished frame -> 1/2 res
		this->screenAlignedQuad->Bind(true);
		glViewport(0, 0, this->pingPongTextures[0].Width(), this->pingPongTextures[0].Height());

		// Bind the target FBO, luminance threshold shader, and source texture:
		this->pingPongTextures[0].BindFramebuffer(true);
		this->blurShaders[BLUR_SHADER_LUMINANCE_THRESHOLD]->Bind(true);
		this->outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, true);

		// Draw!
		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		this->blurShaders[BLUR_SHADER_LUMINANCE_THRESHOLD]->Bind(false);
		this->outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, false);
		this->pingPongTextures[0].BindFramebuffer(false);

		// Continue downsampling: Blit to the remaining textures:
		this->blitShader->Bind(true);
		for (int i = 1; i < NUM_DOWN_SAMPLES; i++)
		{
			// Configure the viewport:
			glViewport(0, 0, this->pingPongTextures[i].Width(), this->pingPongTextures[i].Height());

			// Bind the target FBO, and source texture to the shader
			this->pingPongTextures[i].BindFramebuffer(true);
			this->pingPongTextures[i - 1].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, true);

			// Draw!
			glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

			// Cleanup:
			this->pingPongTextures[i - 1].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, false);
			this->pingPongTextures[i].BindFramebuffer(false);
		}

		// Cleanup:
		this->blitShader->Bind(false);

		// Blur the final low-res image:
		glViewport(0, 0, this->pingPongTextures[NUM_DOWN_SAMPLES].Width(), this->pingPongTextures[NUM_DOWN_SAMPLES].Height());
		for (int i = 0; i < this->NUM_BLUR_PASSES; i++)
		{
			// Horizontal pass: (NUM_DOWN_SAMPLES - 1) -> NUM_DOWN_SAMPLES

			// Bind the target FBO, shader, and source texture:
			this->pingPongTextures[NUM_DOWN_SAMPLES].BindFramebuffer(true);
			blurShaders[BLUR_SHADER_HORIZONTAL]->Bind(true);
			this->pingPongTextures[NUM_DOWN_SAMPLES - 1].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, true);

			// Draw!
			glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0));

			// Cleanup:
			this->pingPongTextures[NUM_DOWN_SAMPLES - 1].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, false);
			blurShaders[BLUR_SHADER_HORIZONTAL]->Bind(false);
			this->pingPongTextures[NUM_DOWN_SAMPLES].BindFramebuffer(false);


			// Vertical pass: NUM_DOWN_SAMPLES -> (NUM_DOWN_SAMPLES - 1)
			
			// Bind the target FBO, shader, and source texture:
			this->pingPongTextures[NUM_DOWN_SAMPLES - 1].BindFramebuffer(true);
			blurShaders[BLUR_SHADER_VERTICAL]->Bind(true);
			this->pingPongTextures[NUM_DOWN_SAMPLES].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, true);

			// Draw!
			glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0));

			// Cleanup:
			this->pingPongTextures[NUM_DOWN_SAMPLES].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, false);
			blurShaders[BLUR_SHADER_VERTICAL]->Bind(false);
			this->pingPongTextures[NUM_DOWN_SAMPLES - 1].BindFramebuffer(false);
		}

		// Up-sample: Blit to successively larger textures:
		this->blitShader->Bind(true);
		for (int i = NUM_DOWN_SAMPLES - 1; i > 0; i--)
		{
			// Configure the viewport for the next, larger texture:
			glViewport(0, 0, this->pingPongTextures[i - 1].Width(), this->pingPongTextures[i - 1].Height());

			// Bind the target FBO, and source texture to the shader:
			this->pingPongTextures[i - 1].BindFramebuffer(true);
			this->pingPongTextures[i].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, true);

			// Draw!
			glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0));

			// Cleanup:
			this->pingPongTextures[i].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, false);
			this->pingPongTextures[i - 1].BindFramebuffer(false);
		}

		// Additively blit final blurred result (ie. half res) to the original, full-sized image: [0] -> output material
		glViewport(0, 0, this->outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO)->Width(), this->outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO)->Height());
		((RenderTexture*)this->outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO))->BindFramebuffer(true);

		// Bind source:
		this->pingPongTextures[0].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, true);
		
		glEnable(GL_BLEND);
		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
		glDisable(GL_BLEND);

		// Set the final frame material and shader to apply tone mapping:
		finalFrameMaterial	= this->outputMaterial;
		finalFrameShader	= this->toneMapShader;

		// Cleanup:
		blitShader->Bind(false);
		this->pingPongTextures[0].Bind(RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO, false);
		((RenderTexture*)outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO))->BindFramebuffer(false);

		screenAlignedQuad->Bind(false);
	}
}


