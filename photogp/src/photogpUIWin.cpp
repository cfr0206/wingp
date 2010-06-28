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
/*
	File: photogpUIWin.cpp

	Copyright 1990-1991, Thomas Knoll.
	All rights reserved.

	Windows platform dependent C source file for Export example.
*/

#include "photogp.h"
#include "DialogUtilities.h"

#include <io.h>
#ifndef _INC_TCHAR
#include <tchar.h>
#endif	//

extern bool CreateDirectoryPath( LPCTSTR pcszDirectory );

/*****************************************************************************/

const uint8 kDEditText = 4;

char tmp_file[MAX_PATH+1];//полный путь к созданному файлу с данными
/*****************************************************************************/

DLLExport BOOL WINAPI ExportProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);

/*****************************************************************************/

void DoAbout (AboutRecordPtr about)
{
	
	ShowAbout(about);
	//DoGPUI(0);
}

/*****************************************************************************/

Boolean DoUI (GPtr globals)
{
	char *my_lpstrFile = (char *)gStuff->filename;
	char my_filename[256];
	char my_lpstrFileTitle[256];
	char my_extension[_MAX_EXT] = ".gpraw";
	char my_lpstrInitialDir[256];
	char my_dir[256];
	const char *my_lpstrDefExt = "gpraw";
	const char *my_lpstrFilter = "gpraw (*.gpraw)\0*.gpraw\0\0\0";
	const char *my_lpstrTitle = "Output gpraw";
	PlatformData *platform;
	int nResult;
	int len = gStuff->filename[0];
	short hlen;
	OPENFILENAME lpofn;

	gStuff->filename[len+1] = 0; // make sure null terminated

	if (gAliasHandle != NULL)
	{
		/* we've been handed an aliasHandle. Use that instead of querying */
		PIHandle2PString(gAliasHandle, (unsigned char *)&my_lpstrInitialDir);
		hlen = my_lpstrInitialDir[0];

		if (my_lpstrInitialDir [hlen] == '/' || my_lpstrInitialDir [hlen] == '\\')
		{
			_splitpath((const char *)&gStuff->filename[1],
					   NULL, /* drive */
					   NULL, /* dir */
					   (Ptr)gStuff->filename,
					   NULL); /* ext */
			AppendString((unsigned char *)&my_lpstrInitialDir, gStuff->filename, 0, (short)strlen((const char *)gStuff->filename));
			AppendCharToString((unsigned char *)&my_lpstrInitialDir, '.');
			AppendString((unsigned char *)&my_lpstrInitialDir, (const unsigned char *)my_lpstrDefExt, 0, (short)strlen(my_lpstrDefExt));
			hlen = my_lpstrInitialDir[0];
		}
		else my_lpstrInitialDir [hlen+1] = 0;
		
		gStuff->filename[ (gStuff->filename[0] = 0)+1 ] = 0;
		AppendString(gStuff->filename, 
					 (const unsigned char *)&my_lpstrInitialDir, 1,
					 hlen);
		gStuff->filename[ gStuff->filename[0]+1 ] = 0;
		/* makes string pascal and null terminated */

		/* This has to be a Photoshop handle, since it was handed
		   to us by the host, so dispose it (it'll be recreated
		   later.) */
		PIDisposeHandle((Handle)gAliasHandle);
		gAliasHandle = NULL;
	}
	else
		PICopy (my_lpstrInitialDir, &gStuff->filename[1], (short)gStuff->filename[0]);
	
	_splitpath((const char *)&gStuff->filename[1],
			   my_lpstrInitialDir, /* drive */
			   my_dir, /* dir */
			   (Ptr)gStuff->filename,
			   my_extension); /* ext */
	strncat (my_lpstrInitialDir, my_dir, 255); 
	
	platform = (PlatformData *)((ExportRecordPtr) gStuff)->platformData;

	if (!gQueryForParameters)
	{
		strncat(my_lpstrInitialDir, (const char *)gStuff->filename, 255);
		strncat(my_lpstrInitialDir, ".", 2);
		strncat(my_lpstrInitialDir, my_lpstrDefExt, 5);
		strncpy((Ptr)gStuff->filename, my_lpstrInitialDir, 255);
		return TRUE;
	}
	else // gQueryForParameters
	{
		/* initialize OPENFILENAME structure */

		strncpy(my_filename, (const char *)gStuff->filename, 255);
		strncat(my_filename, ".", 2);
		strncat(my_filename, my_lpstrDefExt, 5);

		lpofn.lStructSize = sizeof(lpofn);
		lpofn.hwndOwner = (HWND)platform->hwnd;
		lpofn.hInstance = GetDLLInstance();
		lpofn.lpstrFilter = my_lpstrFilter;
		lpofn.lpstrCustomFilter = NULL;
		lpofn.nMaxCustFilter = 0;
		lpofn.nFilterIndex = 0;
		lpofn.lpstrFile = my_lpstrFile;
		lpofn.nMaxFile = 255;
		lpofn.lpstrFileTitle = my_lpstrFileTitle;
		lpofn.nMaxFileTitle = 255;
		lpofn.lpstrInitialDir = my_lpstrInitialDir;
		lpofn.lpstrTitle = my_lpstrTitle;
		lpofn.Flags = OFN_LONGNAMES |
					  OFN_HIDEREADONLY | 
					  OFN_PATHMUSTEXIST |
					  OFN_OVERWRITEPROMPT;
		lpofn.nFileOffset = 0;
		lpofn.nFileExtension = 0;
		lpofn.lpstrDefExt = my_lpstrDefExt;
		lpofn.lCustData = 0;
		lpofn.lpfnHook = NULL;
		lpofn.lpTemplateName = NULL;
		lpofn.lStructSize = sizeof(lpofn);

		if (!(nResult = GetSaveFileName(&lpofn)))
			gResult = userCanceledErr;
		else
			gSameNames = (strncmp(my_filename, my_lpstrFileTitle, 255) == noErr);

		return (nResult);
	}
}


