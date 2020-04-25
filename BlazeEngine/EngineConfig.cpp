#include "EngineConfig.h"
#include "BuildConfiguration.h"
#include "KeyConfiguration.h"

#include <fstream>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <type_traits>

using std::ifstream;
using std::any_cast;


namespace BlazeEngine
{
	void EngineConfig::InitializeDefaultValues()
	{
		// Define the default values in unordered_map, to simplify (de)serialization.
		// Note: String values must be explicitely defined as string objects
		this->configValues =
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


			// Key bindings:
			{MACRO_TO_STR(INPUT_BUTTON_FORWARD),	'w'},
			{MACRO_TO_STR(INPUT_BUTTON_BACKWARD),	's'},
			{MACRO_TO_STR(INPUT_BUTTON_LEFT),		'a'},
			{MACRO_TO_STR(INPUT_BUTTON_RIGHT),		'd'},
			{MACRO_TO_STR(INPUT_BUTTON_UP),			string(SPACE)},
			{MACRO_TO_STR(INPUT_BUTTON_DOWN),		string(L_SHIFT)},

			{MACRO_TO_STR(INPUT_BUTTON_QUIT),		string(ESC)},

			// Mouse bindings:
			{MACRO_TO_STR(INPUT_MOUSE_LEFT),		string(MACRO_TO_STR(INPUT_MOUSE_LEFT))},
			{MACRO_TO_STR(INPUT_MOUSE_RIGHT),		string(MACRO_TO_STR(INPUT_MOUSE_RIGHT))},

		};

