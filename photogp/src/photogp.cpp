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
//		photogp.cpp
//
//	Copyright 1990-1992, Thomas Knoll.
//	All Rights Reserved.
//
//	Description:
//		This file contains the functions and source
//		for the Export module Outbound, a module that
//		creates a file and stores raw pixel data in it.
//
//	Use:
//		This module shows how to export raw data to a file.
//		It uses a simple "FileUtilities" library that comes
//		with the SDK.  You use it via File>>Export>>Outbound.
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//	Includes
//-------------------------------------------------------------------------------

#include "photogp.h"
#include <PropertyUtils.h>
#include <PIColorSpaceSuite.h>
#include <string>

//-------------------------------------------------------------------------------
//	Prototypes.
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr);		  	// Initialize globals.
void ValidateParameters (GPtr globals);		// Validate parameters.
void DoPrepare (GPtr globals);
void DoStart (GPtr globals);				// Main routine.
void DoContinue (GPtr globals);
void DoFinish (GPtr globals);
void DoInitialRect (GPtr globals);
Boolean DoNextRect (GPtr globals);
void DoExportRect (GPtr globals);

//-------------------------------------------------------------------------------
//	Globals -- Define global variables for plug-in scope.
//-------------------------------------------------------------------------------

SPBasicSuite * sSPBasic = NULL;

extern char tmp_file[MAX_PATH+1];
//-------------------------------------------------------------------------------
//
//	PluginMain / main
//
//	All calls to the plug-in module come through this routine.
//	It must be placed first in the resource.  To achieve this,
//	most development systems require this be the first routine
//	in the source.
//
//	The entrypoint will be "pascal void" for Macintosh,
//	"void" for Windows.
//
//	Inputs:
//		const short selector						Host provides selector indicating
//													what command to do.
//
//		ExportRecord *exportParamBlock				Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIExport.h.
//
//	Outputs:
//		ExportRecord *exportParamBlock				Host provides pointer to parameter
//													block containing pertinent data
//													and callbacks from the host.
//													See PIExport.h.
//
//		long *data									Use this to store a handle to our
//													global parameters structure, which
//													is maintained by the host between
//													calls to the plug-in.
//
//		short *result								Return error result or noErr.  Some
//													errors are handled by the host, some
//													are silent, and some you must handle.
//													See PIGeneral.h.
//
//-------------------------------------------------------------------------------

DLLExport MACPASCAL void PluginMain (const short selector,
						             void *exportParamBlock,
						             long *data,
						             short *result)
{
	//---------------------------------------------------------------------------
	//	(1) Check for about box request.
	//
	// 	The about box is a special request; the parameter block is not filled
	// 	out, none of the callbacks or standard data is available.  Instead,
	// 	the parameter block points to an AboutRecord, which is used
	// 	on Windows.
	//---------------------------------------------------------------------------

	if (selector == exportSelectorAbout)
	{
		sSPBasic = ((AboutRecord*)exportParamBlock)->sSPBasic;
		DoAbout((AboutRecordPtr)exportParamBlock);
	}
	else
	{ // do the rest of the process as normal:

		sSPBasic = ((ExportRecordPtr)exportParamBlock)->sSPBasic;

		Ptr globalPtr = NULL;		// Pointer for global structure
		GPtr globals = NULL; 		// actual globals

		//-----------------------------------------------------------------------
		//	(2) Allocate and initalize globals.
		//
		// 	AllocateGlobals requires the pointer to result, the pointer to the
		// 	parameter block, a pointer to the handle procs, the size of our local
		// 	"Globals" structure, a pointer to the long *data, a Function
		// 	Proc (FProcP) to the InitGlobals routine.  It automatically sets-up,
		// 	initializes the globals (if necessary), results result to 0, and
		// 	returns with a valid pointer to the locked globals handle or NULL.
		//-----------------------------------------------------------------------
		
		globalPtr = AllocateGlobals ((uint32)result,
									 (uint32)exportParamBlock,
									 ((ExportRecordPtr)exportParamBlock)->handleProcs,
									 sizeof(Globals),
						 			 data,
						 			 InitGlobals);
		
		if (globalPtr == NULL)
		{ // Something bad happened if we couldn't allocate our pointer.
		  // Fortunately, everything's already been cleaned up,
		  // so all we have to do is report an error.
		  
		  *result = memFullErr;
		  return;
		}
		
		// Get our "globals" variable assigned as a Global Pointer struct with the
		// data we've returned:
		globals = (GPtr)globalPtr;

		//-----------------------------------------------------------------------
		//	(3) Dispatch selector.
		//-----------------------------------------------------------------------

		switch (selector)
		{
			case exportSelectorPrepare:
				DoPrepare(globals);
				break;
			case exportSelectorStart:
				DoStart(globals);
				break;
			case exportSelectorContinue:
				DoContinue(globals);
				break;
			case exportSelectorFinish:
				DoFinish(globals);
				break;
		}
							
		//-----------------------------------------------------------------------
		//	(4) Unlock data, and exit resource.
		//
		//	Result is automatically returned in *result, which is
		//	pointed to by gResult.
		//-----------------------------------------------------------------------	
		
		// unlock handle pointing to parameter block and data so it can move
		// if memory gets shuffled:
		if ((Handle)*data != NULL)
			PIUnlockHandle((Handle)*data);
	
	} // about selector special		
	
} // end PluginMain

