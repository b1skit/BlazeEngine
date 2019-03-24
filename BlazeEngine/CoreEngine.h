#pragma once
//#include <iostream>
#include "EventManager.h"
#include "LogManager.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "SceneManager.h"

namespace BlazeEngine
{
	// ENGINE CONFIG:
	// TO DO: Implement loading/saving of this object...
	struct EngineConfig
	{
		EngineConfig(CoreEngine* coreEngine)
		{
			this->coreEngine = coreEngine;
		}

		// Renderer config:
		struct
		{
			string windowTitle = "Blaze Engine";
			int windowXRes = 800;
			int windowYRes = 600;

		} renderer;

		struct
		{
			float fieldOfView = 60.0f;
			float near = 1.0f;
			float far = 100.0f;
		} viewCam;

		struct
		{
			string shaderDirectory	= "./Shaders/";
			string defaultShader	= "defaultShader";
			string errorShader		= "errorShader";
		} shader;

		struct
		{
			float mousePitchSensitivity = -0.001f;
			float mouseYawSensitivity = -0.001f;
		} input;

		// Scene config:
		struct
		{
			string sceneRoot = "./Scenes/"; // Root path: All assets stored here

			string scenePath = sceneRoot + "testScene"; // DEBUG: hard coded scene path
		} scene;
		
		// TO DO: Add button config for inputmanager

		// TO DO: Implement load/save functions
		void LoadConfig(string path);
		void SaveConfig(string path);

	private:
		CoreEngine* coreEngine;

	};


	// CORE ENGINE:
	class CoreEngine : public BlazeObject, public EventListener
	{
	public:
		CoreEngine(string configPath);

		// Static Engine component singletons getters:		
		static inline CoreEngine*		GetCoreEngine()		{ return coreEngine; }
		static inline EventManager*		GetEventManager()	{ return BlazeEventManager; }
		static inline InputManager*		GetInputManager()	{ return BlazeInputManager; }
		static inline SceneManager*		GetSceneManager()	{ return BlazeSceneManager; }
		static inline RenderManager*	GetRenderManager()	{ return BlazeRenderManager; }
		
		// Lifetime flow:
		void Startup();
		void Run();
		void Stop();
		void Shutdown();

		// Member functions
		EngineConfig const* GetConfig();

		// BlazeObject interface:
		void Update() {} // Do nothing...

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		
	private:	
		// Constants:
		const double FIXED_TIMESTEP = 1000.0 / 120.0; // Regular step size, in ms
		//const double MAX_TIMESTEP = 0.5;	// Max amount of time before giving up

		// Private engine component singletons:	
		LogManager* const BlazeLogManager = &LogManager::Instance();
		TimeManager* const BlazeTimeManager = &TimeManager::Instance();

		// Static Engine component singletons
		static CoreEngine* coreEngine;
		static EventManager* BlazeEventManager;
		static InputManager* BlazeInputManager;
		static SceneManager* BlazeSceneManager;
		static RenderManager* BlazeRenderManager;
		

		// Engine control:
		bool isRunning = false;

		// Engine configuration:
		EngineConfig config = EngineConfig(this);
		string configPath;
		bool configDirty = false; // Marks whether we need to save the config file or not
	};
}
