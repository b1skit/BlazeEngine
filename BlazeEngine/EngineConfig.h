#pragma once

#include <string>
#include <unordered_map>
#include <any>

using std::string;
using std::to_string;
using std::unordered_map;
using std::any;


namespace BlazeEngine
{
	struct EngineConfig
	{
		EngineConfig();

		// Initialize the configValues mapping with default values. MUST be called before the config can be accessed. Set all default values here.
		void InitializeDefaultValues();

		// Get a config value, by type
		template<typename T>
		T GetValue(const string& valueName) const;
		string GetValueAsString(const string& valueName) const;

		// Set a config value
		template<typename T>
		void SetValue(const string& valueName, T value); // Note: Strings must be explicitely defined as a string("value")

		// Compute the aspect ratio == width / height
		float GetWindowAspectRatio() const { return (float)(GetValue<int>("windowXRes")) / (float)(GetValue<int>("windowYRes")); }

		// Load the config.cfg from CONFIG_FILENAME
		void LoadConfig();

		// Save config.cfg to disk
		void SaveConfig();

		// Public properties:
		string currentScene = "";			// The currently loaded scene (cached during command-line parsing, and accessed once SceneManager is loaded)

	private:
		unordered_map<string, any> configValues;	// The primary config parameter/value mapping


		const string CONFIG_DIR			= ".\\config\\";
		const string CONFIG_FILENAME	= "config.cfg";
		
		bool isDirty = false; // Marks whether we need to save the config file or not


		// Inline helper functions:
		//------------------
		inline string PropertyToConfigString(string property)	{ return " \"" + property + "\"\n"; }
		inline string PropertyToConfigString(float property)	{ return " " + to_string(property) + "\n"; }
		inline string PropertyToConfigString(int property)		{ return " " + to_string(property) + "\n"; }
		inline string PropertyToConfigString(char property)		{ return string(" ") + property + string("\n");	}
		string PropertyToConfigString(bool property);			// Note: Inlined in .cpp file, as it depends on macros defined in KeyConfiguration.h

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