		this->isDirty = true;
	}



	// Get a config value, by type
	template<typename T>
	T EngineConfig::GetValue(const string& valueName) const
	{
		auto result = configValues.find(valueName);
		T returnVal;
		if (result != configValues.end())
		{
			try
			{
				returnVal = any_cast<T>(result->second);
			}
			catch (const std::bad_any_cast & e)
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
	// Explicitely instantiate our templates so the compiler can link them from the .cpp file:
	template string EngineConfig::GetValue<string>(const string& valueName) const;
	template float EngineConfig::GetValue<float>(const string& valueName) const;
	template int EngineConfig::GetValue<int>(const string& valueName) const;
	template bool EngineConfig::GetValue<bool>(const string& valueName) const;
	template char EngineConfig::GetValue<char>(const string& valueName) const;


	string EngineConfig::GetValueAsString(const string& valueName) const
	{
		auto result = configValues.find(valueName);
		string returnVal = "";
		if (result != configValues.end())
		{
			try
			{
				if (result->second.type() == typeid(string))
				{
					returnVal = any_cast<string>(result->second);
				}
				else if (result->second.type() == typeid(float))
				{
					float configValue = any_cast<float>(result->second);
					returnVal = to_string(configValue);
				}
				else if (result->second.type() == typeid(int))
				{
					int configValue = any_cast<int>(result->second);
					returnVal = to_string(configValue);
				}
				else if (result->second.type() == typeid(char))
				{
					char configValue = any_cast<char>(result->second);
					returnVal = string(1, configValue); // Construct a string with 1 element
				}
				else if (result->second.type() == typeid(bool))
				{
					bool configValue = any_cast<bool>(result->second);
					returnVal = configValue == true ? "1" : "0";
				}
			}
			catch (const std::bad_any_cast & e)
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


	// Set a config value
	template<typename T>
	void EngineConfig::SetValue(const string& valueName, T value) // Note: Strings must be explicitely defined as a string("value")
	{
		configValues[valueName] = value;
		this->isDirty = true;
	}
	// Explicitely instantiate our templates so the compiler can link them from the .cpp file:
	template void EngineConfig::SetValue<string>(const string& valueName, string value);
	template void EngineConfig::SetValue<float>(const string& valueName, float value);
	template void EngineConfig::SetValue<int>(const string& valueName, int value);
	template void EngineConfig::SetValue<bool>(const string& valueName, bool value);
	template void EngineConfig::SetValue<char>(const string& valueName, char value);



	// Constructor
	EngineConfig::EngineConfig()
	{
		// Populate the config hash table with initial values
		InitializeDefaultValues();

		// Load config.cfg file
		this->LoadConfig();
	}


	void EngineConfig::LoadConfig()
	{
		LOG("Loading " + this->CONFIG_FILENAME);

		ifstream file;
		file.open((CONFIG_DIR + CONFIG_FILENAME).c_str());

		// If no config is found, create one:
		if (!file.is_open())
		{
			LOG_WARNING("No config.cfg file found! Attempting to create a default version");

			this->isDirty = true;

			this->SaveConfig();

			return;
		}
		
		// Process the config file:
		string line;
		bool foundInvalidString = false;
		while (file.good())
		{
			// Handle malformed strings from previous iteration:
			if (foundInvalidString == true)
			{
				LOG_WARNING("Ignoring invalid command in config.cfg:\n" + line);
				foundInvalidString = false;
			}

			// Get the next line:
			getline(file, line);

			// Replace whitespace with single spaces:
			std::regex tabMatch("([\\s])+");
			string cleanLine = std::regex_replace(line, tabMatch, " ");

			// Skip empty or near-empty lines:
			if (cleanLine.find_first_not_of(" \t\n") == string::npos || cleanLine.length() <= 2) // TODO: Choose a more meaningful minimum line length
			{
				continue;
			}

			// Remove single leading space, if it exists:
			if (cleanLine.at(0) == ' ')
			{
				cleanLine = cleanLine.substr(1, string::npos);
			}

			// Remove comments:
			size_t commentStart = cleanLine.find_first_of("#");
			if (commentStart != string::npos)
			{
				// Remove the trailing space, if it exists:
				if (commentStart > size_t(0) && cleanLine.at(commentStart - 1) == ' ')
				{
					commentStart--;
				}

				cleanLine = cleanLine.substr(0, commentStart);

				if (cleanLine.length() == 0)
				{
					continue;
				}
			}

			// Ensure we have exactly 3 arguments:
			int numSpaces = 0;
			for (int i = 0; i < cleanLine.length(); i++)
			{
				if (cleanLine.at(i) == ' ')
				{
					numSpaces++;
				}
			}
			if (numSpaces < 2)
			{
				foundInvalidString = true;
				continue;
			}

			// Extract leading command:
			size_t firstSpace = cleanLine.find_first_of(" \t", 1);
			string command = cleanLine.substr(0, firstSpace);

			// Remove the command from the head of the string:
			cleanLine = cleanLine.substr(firstSpace + 1, string::npos);


			// Extract the variable property name:
			firstSpace = cleanLine.find_first_of(" \t\n", 1);
			string property = cleanLine.substr(0, firstSpace);

			// Remove the property from the head of the string:
			cleanLine = cleanLine.substr(firstSpace + 1, string::npos);

			// Clean up the value string:
			string value = cleanLine;

			// Remove quotation marks from value string:
			bool isString = false;
			if (value.find("\"") != string::npos)
			{
				isString = true;
				std::regex quoteMatch("([\\\"])+");
				value = std::regex_replace(value, quoteMatch, "");
			}


			// Update config hashtables:
			if (command == "set")
			{
				// Strings:
				if (isString)
				{
					configValues[property] = string(value);
				}
				else
				{
					// Booleans:
					string boolString = ToLowerCase(value);
					if (boolString == TRUE_STRING)
					{
						configValues[property] = true;
						continue;
					}
					else if (boolString == FALSE_STRING)
					{
						configValues[property] = false;
						continue;
					}

					// Numeric values: Try and cast as an int, and fallback to a float if it fails
					size_t position = 0;
					int intResult = std::stoi(value, &position);

					// Ints:
					if (position == value.length())
					{
						configValues[property] = intResult;
					}
					else // Floats:
					{
						float floatResult = std::stof(value);
						configValues[property] = floatResult;
					}
				}

			}
			else if (command == "bind")
			{
				configValues[property] = (char)value[0]; // Assume bound values are single chars, for now. Might need to rework this to bind more complex keys
			}
			else
			{
				foundInvalidString = true;
				continue;
			}
		}

		// Handle final malformed string:
		if (foundInvalidString == true)
		{
			LOG_WARNING("Ignoring invalid command in config.cfg:\n" + line);
		}

		this->isDirty = false;
	}


	void EngineConfig::SaveConfig()
	{
		if (this->isDirty == false)
		{
			LOG("SaveConfig called, but config has not changed. Returning without modifying file on disk");
			return;
		}

		// Create the .\config\ directory, if none exists
		std::filesystem::path configPath = CONFIG_DIR;
		if (!std::filesystem::exists(configPath))
		{
			LOG("Creating .\\config\\ directory");

			std::filesystem::create_directory(configPath);
		}

		// Write our config to disk:
		std::ofstream config_ofstream(CONFIG_DIR + CONFIG_FILENAME);
		config_ofstream << "# BlazeEngine config.cfg file:\n";


		// Output each value, by type:
		for (std::pair<string, any> currentElement : configValues)
		{
			if (currentElement.second.type() == typeid(string))
			{
				config_ofstream << SET_CMD << currentElement.first << PropertyToConfigString(any_cast<string>(currentElement.second));
			}
			else if (currentElement.second.type() == typeid(float))
			{
				config_ofstream << SET_CMD << currentElement.first << PropertyToConfigString(any_cast<float>(currentElement.second));
			}
			else if (currentElement.second.type() == typeid(int))
			{
				config_ofstream << SET_CMD << currentElement.first << PropertyToConfigString(any_cast<int>(currentElement.second));
			}
			else if (currentElement.second.type() == typeid(bool))
			{
				config_ofstream << SET_CMD << currentElement.first << PropertyToConfigString(any_cast<bool>(currentElement.second));
			}
			else if (currentElement.second.type() == typeid(char))
			{
				config_ofstream << BIND_CMD << currentElement.first << PropertyToConfigString(any_cast<char>(currentElement.second));	
			}
			else
			{
				LOG_ERROR("Cannot write unsupported type to config");
			}
		}
		

		this->isDirty = false;
	}


	// Note: We inline this here, as it depends on macros defined in KeyConfiguration.h
	inline string EngineConfig::PropertyToConfigString(bool property) { return string(" ") + (property == true ? TRUE_STRING : FALSE_STRING) + string("\n"); }
}