//-------------------------------------------------------------------------------
//
//	InitGlobals
//	
//	Initalize any global values here.  Called only once when global
//	space is reserved for the first time.
//
//	Inputs:
//		Ptr globalPtr		Standard pointer to a global structure.
//
//	Outputs:
//		Initializes any global values with their defaults.
//
//-------------------------------------------------------------------------------

void InitGlobals (Ptr globalPtr)
{	
	// create "globals" as a our struct global pointer so that any
	// macros work:
	GPtr globals = (GPtr)globalPtr;
	
	// Initialize global variables:
	ValidateParameters (globals);
	
} // end InitGlobals


//-------------------------------------------------------------------------------
//
//	ValidateParameters
//
//	Initialize parameters to default values.
//
//	Inputs:
//		GPtr globals		Pointer to global structure.
//
//	Outputs:
//		gCurrentHistory		Default: 1.
//
//-------------------------------------------------------------------------------

void ValidateParameters (GPtr globals)
{
	gQueryForParameters = true;
	gAliasHandle = nil; // no handle, yet

} // end ValidateParameters

//-------------------------------------------------------------------------------
//
//	DoPrepare
//
//	Initialize parameters to default values.
//
//	Inputs:
//
//	Outputs:

/*****************************************************************************/

/* Prepare to export an image.	If the plug-in module needs only a limited
   amount of memory, it can lower the value of the 'maxData' field. */

void DoPrepare (GPtr globals)
{
	
	if (gStuff->maxData > 0x80000)
		gStuff->maxData = 0x80000;
	
}



/*****************************************************************************/
void PrintExportFile(GPtr globals)
{
char *fn =  tmp_file;
char gprint[MAX_PATH];
char gprint_path[MAX_PATH+1]={0};

DWORD rz=GetEnvironmentVariable("GPRINT", gprint_path, MAX_PATH);
if(rz==0)//переменной нет - путь прописан в PATH
	sprintf(gprint,"gprint.exe -i%s",fn);
else
	sprintf(gprint,"%s -i%s",gprint_path, fn);


STARTUPINFO si;
PROCESS_INFORMATION pi;

ZeroMemory( &si, sizeof(si) );
si.cb = sizeof(si);
ZeroMemory( &pi, sizeof(pi) );

// Start the child process. 
if( !CreateProcess( NULL,   // No module name (use command line). 
   TEXT(gprint), // Command line. 
   NULL,             // Process handle not inheritable. 
   NULL,             // Thread handle not inheritable. 
   FALSE,            // Set handle inheritance to FALSE. 
   NORMAL_PRIORITY_CLASS,                // Creation flags. 
   NULL,             // Use parent's environment block. 
   NULL,             // Use parent's starting directory. 
   &si,              // Pointer to STARTUPINFO structure.
   &pi )             // Pointer to PROCESS_INFORMATION structure.
   ) 
{
	//printf( "CreateProcess failed (%d).\n", GetLastError() );
	return;
}


// Wait until child process exits.
//WaitForSingleObject( pi.hProcess, INFINITE );

// Close process and thread handles. 
CloseHandle( pi.hProcess );
CloseHandle( pi.hThread );
}



