# Blaze Engine
A real-time 3D renderer, with the architecture of a game engine.
- Requires a GPU compatible with OpenGL 4.3+

Check the Documentation directory for UML diagrams.

Included Dependencies:
- Assimp used for .FBX scene imports
- GLM used for OpenGL math data structures
- SDL 2.0 used for input event parsing
- STB used for .PNG image loading
- Compiles with Visual Studio 2019

.FBX Scene support:
- Loaded via commandline flag -scene <sceneName> (Eg. -scene myScene)
- File references must use relative paths (ie. When exporting from Maya, set the project directory to the root of the scene)
- The scene's directory name and .FBX name must match (eg. \Scenes\myScene\myScene.FBX)
- BlazeEngine supports the standard scene heirarchy, however group names and mesh names must be unique
- Ambient lights must contain the string "ambient" within their name
- Mesh triangulation is not required, but recommended
- Tangents/bitangents are not required, but recommended
- Meshes must have valid UV's, as they're required by Assimp for tangent/bitangent generation
- 1 unit = 1m
  
Recommended Visual Studio extensions:
- Smart Command Line Arguments: https://marketplace.visualstudio.com/items?itemName=MBulli.SmartCommandlineArguments
