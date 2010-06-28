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
//		Outbound.h
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the headers and prototypes
//		for the Export module Outbound, a module that
//		creates a file and stores raw pixel data in it.
//
//	Use:
//		This module shows how to export raw data to a file.
//		It uses a simple "FileUtilities" library that comes
//		with the SDK.  You use it via File>>Export>>Outbound.
//
//-------------------------------------------------------------------------------

#ifndef __photogp_H__				// Has this been defined yet?
#define __photogp_H__				// Only include once by predefining it.

#include "PIExport.h"				// Export Photoshop header file.
#include "PIUtilities.h"			// SDK Utility library.
#include "photogpTerminology.h"	// Terminology specific to this plug-in.

//#if __PIMac__
//	#include <StandardFile.h>
//#endif

//-------------------------------------------------------------------------------
//	Globals -- structures
//-------------------------------------------------------------------------------

// The region of the image being exported (an area and a range of planes):

#pragma pack(push,1)
struct gpraw_t
{
	char id[8];	
	int offset_data;
	unsigned long size_icc;	
	int offset_icc;
	unsigned long w;
	unsigned long h;
	int planes;
	int depth;
	int xdpi;
	int ydpi;
	char color_space[32];
	char channel_name[32][32];
	char document_name[1024];
	gpraw_t()
	{			
			memset(this,0,sizeof(gpraw_t));
			strcpy(id,"gpraw1");
			offset_data=sizeof(gpraw_t);
	};
};

#pragma pack(pop)


typedef struct ExportRegion
{
	Rect rect;
	int16 loPlane;
	int16 hiPlane;

} ExportRegion;

typedef struct Globals
{ // This is our structure that we use to pass globals between routines:

	short					*result;				// Must always be first in Globals.
	ExportRecord			*exportParamBlock;		// Must always be second in Globals.

	Boolean					queryForParameters;
	Boolean					sameNames;
	FileHandle				fRefNum;

	// AliasHandle on Mac, Handle on Windows:
	PIPlatformFileHandle	aliasHandle;

} Globals, *GPtr, **GHdl;
	
//-------------------------------------------------------------------------------
//	Globals -- definitions and macros
//-------------------------------------------------------------------------------

#define gResult 				(*(globals->result))
#define gStuff  				(globals->exportParamBlock)

#define gQueryForParameters		(globals->queryForParameters)
#define gSameNames				(globals->sameNames)
#define gAliasHandle			(globals->aliasHandle)
#define gFRefNum				(globals->fRefNum)

//-------------------------------------------------------------------------------
//	Prototypes
//-------------------------------------------------------------------------------

Boolean ReadScriptParams (GPtr globals);	// Read any scripting params.
OSErr WriteScriptParams (GPtr globals);		// Write any scripting params.

void DoAbout (AboutRecordPtr about); 	    // Pop about box.
Boolean DoUI (GPtr globals);				// Show the UI.

Boolean CreateExportFile (GPtr globals);
Boolean WriteExportFile (GPtr globals);
Boolean CloseExportFile (GPtr globals);

void MarkExportFinished (ExportRecord *stuff);

OSErr FetchData (ExportRecord *stuff, 		// in/out
				 ExportRegion *region, 		// in
				 void **data, 				// out
				 int32 *rowBytes); 			// out



#endif // __photogp_H__
