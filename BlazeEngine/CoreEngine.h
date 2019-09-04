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
	// TODO: Implement loading/saving of this object...
	// TODO: Make various fields const ?
	// TODO: Make this object (and members) static?
	struct EngineConfig
	{
		EngineConfig()
		{
			
		}

		// Renderer config:
		struct
		{
			string windowTitle			= "Blaze Engine";
			int windowXRes				= 1024;
			int windowYRes				= 768;

			bool useForwardRendering	= false;

		} renderer;

		// Compute the aspect ratio == width / height
		float GetWindowAspectRatio() const { return (float)renderer.windowXRes / (float)renderer.windowYRes; }

		struct
		{
			float defaultFieldOfView	= 60.0f;
			float defaultNear			= 1.0f;
			float defaultFar			= 100.0f;

			float defaultExposure		= 1.0f;
		} mainCam;

		struct
		{
			// Camera defaults:
			float defaultNear					= 1.0f;
			float defaultFar					= 100.0f;

			float defaultOrthoHalfWidth			= 5.0f;		// TODO: Choose appropriate values??
			float defaultOrthoHalfHeight		= 5.0f;		// -> Function of resolution and scene width

			float defaultMinShadowBias			= 0.01f;
			float defaultMaxShadowBias			= 0.05f;

			// Texture dimensions:
			int defaultShadowMapWidth			= 2048;
			int defaultShadowMapHeight			= 2048;

			int defaultShadowCubeMapthWidth		= 512;
			int defaultShadowCubeMapthHeight	= 512;
		} shadows;

		struct
		{
			const string shaderDirectory					= ".\\Shaders\\";
			const string errorShaderName					= "errorShader";
			const string defaultShaderName					= "lambertShader";
			
			// Depth map rendering:
			const string depthShaderName					= "depthShader";
			const string cubeDepthShaderName				= "cubeDepthShader";

			// Deferred rendering:
			const string gBufferFillShaderName				= "gBufferFillShader";

			const string deferredAmbientLightShaderName		= "deferredAmbientLightShader";
			const string deferredKeylightShaderName			= "deferredKeyLightShader";
			const string deferredPointLightShaderName		= "deferredPointLightShader";

			const string skyboxShaderName					= "skyboxShader";

			const string blitShader							= "blitShader";
			const string blurShader							= "blurShader";
			const string toneMapShader						= "toneMapShader";

			const float	defaultSceneEmissiveIntensity		= 2.0f;
		} shader;

		struct
		{
			float mousePitchSensitivity = -0.0001f;
			float mouseYawSensitivity	= -0.0001f;
		} input;

		// Scene config:
		struct
		{
			const string sceneRoot		= ".\\Scenes\\";	// Root path: All assets stored here
			
			string currentScene			= "";			// The currently loaded scene
		} scene;
		
		// TODO: Add button config for inputmanager
		//struct inputConfig
		//{

		//};

		// TODO: Implement load/save functions
		void LoadConfig(string path);
		void SaveConfig(string path);

	private:
		

	};


	// CORE ENGINE:
	class CoreEngine : public BlazeObject, public EventListener
	{
	public:
		CoreEngine(int argc, char** argv);

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
		void Update()	{}	// Do nothing...

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
		EngineConfig config;
		string configPath	= ".\\config.cfg";
		bool configDirty	= false; // Marks whether we need to save the config file or not

		bool ProcessCommandLineArgs(int argc, char** argv);
	};
}
