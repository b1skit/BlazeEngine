#include "Light.h"
#include "CoreEngine.h"
#include "Camera.h"
#include "BuildConfiguration.h"


namespace BlazeEngine
{
	BlazeEngine::Light::Light(string lightName, LIGHT_TYPE lightType, vec3 color, ShadowMap* shadowMap /*= nullptr*/, float radius /*= 1.0f*/)
	{
		this->lightName		= lightName;
		this->type			= lightType;
		this->color			= color;

		this->shadowMap		= shadowMap;

		// Set up deferred light mesh:
		string shaderName;
		switch (lightType)
		{
		case LIGHT_AMBIENT_COLOR:
		case LIGHT_AMBIENT_IBL:
		{
			vector<string> shaderKeywords;
			if (lightType == LIGHT_AMBIENT_COLOR)
			{
				shaderKeywords.push_back("AMBIENT_COLOR");
			}
			else // LIGHT_AMBIENT_IBL
			{
				shaderKeywords.push_back("AMBIENT_IBL");
			}

			Shader* ambientLightShader = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("deferredAmbientLightShaderName"), &shaderKeywords);

			// Attach a deferred Material:
			this->deferredMaterial = new Material
			(
				lightName + "_deferredMaterial",
				ambientLightShader,
				(TEXTURE_TYPE)0, // No textures
				true
			);

			// Attach a screen aligned quad:
			this->deferredMesh = new Mesh
			(
				Mesh::CreateQuad	// Align along near plane
				(
					vec3(-1.0f, 1.0f, -1.0f),	// TL
					vec3(1.0f, 1.0f, -1.0f),	// TR
					vec3(-1.0f, -1.0f, -1.0f),	// BL
					vec3(1.0f, -1.0f, -1.0f)	// BR
				)
			);

			break;
		}			

		case LIGHT_DIRECTIONAL:
		{
			// Attach a deferred Material:
			this->deferredMaterial = new Material
			(
				lightName + "_deferredMaterial",
				CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("deferredKeylightShaderName"),
				(TEXTURE_TYPE)0, // No textures
				true
			);

			// Attach a screen aligned quad:
			this->deferredMesh = new Mesh
			(
				Mesh::CreateQuad	// Align along near plane
				(
					vec3(-1.0f,	1.0f,	-1.0f),	// TL
					vec3(1.0f,	1.0f,	-1.0f),	// TR
					vec3(-1.0f,	-1.0f,	-1.0f),	// BL
					vec3(1.0f,	-1.0f,	-1.0f)	// BR
				)
			);
			break;
		}

		case LIGHT_POINT:
			this->deferredMaterial = new Material
			(
				lightName + "_deferredMaterial",
				CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("deferredPointLightShaderName"),
				(TEXTURE_TYPE)0, // No textures
				true
			);

			this->deferredMesh = new Mesh
			(
				Mesh::CreateSphere
				(
					radius
				)
			);
			this->deferredMesh->GetTransform().Parent(&this->transform);

			break;

		case LIGHT_SPOT:
		case LIGHT_AREA:
		case LIGHT_TUBE:
		default:
			// TODO: Implement light meshes for additional light types
			break;
		}
	}


	void Light::Destroy()
	{
		if (shadowMap != nullptr)
		{
			delete shadowMap;
			shadowMap = nullptr;
		}

		if (deferredMesh != nullptr)
		{
			delete deferredMesh;
			deferredMesh = nullptr;
		}

		if (deferredMaterial != nullptr)
		{
			delete deferredMaterial;
			deferredMaterial = nullptr;
		}

		lightName += "_DELETED";
	}


	void Light::Update()
	{
	}


	void Light::HandleEvent(EventInfo const * eventInfo)
	{
	}


	ShadowMap*& Light::ActiveShadowMap(ShadowMap* newShadowMap /*= nullptr*/)
	{
		// No-arg: Gets the current shadow map
		if (newShadowMap == nullptr)
		{
			return this->shadowMap;
		}

		if (shadowMap != nullptr)
		{
			LOG("Deleting an existing shadow map");
			delete shadowMap;
			shadowMap = nullptr;
		}

		this->shadowMap = newShadowMap;

		return this->shadowMap;
	}
}

