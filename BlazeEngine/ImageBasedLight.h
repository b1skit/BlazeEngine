#pragma once

#include "Light.h"	// Base class

#include <string>

using std::string;


namespace BlazeEngine
{
	// Predeclarations:
	class Material;
	class RenderTexture;


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

		// Get the Irradiance Environment Map material:
		Material* GetIEMMaterial()		{ return this->IEM_Material; }
		Material* GetPMREMMaterial()	{ return this->PMREM_Material; }

		// Check if an IBL was successfully loaded
		bool IsValid() const		{ return this->IEM_isValid && this->PMREM_isValid; }


		// Public static functions:
		//-------------------------

		// Convert an equirectangular HDR image to a cubemap
		// hdrPath is relative to the scene path, with no leading slash eg. "IBL\\ibl.hdr"
		// iblType controls the filtering (IEM/PMREM/None) applied to the converted cubemap 
		// Returns an array of 6 textures
		static RenderTexture** ConvertEquirectangularToCubemap(string sceneName, string relativeHDRPath, int xRes, int yRes, IBL_TYPE iblType = RAW_HDR);

	private:
		Material* IEM_Material		= nullptr;	// Irradiance Environment Map (IEM) Material: Deallocated in destructor
		Material* PMREM_Material	= nullptr;	// Pre-filtered Mip-mapped Radiance Environment Map (PMREM) Material: Deallocated in destructor

		// Cubemap face resolution:
		int xRes					= 512;
		int yRes					= 512;

		bool IEM_isValid			= false; // Is the IEM valid? (Ie. Were IBL textures successfully loaded?)
		bool PMREM_isValid			= false; // Is the PMREM valid? (Ie. Were IBL textures successfully loaded?)
	};
}


