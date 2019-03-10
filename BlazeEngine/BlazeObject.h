// An interface for all Blaze Engine objects.
// Contains common fields and methods (Eg. identifiers) useful for all Blaze Engine objects

#pragma once
#include <string>
//#include <unordered_map>

//#include <iostream> // DEBUG
//using std::cout;

using std::string;
//using std::unordered_map;


// Global variables: These should never be modified directly.
namespace BlazeEnginePrivate
{
	static unsigned long objectIDs = 0;
}


namespace BlazeEngine
{
	// Predeclarations:
	class CoreEngine;

	class BlazeObject
	{
	public:
		BlazeObject(string name)
		{
			if (!name.length() == 0) // Default to "unnamed" if no valid name is received
			{
				this->name = name;
			}

			this->coreEngine = coreEngine;

			objectID = AssignObjectID();
		}

		// Getters/Setters:
		inline unsigned long GetObjectID() { return objectID; }

		inline string GetName() const { return name; }

		// Used to hash objects when inserting into an unordered_map
		inline string GetHashString() { return hashString; }

		virtual void Update() = 0;

	protected:
		unsigned long objectID; // Hashed value
		CoreEngine* coreEngine;
	private:
		string name = "unnamed";
		string hashString;


		std::hash<string> hashFunction;

		// Utilities:
		unsigned long AssignObjectID()
		{ 
			hashString = name + std::to_string(BlazeEnginePrivate::objectIDs++); // Append a number to give different hashes for the same name
			size_t hash = hashFunction(hashString);

			return (unsigned long) hash;
		}
	};
}