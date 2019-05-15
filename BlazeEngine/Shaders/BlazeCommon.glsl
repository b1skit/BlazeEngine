// Blaze Engine Shader Common
// Defines variables, structures, and functions common to all shaders

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