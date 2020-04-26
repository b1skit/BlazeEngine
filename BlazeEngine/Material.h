#pragma once

#include <vector>
#include <string>

#include "glm.hpp"
#include "GL/glew.h" 

using glm::vec3;
using glm::vec4;
using std::vector;
using std::string;


namespace BlazeEngine
{
	// Predeclarations:
	class Texture;
	class Shader;


	enum TEXTURE_TYPE
	{
		TEXTURE_0							= 0,	// RESERVED: Starting offset for *binding* Textures to a texture unit: TEXTURE_0 + TEXTURE_<texture type>

		TEXTURE_ALBEDO						= 0,	// Contains transparency in the alpha channel
		TEXTURE_NORMAL						= 1,
		TEXTURE_RMAO						= 2,	// Packed Roughness, Metalic, AmbientOcclusion (RGB) + unused A
		TEXTURE_EMISSIVE					= 3,

		TEXTURE_COUNT						= 4,	// RESERVED: Number of Texture slots a material has

		// GBuffer RenderTexture names:
		RENDER_TEXTURE_0					= 4,	// RESERVED: Starting offset for *binding* RenderTextures to a texture unit: RENDER_TEXTURE_0 + RENDER_TEXTURE_<texture type>

		RENDER_TEXTURE_ALBEDO				= 0,
		RENDER_TEXTURE_WORLD_NORMAL			= 1,
		RENDER_TEXTURE_RMAO					= 2,
		RENDER_TEXTURE_EMISSIVE				= 3,
		RENDER_TEXTURE_WORLD_POSITION		= 4,
		RENDER_TEXTURE_MATERIAL_PROPERTY_0	= 5,	// MATERIAL_PROPERTY_0
		RENDER_TEXTURE_DEPTH				= 6,	// Make this the last element

		RENDER_TEXTURE_COUNT				= 7,	// RESERVED: Number of RenderTexture slots a material has

		// Depth map texture units:
		DEPTH_TEXTURE_0						= 11,	// RESERVED: Starting offset for *binding* depth RenderTextures to a texture unit: DEPTH_TEXTURE_0 + DEPTH_TEXTURE_<texture tyep>. First unit must equal TEXTURE_COUNT + RENDER_TEXTURE_COUNT

		DEPTH_TEXTURE_SHADOW				= 0,

		DEPTH_TEXTURE_COUNT					= 1,	// RESERVED: Number of DEPTH RenderTexture slots a material has

		// Cube maps:
		CUBE_MAP_0							= 12,	// RESERVED: Starting offset for *binding* cube RenderTextures to a texture unit: CUBE_MAP_0 + CUBE_MAP_TEXTURE_<texture tyep>. First unit must equal TEXTURE_COUNT + RENDER_TEXTURE_COUNT + DEPTH_TEXTURE_COUNT

		CUBE_MAP_0_RIGHT					= 0,	// X+
		CUBE_MAP_1_LEFT						= 1,	// X-
		CUBE_MAP_2_TOP						= 2,	// Y+
		CUBE_MAP_3_BOTTOM					= 3,	// Y-
		CUBE_MAP_4_NEAR						= 4,	// Z+
		CUBE_MAP_5_FAR						= 5,	// Z-

		CUBE_MAP_COUNT						= 6,

	}; // Note: If new enums are added, don't forget to update Material::RENDER_TEXTURE_SAMPLER_NAMES[] as well!


	enum MATERIAL_PROPERTY_INDEX
	{
		MATERIAL_PROPERTY_0,		// Shader's matProperty0
		//MATERIAL_PROPERTY_1,		// Shader's matProperty1
		//MATERIAL_PROPERTY_2,		// Shader's matProperty2
		//MATERIAL_PROPERTY_3,		// Shader's matProperty3
		//MATERIAL_PROPERTY_4,		// Shader's matProperty4
		//MATERIAL_PROPERTY_5,		// Shader's matProperty5
		//MATERIAL_PROPERTY_6,		// Shader's matProperty6
		//MATERIAL_PROPERTY_7,		// Shader's matProperty7

		MATERIAL_PROPERTY_COUNT		// Reserved: Number of properties a material can hold
	}; // Note: If new enums are added, don't forget to update Material::MATERIAL_PROPERTY_NAMES[] as well!


	class Material
	{
	public:
		Material(string materialName, string shaderName, TEXTURE_TYPE textureCount = TEXTURE_COUNT, bool isRenderMaterial = false);
		Material(string materialName, Shader* shader, TEXTURE_TYPE textureCount = TEXTURE_COUNT, bool isRenderMaterial = false);

		void Destroy();

		// TODO: Copy constructor, assignment operator

		// Getters/Setters:
		inline string const&	Name()									{ return name; }
		inline Shader*&			GetShader()								{ return shader; }
		inline vec4&			Property(MATERIAL_PROPERTY_INDEX index) { return properties[index]; }

		Texture*&				AccessTexture(TEXTURE_TYPE textureType);
		inline int const&		NumTextureSlots()						{ return numTextures; }

		vector<string> const&	ShaderKeywords() const					{ return shaderKeywords; }
		void					AddShaderKeyword(string const& newKeyword);
		

		inline bool&			IsRenderMaterial()						{ return isRenderMaterial; }


		void BindAllTextures(GLuint const& shaderReference = 0);

		// Helper function: Attaches an array of 6 textures
		void AttachCubeMapTextures(Texture** cubeMapFaces); // cubeMapFaces must be EXACTLY 6 elements


		// RenderTexture sampler names:
		//-----------------------------
		const static string TEXTURE_SAMPLER_NAMES[TEXTURE_COUNT];
		const static string RENDER_TEXTURE_SAMPLER_NAMES[RENDER_TEXTURE_COUNT];
		const static string DEPTH_TEXTURE_SAMPLER_NAMES[DEPTH_TEXTURE_COUNT];
		const static string CUBE_MAP_TEXTURE_SAMPLER_NAMES[CUBE_MAP_COUNT];
		const static string MATERIAL_PROPERTY_NAMES[MATERIAL_PROPERTY_COUNT];
		
	protected:
		

	private:
		string		name;									// Must be unique: Identifies this material

		Shader*		shader		= nullptr;					// Deallocated up in SceneManager.Shutdown()
		Texture**	textures	= nullptr;					// Deallocated when object is destroyed in SceneManager.Shutdown()
		int			numTextures = 0;
		
		vec4		properties[MATERIAL_PROPERTY_COUNT];	// Generic material properties

		vector<string> shaderKeywords;

		bool isRenderMaterial	= false;

		// Private member functions:
		//--------------------------

		// Helper function: Initialize arrays
		void Init();
	};
}


