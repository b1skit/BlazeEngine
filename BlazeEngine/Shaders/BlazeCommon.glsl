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

#if defined(BLAZE_FRAGMENT_SHADER) && !defined(BLAZE_GBUFFER)
	out vec4 FragColor;
#endif


// Common shader properties:
//--------------------------

#if defined(BLAZE_VERTEX_SHADER)
	layout(location = 9) out struct VtoF	// Vertex output
#elif defined(BLAZE_GEOMETRY_SHADER)
	struct VtoF								// Geometry in/out: Must be bound to the same location as both an in and out in the geometry shader
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
		vec3 shadowPos;			// Shadowmap projection-space position

		mat3 TBN;				// Normal map change-of-basis matrix
#if defined(BLAZE_VERTEX_SHADER) || defined(BLAZE_FRAGMENT_SHADER)
	} data;
#elif defined(BLAZE_GEOMETRY_SHADER)
	};
#endif


// Forward Lighting:
uniform vec3 ambientColor;		// Deprecated: Use deferred lightColor instead

// Deferred lights:
uniform vec3 lightWorldDir;		// Normalized, world space, points towards light source
uniform vec3 lightColor;
uniform vec3 lightWorldPos;		// Light position in world space


// Matrices:
uniform mat4 in_model;			// Local -> World
uniform mat4 in_modelRotation;	// Local -> World, rotations ONLY (i.e. For transforming normals)
uniform mat4 in_view;			// World -> View
uniform mat4 in_projection;		// View -> Projection
uniform mat4 in_mv;				// [View * Model]
uniform mat4 in_mvp;			// [Projection * View * Model]
// TODO: Assign locations for these uniforms, and bind them to each shader in the RenderManager
// Probably need to be offset somehow (b/c of size of VtoF struct?)???


// Texture samplers:
// NOTE: Binding locations must match the definitions in Material.h
												// TEXTURE:								FBX MATERIAL SOURCE SLOT:
												//---------								-------------------------
layout(binding = 0) uniform sampler2D albedo;	// Albedo (RGB) + transparency (A)		Diffuse/color
layout(binding = 1) uniform sampler2D normal;	// Tangent-space normals (RGB)			Bump
layout(binding = 2) uniform sampler2D RMAO;		// Roughness, Metalic, albedo			Specular
layout(binding = 3) uniform sampler2D emissive;	// Emissive (RGB)						Incandescence



// GBuffer samplers: (For reading FROM GBuffer textures)
layout(binding = 4) uniform sampler2D GBuffer_Albedo;
layout(binding = 5) uniform sampler2D GBuffer_WorldNormal;
layout(binding = 6) uniform sampler2D GBuffer_RMAO;
layout(binding = 7) uniform sampler2D GBuffer_Emissive;
layout(binding = 8) uniform sampler2D GBuffer_WorldPos;
layout(binding = 9) uniform sampler2D GBuffer_MatProp0;

layout(binding = 10) uniform sampler2D GBuffer_Depth;

layout(binding = 11) uniform sampler2D shadowDepth;			// Currently bound shadow depth map

// Shadow map parameters:
uniform vec4		GBuffer_Depth_TexelSize;	// .xyzw = (1/width, 1/height, width, height)
uniform mat4		shadowCam_vp;			// Shadow map: [Projection * View]

uniform float		maxShadowBias;			// Offsets for preventing shadow acne
uniform float		minShadowBias;


// Generic material properties:
uniform vec4 matProperty0;		// .x == Phong cosine exponent
//uniform vec4 matProperty1;
//uniform vec4 matProperty2;
//uniform vec4 matProperty3;
//uniform vec4 matProperty4;
//uniform vec4 matProperty5;
//uniform vec4 matProperty6;
//uniform vec4 matProperty7;


// System variables:
uniform vec4 screenParams;		// .x = xRes, .y = yRes, .z = 1/xRes, .w = 1/yRes
//uniform vec4 zBufferParams;

//uniform vec3 cameraPosition;	// World-space camera position
//// TODO: ^^^Implement these