/*****************************************************************************/



Boolean CreateExportFile (GPtr globals)
	{
	
	OFSTRUCT ostr;
	//OSErr  result;
	DWORD rz;
	HFILE file;
	char *filename;

//-------------------------------------------------------------------------
	char tmp_path[MAX_PATH+1];
	
	/* Get Temp folder 
	- проверяем переменную среды GPRINT_TEMP
		- если есть проверяем существование пути
			- если пути нет - создаём
	-если переменной нет - получает системный путь
	*/
	rz=GetEnvironmentVariable("GPRINT_TEMP", tmp_path, MAX_PATH);
	if (rz==0)
		{GetTempPath(MAX_PATH, tmp_path);}
	else
	{
		CreateDirectoryPath(tmp_path);
	}

	/* Create temp file in temp folder*/
	rz = GetTempFileName(tmp_path, "GPR", 0, tmp_file);
//-------------------------------------------------------------------------


	/* Delete the file if it exists. */

	gFRefNum = 0;

	filename = (char *)tmp_file;// gStuff->filename;
	//result = OpenFile(filename,&ostr,(WORD)OF_DELETE);

	//if (result == -1)
	//	{
	//	if (ostr.nErrCode != ENOENT)
	//		{
	//		gResult = openErr;
	//		return FALSE;
	//		}
	//	}


	/* Create a new file */

	file = OpenFile(filename,&ostr,OF_CREATE|OF_WRITE|OF_SHARE_EXCLUSIVE);

	if (file == -1)
		{
		gResult = openErr;
		return FALSE;
		}

	else
		{
		gFRefNum = file;
		return TRUE;
		}

	}
	

/*****************************************************************************/

Boolean CloseExportFile (GPtr globals)
{

	//char *filename;
	//char my_lpstrInitialDir[256];
	//char my_dir[256];
	//char my_extension[_MAX_EXT] = ".gpraw";
	//int	len = 0;

	//OFSTRUCT	ostr;
	
	if (gFRefNum != 0)
		_lclose(gFRefNum);

	/* Delete the file if we had an error. */

	//if (gResult != noErr)
	//	{
	//	filename = (char *) gStuff->filename;
	//	OpenFile(filename,&ostr,(WORD)OF_DELETE);
	//	}
		
	/* Mark the file as clean. */
	
	gStuff->dirty = FALSE;
	
	/* Create alias for scripting system */

	//strncpy(my_lpstrInitialDir, (const char *)gStuff->filename, 255);
	//
	//len = strlen((const char *)gStuff->filename)+1;

	//if (gSameNames)
	//{
	//	_splitpath((const char *)gStuff->filename,
	//			   my_lpstrInitialDir, /* drive */
	//			   my_dir, /* dir */
	//			   (Ptr)gStuff->filename,
	//			   my_extension); /* ext */
	//	strncat(my_lpstrInitialDir, my_dir, 255);
	//	len = strlen(my_lpstrInitialDir)+1;
	//}
	//	
	//my_lpstrInitialDir [len] = 0;

	/* If same names, then ignore filename.  Else, leave it in. */

	/* uncomment out to make a pascal string
	gStuff->filename[0] = len;
	strncpy(&gStuff->filename[1], my_lpstrInitialDir, len);
	*/
	//strncpy((Ptr)gStuff->filename, my_lpstrInitialDir, len); // comment out for pascal
	//gStuff->filename[len+1] = 0; // just in case


	//gAliasHandle = PINewHandle(len+1);
	//if (gAliasHandle != NULL)
	//{
	//	HLock(gAliasHandle);
	//	strncpy(*gAliasHandle, (const char *)gStuff->filename, len+1);
	//	HUnlock(gAliasHandle);
	//}

	return TRUE;
	
}

