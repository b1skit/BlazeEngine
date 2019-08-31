// Member class of the RenderManager. Handles PostFX work

#include "PostFXManager.h"
#include "BuildConfiguration.h"
#include "CoreEngine.h"
#include "Mesh.h"
#include "RenderTexture.h"
#include "Shader.h"

#include <vector>


namespace BlazeEngine
{
	PostFXManager::~PostFXManager()
	{
		if (pingPongMaterial0 != nullptr)
		{
			pingPongMaterial0->Destroy();
			delete pingPongMaterial0;
			pingPongMaterial0 = nullptr;
		}

		if (pingPongMaterial1 != nullptr)
		{
			pingPongMaterial1->Destroy();
			delete pingPongMaterial1;
			pingPongMaterial1 = nullptr;
		}

		if (blitShader != nullptr)
		{
			blitShader->Destroy();
			delete blitShader;
			blitShader = nullptr;
		}

		if (blurShaders != nullptr)
		{
			for (int i = 0; i < BLUR_SHADER_COUNT; i++)
			{
				blurShaders[i]->Destroy();
				delete blurShaders[i];
				blurShaders[i] = nullptr;
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

		// Configure deferred output:
		pingPongMaterial0 = new Material("PostFX_PingPongMaterial_0", nullptr, (TEXTURE_TYPE)1, true);
		pingPongMaterial1 = new Material("PostFX_PingPongMaterial_1", nullptr, (TEXTURE_TYPE)1, true);

		RenderTexture* bloomOutputTexture0 = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes,
			CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
			"PostFX_PingPongTexture_0",
			false,
			RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO
		);

		bloomOutputTexture0->Format()			= GL_RGBA;		// Note: Using 4 channels for future flexibility
		bloomOutputTexture0->InternalFormat()	= GL_RGBA32F;

		bloomOutputTexture0->TextureMinFilter()	= GL_LINEAR;
		bloomOutputTexture0->TextureMaxFilter()	= GL_LINEAR;

		bloomOutputTexture0->AttachmentPoint()	= GL_COLOR_ATTACHMENT0 + 0;

		bloomOutputTexture0->ReadBuffer()		= GL_COLOR_ATTACHMENT0 + 0;
		bloomOutputTexture0->DrawBuffer()		= GL_COLOR_ATTACHMENT0 + 0;

		// Assign and buffer texture:
		pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO) = bloomOutputTexture0;
		bloomOutputTexture0->Buffer();

		// Create a 2nd copy of the RenderTexture:
		RenderTexture* bloomOutputTexture1 = new RenderTexture(*bloomOutputTexture0, false);
		bloomOutputTexture1->TexturePath() = "PostFX_PingPongTexture_1"; // Update the name
		bloomOutputTexture1->Buffer();
		pingPongMaterial1->AccessTexture(RENDER_TEXTURE_ALBEDO) = bloomOutputTexture1;


		// Configure shaders:

		vector<string> blurLuminanceThresholdKeywords(1, "BLUR_SHADER_LUMINANCE_THRESHOLD");
		blurShaders[BLUR_SHADER_LUMINANCE_THRESHOLD] = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->shader.blurShader, &blurLuminanceThresholdKeywords);
		
		vector<string> horizontalBlurKeywords(1, "BLUR_SHADER_HORIZONTAL");
		blurShaders[BLUR_SHADER_HORIZONTAL] = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->shader.blurShader, &horizontalBlurKeywords);

		vector<string> verticalBlurKeywords(1, "BLUR_SHADER_VERTICAL");
		blurShaders[BLUR_SHADER_VERTICAL] = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->shader.blurShader, &verticalBlurKeywords);


		blitShader = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->shader.blitShader);


		// Upload Shader parameters:
		vec4 texelSize = CoreEngine::GetCoreEngine()->GetSceneManager()->GetMainCamera()->RenderMaterial()->AccessTexture(TEXTURE_ALBEDO)->TexelSize();
		for (int i = 0; i < BLUR_SHADER_COUNT; i++)
		{
			blurShaders[i]->UploadUniform("texelSize", &texelSize.x, UNIFORM_Vec4fv);
		}


		screenAlignedQuad = new Mesh
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


	void PostFXManager::ApplyPostFX()
	{
		screenAlignedQuad->Bind(true);
		glViewport(0, 0, pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO)->Width(), pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO)->Height());

		// Pass 1: Apply luminance threshold, finished frame -> pingPong0

		// Bind the target FBO:
		glBindFramebuffer(GL_FRAMEBUFFER, (((RenderTexture*)pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO))->FBO() ));
		
		// Bind the shader:
		blurShaders[BLUR_SHADER_LUMINANCE_THRESHOLD]->Bind(true);

		// Attach the source texture to the shader:
		outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(blurShaders[BLUR_SHADER_LUMINANCE_THRESHOLD]->ShaderReference(), RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);

		// Draw!
		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);

		// Cleanup:
		blurShaders[BLUR_SHADER_LUMINANCE_THRESHOLD]->Bind(false);
		outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(0, RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);

		const int NUM_BLUR_PASSES = 2; // How many pairs of horizontal + vertical blur passes to perform

		for (int i = 0; i < NUM_BLUR_PASSES; i++)
		{
			// Horizontal pass: pingPong0 -> pingPong1

			glBindFramebuffer(GL_FRAMEBUFFER, (((RenderTexture*)pingPongMaterial1->AccessTexture(RENDER_TEXTURE_ALBEDO))->FBO()));
			
			blurShaders[BLUR_SHADER_HORIZONTAL]->Bind(true);

			pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(blurShaders[BLUR_SHADER_HORIZONTAL]->ShaderReference(), RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);

			glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0));
			
			blurShaders[BLUR_SHADER_HORIZONTAL]->Bind(false);
			pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(0, RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);


			// Vertical pass: pingPong1 -> pingPong0
			glBindFramebuffer(GL_FRAMEBUFFER, (((RenderTexture*)pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO))->FBO()));

			blurShaders[BLUR_SHADER_VERTICAL]->Bind(true);

			pingPongMaterial1->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(blurShaders[BLUR_SHADER_VERTICAL]->ShaderReference(), RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);

			glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0));

			blurShaders[BLUR_SHADER_VERTICAL]->Bind(false);
			pingPongMaterial1->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(0, RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);
		}


		// Add the blurred result to the original image: pingPong0 -> outputMaterial
		glBindFramebuffer(GL_FRAMEBUFFER, (((RenderTexture*)outputMaterial->AccessTexture(RENDER_TEXTURE_ALBEDO))->FBO()));

		blitShader->Bind(true);

		pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(blitShader->ShaderReference(), RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);

		glEnable(GL_BLEND);
		glDrawElements(GL_TRIANGLES, screenAlignedQuad->NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
		glDisable(GL_BLEND);

		// Cleanup:
		blitShader->Bind(false);
		pingPongMaterial0->AccessTexture(RENDER_TEXTURE_ALBEDO)->Bind(0, RENDER_TEXTURE_0 + RENDER_TEXTURE_ALBEDO);
		screenAlignedQuad->Bind(false);
	}


	Material* PostFXManager::BloomMaterial()
	{
		return pingPongMaterial0;
	}
}


