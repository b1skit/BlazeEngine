#pragma once

#include "Light.h"
#include "Texture.h"
#include "Material.h"

#include "RenderTexture.h"

#include <string>

using std::string;


namespace BlazeEngine
{
	enum IBL_TYPE
	{
		IBL_IEM,		// Irradience Environment Map
		IBL_PMREM,		// Pre-filtered Mipmapped Radience Environment Map

		RAW_HDR,		// Unfiltered: Used for straight conversion of equirectangular map to cubemap

		IBL_COUNT		// RESERVED: The number of IBL texture types supported
	};


	class ImageBasedLight : public Light
	{
	public:
		ImageBasedLight(string lightName, string relativeHDRPath);

		~ImageBasedLight();

		Material* GetIBLMaterial() { return this->iblMaterial; }

		// Check if an IBL was successfully loaded
		bool IsValid()	{ return this->isValid; }


		// Public static functions:
		//-------------------------

		// Convert an equirectangular HDR image to a cubemap
		// hdrPath is relative to the scene path, with no leading slash eg. "IBL\\ibl.hdr"
		// iblType controls the filtering (IEM/PMREM/None) applied to the converted cubemap 
		// Returns an array of 6 textures
		static RenderTexture** ConvertEquirectangularToCubemap(string sceneName, string relativeHDRPath, int xRes, int yRes, IBL_TYPE iblType = RAW_HDR);

	private:
		Material* iblMaterial	= nullptr;	// Deallocated in destructor

		// Cubemap face resolution:
		int xRes				= 512;
		int yRes				= 512;

		bool isValid			= false; // Is this a valid IBL light? (Ie. Were IBL textures successfully loaded?)
	};
}


