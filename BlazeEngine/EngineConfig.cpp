#include "EngineConfig.h"
#include "BuildConfiguration.h"

#include <fstream>
#include <filesystem>
#include <regex>

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


				LOG_ERROR(line);

				

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


				LOG_ERROR("Line after whitespace replacement:\n>" + cleanLine + "<");

				
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

					LOG_ERROR("Line after comment removal:\n>" + cleanLine + "<");

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
				string command	= cleanLine.substr(0, firstSpace);

				LOG_ERROR("Command = >" + command + "<");

				// Remove the command from the head of the string:
				cleanLine			= cleanLine.substr(firstSpace + 1, string::npos);
				
				
				// Extract the variable property name:
				firstSpace = cleanLine.find_first_of(" \t\n", 1);
				string property = cleanLine.substr(0, firstSpace);


				LOG_ERROR("property = >" + property + "<");


				// Remove the property from the head of the string:
				cleanLine = cleanLine.substr(firstSpace + 1, string::npos);

				// Clean up the value string:
				string value = cleanLine;

				// Remove quotation marks from value string:
				std::regex quoteMatch("([\\\"])+");
				value = std::regex_replace(value, quoteMatch, "");

				LOG_ERROR("value = >" + value + "<");

				
				// Update config hashtables:
				if (command == "set")
				{
					LOG_ERROR("FOUND SET COMMAND");

					// Check and see if we can find a matching value:
					if (configValues.find(property) != configValues.end())
					{
						configValues[property] = value;
					}
				}
				else if (command == "bind")
				{
					// TODO: Implement config key bindings

					LOG_ERROR("Found bind command, but binding is not yet implemented");

					configValues[property] = value; // store it anyway
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
		config_ofstream << "# BlazeEngine config file\n";


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
			else
			{
				LOG_ERROR("Cannot write unsupported type to config");
			}
		}
		

		this->isDirty = false;
	}

}