/*****************************************************************************/
/* Requests pointer to the first part of the image to be filtered. */
void DoStart (GPtr globals)
{
	
	/* This plug-in does not support bitmap images */

	if (gStuff->imageMode == plugInModeBitmap)
	{
		gResult = exportBadMode;
		return;
	}
	
	ValidateParameters(globals);
	
	/* check with the scripting system whether to pop our dialog */
	
	gQueryForParameters = ReadScriptParams (globals);
	

	//if (!DoUI (globals))
	//	return;
		
	if (!CreateExportFile (globals))
		return;
	
	WriteExportFile (globals);
	
	CloseExportFile (globals);

	PrintExportFile(globals);
	

	MarkExportFinished (gStuff);

}

/*****************************************************************************/

/* Filters the area and requests the next area. */

void DoContinue (GPtr globals)
{
	
	/* We shouldn't get here because we did all of the work during the
	   start phase, but we add some code just in case. */
	
	MarkExportFinished (gStuff);
	
	gResult = userCanceledErr;

}

/*****************************************************************************/

/* This routine will always be called if DoStart does not return an error
   (even if DoContinue returns an error or the user aborts the operation).
   This allows the module to perform any needed cleanup.  None is required
   in this example. */

void DoFinish (GPtr globals)
{
	WriteScriptParams (globals);
}

/*****************************************************************************/

#define RANGE_ITER(lower,upper,first,last,step)								  \
	for (lower = (first);                         							  \
		 (upper = (((lower) + (short)(step) < (last)) ? (lower) + (short)(step) : (last))), \
		 lower < (last);													  \
		 lower = upper)

/*****************************************************************************/
void WriteImageHead(GPtr globals)
{
	gpraw_t gpraw_head;

	char color_space[256];
	switch(gStuff->imageMode)
	{
	case plugInModeGrayScale:
		strcpy(color_space,"GRAYSCALE");//MessageBox(0,"Gray","",MB_OK);
		break;
	case plugInModeIndexedColor:
	case plugInModeRGBColor:
		strcpy(color_space,"RGB");//MessageBox(0,"RGB","",MB_OK);
		break;
	case plugInModeCMYKColor:
		strcpy(color_space,"CMYK");//MessageBox(0,"CMYK","",MB_OK);
		break;
	case plugInModeLabColor:
		strcpy(color_space,"LAB");//MessageBox(0,"Lab","",MB_OK);
		break;
	case plugInModeMultichannel:
		strcpy(color_space,"RAW");//MessageBox(0,"raw","",MB_OK);
		break;

	default:
		strcpy(color_space,"unknown");
		break;

	}

	string docname;
	OSErr err=PIGetDocumentName(docname);
	if (err==noErr)
	{
		char *cdocname=(char*)docname.c_str();
		char * fn=strrchr(cdocname,'\\');
		if (fn==NULL)
		{
			strcpy(gpraw_head.document_name,cdocname);
		}
		else 
		{
			strcpy(gpraw_head.document_name,fn+1);
		}
	}


	gpraw_head.w=gStuff->imageSize32.h;
	gpraw_head.h=gStuff->imageSize32.v;
	gpraw_head.planes=gStuff->planes;
	gpraw_head.depth=gStuff->depth;
	gpraw_head.xdpi=gStuff->imageHRes>>16;
	gpraw_head.ydpi=gStuff->imageVRes>>16;
	strcpy(gpraw_head.color_space,color_space);
	if (gStuff->iCCprofileData!=NULL)
	{
		gpraw_head.size_icc=gStuff->iCCprofileSize;
		gpraw_head.offset_icc=gpraw_head.offset_data+
			(gpraw_head.w*gpraw_head.h*gpraw_head.planes*gpraw_head.depth/8);
	}
	else
	{
		gpraw_head.offset_icc=0;
	}

	std::string name;
	for(int32 i=0;i<gStuff->planes;i++)
	{
		OSErr err=PIGetChannelName(i, name);
		strcpy(gpraw_head.channel_name[i],name.c_str());
	}

	long buf_len=sizeof(gpraw_t);
	FSWrite(gFRefNum, &buf_len, &gpraw_head);
}



