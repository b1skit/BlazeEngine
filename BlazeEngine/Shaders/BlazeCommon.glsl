// Blaze Engine Shader Common
// Defines variables and structures common to all shaders

// Vertex shader specific properties:
//-----------------------------------

#if defined(BLAZE_VERTEX_SHADER)
	layout(location = 0) in vec3 in_position;
	layout(location = 1) in vec4 in_color;

	layout(location = 2) in vec3 in_normal;
	layout(location = 3) in vec3 in_tangent;
	layout(location = 4) in vec3 in_bitangent;

	layout(location = 5) in vec4 in_uv0;
	layout(location = 6) in vec4 in_uv1;
	layout(location = 7) in vec4 in_uv2;
	layout(location = 8) in vec4 in_uv3;
#endif


// Fragment shader specific properties:
//-------------------------------------

#if defined(BLAZE_FRAGMENT_SHADER)
	out vec4 FragColor;
#endif


// Common shader properties:
//--------------------------

#if defined(BLAZE_VERTEX_SHADER)
	layout(location = 9) out struct VtoF	// Vertex output
#elif defined(BLAZE_FRAGMENT_SHADER)
	layout(location = 9) in struct VtoF		// Fragment input
#endif
	{
		vec4 vertexColor;

		vec3 vertexWorldNormal;

		vec4 uv0;
		vec4 uv1;
		vec4 uv2;
		vec4 uv3;

		vec3 viewPos;			// Camera/eye-space position
		vec3 worldPos;			// World-space position

		mat3 TBN;				// Normal map change-of-basis matrix
	} data;


// Lighting:
uniform vec3 ambient;

uniform vec3 key_direction;		// Normalized, world space, points towards light source
uniform vec3 key_color;
uniform mat4 key_vp;			// Keylight: [Projection * View]
uniform float key_shadowBias;	// Used to prevent shadow acne



// Matrices:
uniform mat4 in_model;			// Local -> World
uniform mat4 in_modelRotation;	// Local -> World, rotations ONLY (i.e. For transforming normals)
uniform mat4 in_view;			// World -> View
uniform mat4 in_projection;		// View -> Projection
uniform mat4 in_mv;				// [View * Model]
uniform mat4 in_mvp;			// [Projection * View * Model]


// Texture samplers:
								// TEXTURE:								FBX MATERIAL SOURCE SLOT:
								//---------								-------------------------
uniform sampler2D albedo;		// Albedo (RGB) + transparency (A)		Diffuse/color
uniform sampler2D normal;		// Tangent-space normals (RGB)			Bump
//uniform sampler2D emissive;	// Emissive (RGB)						Incandescence
uniform sampler2D RMAO;			// Roughness, Metalic, albedo			Specular


// RenderTexture samplers:
uniform sampler2D shadowDepth;	// The currently bound shadow depth map

// Generic shadow map properties:
//uniform float shadowBias;
// TODO: Implement this^^^

// Generic material properties:
uniform vec3 matProperty0; // .x == Phong cosine exponent
//uniform vec3 matProperty1;
//uniform vec3 matProperty2;
//uniform vec3 matProperty3;
//uniform vec3 matProperty4;
//uniform vec3 matProperty5;
//uniform vec3 matProperty6;
//uniform vec3 matProperty7;


//// Camera variables:
//uniform vec4 zBufferParams;
//uniform vec4 screenParams;
//uniform vec3 cameraPosition;	// World-space camera position
//// TODO: ^^^Implement these