/*****************************************************************************/
/* Dispose alias handle */

void DestroyAliasHandle (GPtr globals)
{
	if (gAliasHandle != NULL)
		PIDisposeHandle((Handle)gAliasHandle);
	gAliasHandle = NULL;
}

/* Dialog box code to ask for export parameters. */

DLLExport BOOL WINAPI ExportProc(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)   // Win32 Change
{
	 static char *filename;
	 int idd;	// Win32 Change

    switch  (wMsg)
	{
		case WM_INITDIALOG:
            // Initialize file name with default values
			filename = (char *)lParam;
            StuffText(hDlg, kDEditText, (unsigned char *)&filename);
            CenterDialog(hDlg);

            return  FALSE;

		case WM_COMMAND   :
	  		idd = COMMANDID (wParam);              // WIN32 Change
            switch  (idd)
			{
				case OK:
					FetchText(hDlg, kDEditText, (unsigned char *)&filename);
					/* drop into cancel */
				case CANCEL:
					EndDialog(hDlg, idd);
					break;
				default:
                    return FALSE;
            }
            break;

		default:
			return  FALSE;
	}
    return  TRUE;
}



bool CreateDirectoryPath( LPCTSTR pcszDirectory )
{
	//	Must be passed a valid string!
	//ASSERT_VALID_STR( pcszDirectory );

	static TCHAR cSlash = _T('\\');

	bool bRetVal = false;

	const int nLength = _tcslen( pcszDirectory ) + 1;
	LPTSTR pszDirectoryPath = (LPTSTR)malloc( nLength * sizeof( TCHAR ) );
	if( pszDirectoryPath )
	{

		LPCTSTR pcszNextDirectory = pcszDirectory;

		//
		//	Determine if the path is a UNC path. We do this by looking at the first two bytes
		//	and checkin they are both backslashes
		if( nLength > 2 && *pcszNextDirectory == cSlash && *(pcszNextDirectory+1) == cSlash )
		{
			//	We need to skip passed this bit and copy it into out local path.
			//	"\\Russ\C\"
			pcszNextDirectory += 2;
			while( *pcszNextDirectory && *pcszNextDirectory != cSlash )	pcszNextDirectory++;
			pcszNextDirectory++;
			while( *pcszNextDirectory && *pcszNextDirectory != cSlash )	pcszNextDirectory++;
			_tcsncpy( pszDirectoryPath, pcszDirectory, pcszNextDirectory - pcszDirectory );
			pszDirectoryPath[ pcszNextDirectory - pcszDirectory ] = '\000';
		}

		//
		//	Set the return value to true because the nly thing that can fail now is the
		//	CreateDirectory. If that fails then we change the return value back to fals.
		bRetVal = true;

		//
		//	Now, loop over the path, creating directories as we go. If we fail at any point then get out of the loop
		do
		{
			if( *pcszNextDirectory )
				pcszNextDirectory++;

			while( *pcszNextDirectory && *pcszNextDirectory != cSlash )
				pcszNextDirectory++;

			_tcsncpy( pszDirectoryPath, pcszDirectory, pcszNextDirectory - pcszDirectory );
			pszDirectoryPath[ pcszNextDirectory - pcszDirectory ] = '\000';

			if( _taccess( pszDirectoryPath, 0 ) )
			{
				if( !CreateDirectory( pszDirectoryPath, NULL ) )
				{
					bRetVal = false;
					break;
				}
			}
		}
		while( *pcszNextDirectory );

		free( pszDirectoryPath );
		pszDirectoryPath = NULL;
	}
	return bRetVal;
}
// end photogpUIWin.cpp