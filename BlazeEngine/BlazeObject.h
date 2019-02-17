// An interface for all Blaze Engine objects.
// Contains common fields and methods (Eg. identifiers) useful for all Blaze Engine objects

#pragma once

#include <string>
#include <iostream> // DEBUG
using std::cout;
using std::string;


// Global variables: These should never be modified directly.
namespace BlazeEnginePrivate
{
	static int objectIDs = 0;
}


namespace BlazeEngine
{
	class BlazeObject
	{
	public:
		BlazeObject()
		{
			objectID = AssignObjectID();
		}

		inline int GetObjectID()
		{
			return objectID;
		}

		inline string GetName()
		{
			return name;
		}

		inline void SetName(string newName)
		{
			name = newName;
		}

	protected:
		int objectID; // TO DO: Replace this with a hash of the object name

	private:
		string name = "unnamed";

		// Utilities:
		int AssignObjectID()
		{
			return BlazeEnginePrivate::objectIDs++;
		}
	};
}