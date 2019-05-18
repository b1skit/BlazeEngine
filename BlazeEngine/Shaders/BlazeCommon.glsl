// Blaze Engine Shader Common
// Defines variables, structures, and functions common to all shaders

// Vertex shader specific properties:
//-----------------------------------
#if defined(BlazeVertexShader)
	layout(location = 0) in vec3 in_position;
	layout(location = 1) in vec4 in_color;

	layout(location = 2) in vec3 in_normal;
	layout(location = 3) in vec3 in_tangent;
	layout(location = 4) in vec3 in_bitangent;

	layout(location = 5) in vec4 in_uv0;
	layout(location = 6) in vec4 in_uv1;
	layout(location = 7) in vec4 in_uv2;
	layout(location = 8) in vec4 in_uv3;

	layout(location = 9) out struct VtoF
	{
		vec4 vertexColor;
	
		vec3 fragWorldNormal;
		vec3 tangent;
		vec3 bitangent;

		vec4 uv0;
		vec4 uv1;
		vec4 uv2;
		vec4 uv3;

	//	vec3 worldPos;
		vec3 viewPos;		// Camera/eye-space position
	} data;
#endif


// Fragment shader specific properties:
//-------------------------------------
#if defined(BlazeFragmentShader)
	layout(location = 9) in struct VtoF
	{
		vec4 vertexColor;
	
		vec3 fragWorldNormal;
		vec3 tangent;
		vec3 bitangent;

		vec4 uv0;
		vec4 uv1;
		vec4 uv2;
		vec4 uv3;

	//	vec3 worldPos;
		vec3 viewPos;		// Camera/eye-space position
	} data;

	out vec4 FragColor;
#endif

// Common shader properties:
//--------------------------
uniform vec3 ambient;

uniform vec3 keyDirection;	// Normalized, world space, points towards light source
uniform vec3 keyColor;

uniform mat4 in_model;		// Local -> World
uniform mat4 in_view;		// World -> View
uniform mat4 in_projection; // View -> Projection
uniform mat4 in_mv;			// View * Model
uniform mat4 in_mvp;		// Projection * View * Model

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D RMAO;

// Generic material properties:
uniform vec3 matProperty0; // .x == Phong cosine exponent
//uniform vec3 matProperty1;
//uniform vec3 matProperty2;
//uniform vec3 matProperty3;
//uniform vec3 matProperty4;
//uniform vec3 matProperty5;
//uniform vec3 matProperty6;
//uniform vec3 matProperty7;