Boolean WriteExportFile (GPtr globals)
{
	/* We write out the file as an interleaved raw file. */ 
	/* We need to figure out how many rows to write at one time. */
	
	long chunk = gStuff->maxData / gStuff->imageSize.h / gStuff->planes;// / (gStuff->depth/8);
	
	ExportRegion region;
	
	region.rect.left = 0;
	region.rect.right = gStuff->imageSize.h;
	region.loPlane = 0;
	region.hiPlane = gStuff->planes - 1;


	WriteImageHead(globals);

	RANGE_ITER (region.rect.top, region.rect.bottom,
				0, gStuff->imageSize.v, chunk)
		{
		
		int16 row;
		long rowCount = gStuff->imageSize.h * (long) gStuff->planes * (gStuff->depth/8);
		void *data = 0;
		int32 rowBytes = 0;
		unsigned8 *rowData;
		
		if (!TSC (TestAbort ())) return FALSE;

		if (!TSR (FetchData (gStuff, &region, &data, &rowBytes))) return FALSE;
		
		for (row = region.rect.top, rowData = (unsigned8 *) data;
			 row < region.rect.bottom;
			 ++row, rowData += rowBytes)
			{
			
			long count = rowCount;
			
			PIUpdateProgress (row, gStuff->imageSize.v);
			
			if (!TSC (TestAbort ())) return FALSE;
			
			if (gStuff->depth==16)//тут  исправляем 15 бит -> 16
			{
				WORD *buf=(WORD*)rowData;
				for(long i=0;i<(count/2);i++)
					buf[i]=(buf[i]*65535+16384)/32768;//формула с сайта photoshop dev.
			}
			//инвертируем CMYK и мультиколор
			if (gStuff->imageMode==plugInModeCMYKColor || 
				gStuff->imageMode==plugInModeMultichannel)
			{
				if(gStuff->depth==8)
				{
					for(long i=0;i<count;i++)
						rowData[i]= 255-rowData[i];
				}
				if(gStuff->depth==16)
				{
					WORD *buf=(WORD*)rowData;
					for(long i=0;i<(count/2);i++)
						buf[i]=65535-buf[i];			
				}
			}

			if (!TSR (FSWrite (gFRefNum, &count, rowData))) return FALSE;			
			}
		
		}
	//Запишем профайл если он есть
	if (gStuff->iCCprofileData!=NULL)
	{
		char * icc=*(gStuff->iCCprofileData);
		long icc_size=gStuff->iCCprofileSize;
		if (!TSR (FSWrite (gFRefNum, &icc_size, icc))) return FALSE;
	}
	
	return TRUE;
	
}

/*****************************************************************************/

OSErr FetchData (ExportRecord *stuff,
				 ExportRegion *region,
				 void **data,
				 int32 *rowBytes)
{
	
	OSErr result;
	
	if (!WarnHostAdvanceStateAvailable (stuff->advanceState))
		return userCanceledErr;
		
	stuff->theRect = region->rect;
	stuff->loPlane = region->loPlane;
	stuff->hiPlane = region->hiPlane;
	
	result = (*(stuff->advanceState)) ();
	
	if (result != noErr)
		{
		*data = NULL;
		*rowBytes = 0;
		}
	else
		{
		*data = stuff->data;
		*rowBytes = stuff->rowBytes;
		}
		
	return result;
	
}

/*****************************************************************************/

void MarkExportFinished (ExportRecord *stuff)
	{
	
	PISetRect (&stuff->theRect, 0, 0, 0, 0);
	
	}

//-------------------------------------------------------------------------------
