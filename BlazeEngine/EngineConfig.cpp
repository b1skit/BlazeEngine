#include "EngineConfig.h"
#include "BuildConfiguration.h"

#include <fstream>
#include <filesystem>
#include <regex>

#include "SDL_keyboard.h"
#include "SDL_keycode.h"

using std::ifstream;


#define NUM_VALUES 1

namespace BlazeEngine
{
	// Constructor
	EngineConfig::EngineConfig()
	{
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
		else
		{
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
				size_t firstSpace	= cleanLine.find_first_of(" \t", 1);
				string command		= cleanLine.substr(0, firstSpace);

				// Remove the command from the head of the string:
				cleanLine			= cleanLine.substr(firstSpace + 1, string::npos);
				
				
				// Extract the variable property name:
				firstSpace			= cleanLine.find_first_of(" \t\n", 1);
				string property		= cleanLine.substr(0, firstSpace);

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
		}
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

}