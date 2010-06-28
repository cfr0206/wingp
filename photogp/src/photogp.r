// ADOBE SYSTEMS INCORPORATED
// Copyright  1993 - 2002 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//-------------------------------------------------------------------------------
//
//	File:
//		photogp.r
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the resource information
//		for the Export module , a module that
//		creates a file and stores raw pixel data in it.
//
//	Use:
//		This module shows how to export raw data to a file.
//		It uses a simple "FileUtilities" library that comes
//		with the SDK.  You use it via File>>Export>>Outbound.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Definitions -- Required by include files.
//-------------------------------------------------------------------------------

// The About box and resources are created in PIUtilities.r.
// You can easily override them, if you like.

#define plugInName			"PhotoGP"
#define plugInCopyrightYear	"2009"
#define plugInDescription \
	"Gutenprint print plug-in for Adobe Photoshop¨."

//-------------------------------------------------------------------------------
//	Definitions -- Required by other resources in this rez file.
//-------------------------------------------------------------------------------

// Dictionary (aete) resources:

#define vendorName				"Dickobraz"
#define plugInAETEComment		"Gutenprint print plug-in"

#define plugInSuiteID			'sdK5'
#define plugInClassID			'outB'
#define plugInEventID			typeNull // must be this

//-------------------------------------------------------------------------------
//	Set up included files for Macintosh and Windows.
//-------------------------------------------------------------------------------

#include "PIDefines.h"

	#define Rez
	#include "PIGeneral.h"
	#include "PIUtilities.r"


#include "PITerminology.h"
#include "PIActions.h"

#include "photogpTerminology.h"	// Terminology specific to this plug-in.

//-------------------------------------------------------------------------------
//	PiPL resource
//-------------------------------------------------------------------------------

resource 'PiPL' (ResourceID, plugInName " PiPL", purgeable)
{
	{
		Kind { Export },
		Name { plugInName "..." },
		Version { (latestExportVersion << 16) | latestExportSubVersion },

			CodeWin32X86 { "PluginMain" },

		HasTerminology
		{
			plugInClassID,		// class ID
			plugInEventID,		// event ID
			ResourceID,			// aete ID
			"" 					// uniqueString
		},
		
		SupportedModes
		{
			noBitmap, doesSupportGrayScale,
			doesSupportIndexedColor, doesSupportRGBColor,
			doesSupportCMYKColor, doesSupportHSLColor,
			doesSupportHSBColor, doesSupportMultichannel,
			doesSupportDuotone, noSupportLABColor
		},
			
		EnableInfo
		{
			"in (PSHOP_ImageMode, GrayScaleMode, IndexedColorMode, RGBMode,"
			"CMYKMode, HSLMode, HSBMode, MultichannelMode,"
			"DuotoneMode, RGB48Mode, Gray16Mode) ||"
			"PSHOP_ImageDepth == 16" 
		}

//		EnableInfo
//		{
//			"in (PSHOP_ImageMode, GrayScaleMode, IndexedColorMode, RGBMode,"
//			"CMYKMode, HSLMode, HSBMode, MultichannelMode,"
//			"DuotoneMode, LabMode, RGB48Mode, Gray16Mode) ||"
//			"PSHOP_ImageDepth == 16" 
//		}









	}
};

//-------------------------------------------------------------------------------
//	PiMI resource (Photoshop 2.5 and other older hosts)
//-------------------------------------------------------------------------------

resource 'PiMI' (ResourceID, plugInName " PiMI", purgeable)
{ 
	  latestExportVersion,
	  latestExportSubVersion,
	  0,
	  supportsGrayScale +
	  supportsIndexedColor +
	  supportsRGBColor +
	  supportsCMYKColor +
	  supportsHSLColor +
	  supportsHSBColor +
	  supportsMultichannel +
	  supportsDuotone +
	  supportsLABColor,
	  '    ', /* No required host */
	  {},
};

//-------------------------------------------------------------------------------
//	Dictionary (scripting) resource
//-------------------------------------------------------------------------------

resource 'aete' (ResourceID, plugInName " dictionary", purgeable)
{
	1, 0, english, roman,									/* aete version and language specifiers */
	{
		vendorName,											/* vendor suite name */
		"Gutenprint print plug-in",							/* optional description */
		plugInSuiteID,										/* suite ID */
		1,													/* suite code, must be 1 */
		1,													/* suite level, must be 1 */
		{},													/* structure for filters */
		{													/* non-filter plug-in class here */
			vendorName " " plugInName,						/* unique class name */
			plugInClassID,										/* class ID, must be unique or Suite ID */
			plugInAETEComment,								/* optional description */
			{												/* define inheritance */
				"<Inheritance>",							/* must be exactly this */
				keyInherits,								/* must be keyInherits */
				classExport,								/* parent: Format, Import, Export */
				"parent class export",						/* optional description */
				flagsSingleProperty,						/* if properties, list below */
				
				"in",										/* our path */
				keyIn,										/* common key */
				typePlatformFilePath,						/* correct path for platform */
				"file path",								/* optional description */
				flagsSingleProperty
				
				/* no more properties */
			},
			{}, /* elements (not supported) */
			/* class descriptions */
		},
		{}, /* comparison ops (not supported) */
		{}	/* any enumerations */
	}
};

//-------------------------------------------------------------------------------
//	Resource text entries
//
//	Entering these as separate resources because then they
//	transfer directly over to windows via CNVTPIPL.
//
//	If I use a Macintosh 'STR#' resource, I could put all these
//	strings into a single resource, but there is no
//	parallel on Windows.  'STR ' resources, which hold
//	one string per ID, exist on Windows and Macintosh.
//-------------------------------------------------------------------------------

// Prompt string:
resource StringResource (kPrompt, plugInName " Prompt", purgeable)
{
	"Gutenprint:"
};

// Creator and type:
resource StringResource (kCreatorAndType, plugInName " CreatorAndType", purgeable)
{
	"Dickobraz"		// creator
	"????"		// type

};

//-------------------------------------------------------------------------------

// end photogp.r
