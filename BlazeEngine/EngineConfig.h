#pragma once

#include <string>
#include <unordered_map>
#include <any>
#include <stdexcept>

using std::string;
using std::to_string;
using std::unordered_map;
using std::any;
using std::any_cast;




//#define MACRO_AS_STRING(macro) #macro
//
//// Define default parameter names:
//#define TEST_PARAM testParam




namespace BlazeEngine
{
	// Define the default values in unordered_map s, to simply (de)serialization.
	// Note: String values must be explicitely defined as string objects	
	static unordered_map<string, any> configValues =
	{
		// Renderer config:
		{"windowTitle",							string("Blaze Engine")},
		{"windowXRes",							1024},
		{"windowYRes",							768},

		// Quality settings:
		{"useForwardRendering",					false},

		{"numIEMSamples",						15000},	// Number of samples to use when generating IBL IEM texture
		{"defaultIBLPath",						string("IBL\\ibl.hdr")},

		// Camera defaults:
		{"defaultFieldOfView",					60.0f},
		{"defaultNear",							1.0f},		// Note: Default value is used by shadow cameras
		{"defaultFar",							100.0f},	// Note: Default value is used by shadow cameras
		{"defaultExposure",						1.0f},

		// Shadow map defaults:
		{"defaultOrthoHalfWidth",				5.0f},		// TODO: Choose appropriate values??
		{"defaultOrthoHalfHeight",				5.0f},		// -> Function of resolution and scene width
		{"defaultMinShadowBias",				0.01f},
		{"defaultMaxShadowBias",				0.05f},
	
		// Texture dimensions:
		{"defaultShadowMapWidth",				2048},
		{"defaultShadowMapHeight",				2048},
		{"defaultShadowCubeMapthWidth",			512},
		{"defaultShadowCubeMapthHeight",		512},

		// Shader:
		{"shaderDirectory",						string(".\\Shaders\\")},
		{"errorShaderName",						string("errorShader")},
		{"defaultShaderName",					string("lambertShader")},
		 
		 // Depth map rendering:
		 {"depthShaderName",					string("depthShader")},
		 {"cubeDepthShaderName",				string("cubeDepthShader")},
		 
		// Deferred rendering:
		{"gBufferFillShaderName",				string("gBufferFillShader")},
		{"deferredAmbientLightShaderName",		string("deferredAmbientLightShader")},
		{"deferredKeylightShaderName",			string("deferredKeyLightShader")},
		{"deferredPointLightShaderName",		string("deferredPointLightShader")},
		{"skyboxShaderName",					string("skyboxShader")},
		{"blitShader",							string("blitShader")},
		{"blurShader",							string("blurShader")},
		{"toneMapShader",						string("toneMapShader")}, 
		{"defaultSceneEmissiveIntensity",		2.0f},

		// Input parameters:
		{"mousePitchSensitivity",				-0.00005f},
		{"mouseYawSensitivity",					-0.00005f},
		
		// Scene config:
		{"sceneRoot",							string(".\\Scenes\\")},		// Root path: All assets stored here
	};


	struct EngineConfig
	{
		EngineConfig();

		// Get a config value, by type
		template<typename T>
		T GetValue(const string& valueName) const
		{
			auto result = configValues.find(valueName);
			if (result != configValues.end())
			{
				return any_cast<T>(result->second);
			}
			else
			{
				throw std::runtime_error("Config key does not exist");
			}
		}
		
		template<typename T>
		void SetValue(const string& valueName, T value) // Note: Strings must be explicitely defined as a string("value")
		{
			configValues[valueName] = value;
			this->isDirty			= true;
		}

		// Compute the aspect ratio == width / height
		float GetWindowAspectRatio() const { return (float)(GetValue<int>("windowXRes")) / (float)(GetValue<int>("windowYRes")); }

		string currentScene = "";			// The currently loaded scene (cached during command-line parsing, and accessed once SceneManager is loaded)

		// TODO: Add button config for inputmanager
		//struct inputConfig
		//{

		//};


		// Load the config.cfg from CONFIG_FILENAME
		void LoadConfig();

		// Save config.cfg to disk
		void SaveConfig();

	private:
		const string CONFIG_DIR			= ".\\config\\";
		const string CONFIG_FILENAME	= "config.cfg";
		
		bool isDirty = false; // Marks whether we need to save the config file or not

		// Config commands
		const string SET_CMD	= "set ";	// Set a value
		const string BIND_CMD	= "bind ";	// Bind a key to something


		// Helper functions:
		inline string PropertyToConfigString(string property)	{ return " \"" + property + "\"\n"; }
		inline string PropertyToConfigString(float property)	{ return " " + to_string(property) + "\n"; }
		inline string PropertyToConfigString(int property)		{ return " " + to_string(property) + "\n"; }
		inline string PropertyToConfigString(bool property)		{ return " " + to_string(property) + "\n"; }
	};
}


