#pragma once

#include "EngineComponent.h"
#include "EventListener.h"
#include "SceneObject.h"
#include "GameObject.h"
#include "Renderable.h"
//#include "Material.h"
#include "Shader.h"
//#include "Light.h"
//#include "Camera.h"

//class GameObject;
//class SceneObject;

#include <vector>

using std::vector;

namespace BlazeEngine
{
	class SceneManager : public EngineComponent, public EventListener
	{
	public:
		SceneManager() : EngineComponent("SceneManager") {}

		~SceneManager();

		// Singleton functionality:
		static SceneManager& Instance();
		SceneManager(SceneManager const&) = delete; // Disallow copying of our Singleton
		void operator=(SceneManager const&) = delete;

		// EngineComponent interface:
		void Startup(CoreEngine * coreEngine);
		void Shutdown();
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Member functions:
		void LoadScene(string scenePath);

		inline vector<Renderable const*> const* GetRenderables() { return &renderables;	}
		inline vector<Shader>* GetShaders() { return &shaders; } // SHOULD THIS RETURN CONST ?????
		/*inline vector<Light> const* GetLights();
		inline Camera const* GetCamera();*/

	protected:


	private:
		// The scene manager holds and provides access to all of the scene objects needed by the rest of the engine:
		vector<GameObject> gameObjects;
		vector<Renderable const*> renderables; // Pointers to statically allocated renderables held by GameObjects
		vector<Mesh> meshes;
		vector<Material> materials;
		vector<Shader> shaders;

		/*vector<Light> lights;*/
		/*Camera mainCamera;*/



		// Shaders:
		/*vector<Shader> shaders;*/

		unsigned int GetShaderIndex(string shaderName);
		int CreateShader(string shaderName);
		string LoadShaderFile(const string& filepath);
		GLuint CreateGLShaderObject(const string& text, GLenum shaderType);
		bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);
		//void BindShader(int shaderIndex); // Set the active vertex/fragement shader
	};

}

