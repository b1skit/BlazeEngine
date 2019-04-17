#pragma once
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
	// TO DO: Make various fields const ?
	// TO DO: Make this object (and members) static?
	struct EngineConfig
	{
		EngineConfig(CoreEngine* coreEngine)
		{
			this->coreEngine = coreEngine;
		}

		// Renderer config:
		struct
		{
			string windowTitle	= "Blaze Engine";
			int windowXRes		= 800;
			int windowYRes		= 600;

		} renderer;

		struct
		{
			float defaultFieldOfView	= 60.0f;
			float defaultNear			= 1.0f;
			float defaultFar			= 100.0f;
		} viewCam;

		struct
		{
			const string shaderDirectory	= ".\\Shaders\\";
			const string errorShaderName	= "errorShader";
			const string defaultShaderName	= "defaultShader";
		} shader;

		struct
		{
			float mousePitchSensitivity = -0.001f;
			float mouseYawSensitivity	= -0.001f;
		} input;

		// Scene config:
		struct
		{
			const string sceneRoot			= ".\\Scenes\\";	// Root path: All assets stored here

			const string scene01			= "simpleCube";		// Generic scene name
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
