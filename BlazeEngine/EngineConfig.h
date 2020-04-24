#pragma once

#include "BuildConfiguration.h"

#include <string>
#include <unordered_map>
#include <any>
#include <stdexcept>

using std::string;
using std::to_string;
using std::unordered_map;
using std::any;
using std::any_cast;


// Default true/false strings: Choose as lowercase to simplify comparisons
#define TRUE_STRING "true"		
#define FALSE_STRING "false"

// Command strings: End with a space to maintain formatting
#define SET_CMD "set "		// Set a value
#define BIND_CMD "bind "	// Bind a key


namespace BlazeEngine
{
	struct EngineConfig
	{
		// Define the default values in unordered_map, to simplify (de)serialization.
		// Note: String values must be explicitely defined as string objects	
		unordered_map<string, any> configValues =
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
			{"depthShaderName",						string("depthShader")},
			{"cubeDepthShaderName",					string("cubeDepthShader")},

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


			// Key bindings (chars):
			{"btn_forward",							'w'},
			{"btn_backward",						's'},
			{"btn_strafeLeft",						'a'},
			{"btn_strafeRight",						'd'},
		};




		EngineConfig();

		// Get a config value, by type
		template<typename T>
		T GetValue(const string& valueName) const
		{
			auto result = configValues.find(valueName);
			T returnVal;
			if (result != configValues.end())
			{
				try
				{
					returnVal = any_cast<T>(result->second);
				}
				catch (const std::bad_any_cast &e)
				{
					LOG_ERROR("bad_any_cast exception thrown: Invalid type requested from EngineConfig\n" + string(e.what()));
				}
			}
			else
			{
				throw std::runtime_error("Config key does not exist");
			}

			return returnVal;
		}

		
		template<typename T>
		void SetValue(const string& valueName, T value) // Note: Strings must be explicitely defined as a string("value")
		{
			configValues[valueName] = value;
			this->isDirty			= true;
		}

		// Compute the aspect ratio == width / height
		float GetWindowAspectRatio() const { return (float)(GetValue<int>("windowXRes")) / (float)(GetValue<int>("windowYRes")); }

		// Load the config.cfg from CONFIG_FILENAME
		void LoadConfig();

		// Save config.cfg to disk
		void SaveConfig();

		// Public properties:
		string currentScene = "";			// The currently loaded scene (cached during command-line parsing, and accessed once SceneManager is loaded)

	private:
		const string CONFIG_DIR			= ".\\config\\";
		const string CONFIG_FILENAME	= "config.cfg";
		
		bool isDirty = false; // Marks whether we need to save the config file or not


		// Helper functions:
		//------------------
		inline string PropertyToConfigString(string property)	{ return " \"" + property + "\"\n"; }
		inline string PropertyToConfigString(float property)	{ return " " + to_string(property) + "\n"; }
		inline string PropertyToConfigString(int property)		{ return " " + to_string(property) + "\n"; }
		inline string PropertyToConfigString(bool property)		{ return string(" ") + (property == true ? TRUE_STRING : FALSE_STRING) + string("\n"); }
		inline string PropertyToConfigString(char property)		{ return string(" ") + property + string("\n");	}

		// Convert a string to lower case: Used to simplify comparisons
		inline string ToLowerCase(string input)
		{
			string output;
			for (auto currentChar : input)
			{
				output += std::tolower(currentChar);
			}
			return output;
		}
	};
}


