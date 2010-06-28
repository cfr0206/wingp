/***********************************************************************/
/*                                                                     */
/* ASTypes.h                                                           */
/* Adobe Standard Types                                                */
/*                                                                     */
/* Copyright 1996-1999 Adobe Systems Incorporated.                     */
/* All Rights Reserved.                                                */
/*                                                                     */
/* Patents Pending                                                     */
/*                                                                     */
/* NOTICE: All information contained herein is the property of Adobe   */
/* Systems Incorporated. Many of the intellectual and technical        */
/* concepts contained herein are proprietary to Adobe, are protected   */
/* as trade secrets, and are made available only to Adobe licensees    */
/* for their internal use. Any reproduction or dissemination of this   */
/* software is strictly forbidden unless prior written permission is   */
/* obtained from Adobe.                                                */
/*                                                                     */
/* Started by Dave Lazarony, 01/26/1996                                */
/*                                                                     */
/***********************************************************************/


#ifndef __ASTypes__
#define __ASTypes__

/*
 * Includes
 */
 
#ifndef __ASConfig__
#include "ASConfig.h"
#endif

#ifndef __ASPragma__
#include "ASPragma.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

#pragma PRAGMA_ALIGN_BEGIN
#pragma PRAGMA_IMPORT_BEGIN


/*
 * Constants
 */


// error codes
#define kNoErr					0
#define kOutOfMemoryErr			'!MEM'
#define kBadParameterErr		'PARM'
#define kNotImplementedErr		'!IMP'
#define kCantHappenErr			'CANT'


// AMPAPI  Adobe Standard Plugin API calling convention.

#ifndef AMPAPI
	#ifdef MAC_ENV
		#define ASAPI pascal
	#endif
	#ifdef WIN_ENV
		#define ASAPI
	#endif
#endif

// C calling convention for those places that need it.
// This doesn't really do anything, but is  more for
// an explicity declaration when it matters.
#define ASCAPI	


/*
 * Types
 */

// Integer Types

#ifndef _H_CoreExpT		// these conflict with PDFL
	typedef signed char ASInt8;
	typedef signed short ASInt16;
	typedef signed long ASInt32;
#endif

typedef unsigned char ASUInt8;
typedef unsigned short ASUInt16;
typedef unsigned long ASUInt32;

typedef long ASErr;

// Storage Types

typedef unsigned char ASByte;
typedef ASByte* ASBytePtr;

// Unicode Types
typedef ASUInt16 ASUnicode;

// Pointer Types

typedef void* ASPtr;
typedef void** ASHandle;

// Fixed Types

typedef long ASFract;
typedef float ASReal;

#ifndef _H_ASExpT	// these conflict with PDFL
	
	typedef	long ASFixed;

	typedef struct _t_ASFixedPoint {
		ASFixed h, v;
	} ASFixedPoint;

	typedef struct _t_ASFixedRect {
		ASFixed left, top, right, bottom;
	} ASFixedRect;

	typedef struct _t_ASFixedMatrix {
		ASFixed a, b, c, d, tx, ty;
	} ASFixedMatrix;

	typedef struct _t_ASRealPoint {
		ASReal h, v;
	} ASRealPoint;

	typedef struct _t_ASRealRect {
		ASReal left, top, right, bottom;
	} ASRealRect;

	typedef struct _t_ASRealMatrix {
		ASReal a, b, c, d, tx, ty;
	} ASRealMatrix;

#endif


// Platform Structures

#ifdef MAC_ENV
	
	// ASBoolean is the same a Macintosh Boolean.
	typedef unsigned char ASBoolean; 

	#ifndef _H_ASExpT	// these conflict with PDFL
		// ASPortRef is the same as a Macintosh GrafPtr.
		#if Platform_Carbon
			typedef struct OpaqueGrafPtr* ASPortRef;
		#else
			typedef struct GrafPort* ASPortRef;
		#endif

		// ASWindowRef is the same as a Macintosh WindowPtr.
		#if Platform_Carbon
			typedef struct OpaqueWindowPtr* ASWindowRef;
		#else
			typedef struct GrafPort* ASWindowRef;
		#endif

	#endif

	// ASRect is the same size and layout as a Macintosh Rect.
	typedef struct _t_ASRect {
		short top, left, bottom, right;
	} ASRect;

	// ASPoint is the same size and layout as a Macintosh Point.
	typedef struct _t_ASPoint {
		short v, h;
	} ASPoint;

#endif


#ifdef WIN_ENV
		
		// ASBoolean is the same a Windows BOOL.
		typedef int ASBoolean;

	#ifndef _H_ASExpT			// these conflict with PDFL
		    
		// ASPortRef is the same as a Windows HDC.
		typedef void* ASPortRef;				

		// ASWindowRef is the same as a Windows HWND.
		typedef void* ASWindowRef;			

	#endif

	// ASRect is the same size and layout as a Windows RECT.
	typedef struct _t_ASRect {
		long left, top, right, bottom;
	} ASRect;

	// ASPoint is the same size and layout as a Windows POINT.
	typedef struct _t_ASPoint  {
		long h, v;
	} ASPoint;

#endif

// ASRGBColor is the same as a Macintosh RGBColor on Macintosh and Windows.
typedef struct _t_ASRGBColor {
	unsigned short red, green, blue;
} ASRGBColor;


// AIEvent is the same as a Macintosh EventRecord on Macintosh and Windows.
typedef struct _t_ASEvent {
	unsigned short	what;
	unsigned long	message;
	unsigned long	when;
	ASPoint			where;
	unsigned short	modifiers;
} ASEvent;

// This is a generic reference to a resource/plugin file.  If not otherwise stated,
// it is assumed to be equivalent to an SPPluginRef (see "SPPlugs.h")
typedef struct ASAccess* ASAccessRef;



#pragma PRAGMA_IMPORT_END
#pragma PRAGMA_ALIGN_END

#ifdef __cplusplus
}
#endif


#endif
