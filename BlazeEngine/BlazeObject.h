// An interface for all Blaze Engine objects.
// Contains common fields and methods (Eg. identifiers) useful for all Blaze Engine objects

#pragma once

#include <iostream> // DEBUG
using std::cout;

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
			cout << "BLAZEOBJECT CONTRUCTOR CALLED!!!!\n"; // Currently, we've got a bug where our "singletons" are being copied when created.

			objectID = AssignObjectID();
		}

		inline int GetObjectID()
		{
			return objectID;
		}

	protected:
		int objectID;

	private:
		// Utilities:
		int AssignObjectID()
		{
			return BlazeEnginePrivate::objectIDs++;
		}
	};
}