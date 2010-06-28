// ADOBE SYSTEMS INCORPORATED
// Copyright  2007 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this 
// file in accordance with the terms of the Adobe license agreement
// accompanying it.  If you have received this file from a source
// other than Adobe, then your use, modification, or distribution
// of it requires the prior written permission of Adobe.
//-------------------------------------------------------------------
//
//	File:
//		PI3D.h
//
//	Copyright 2007, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Distribution:
//		PUBLIC
//
//	Description:
//		This file describes version 1 of Photoshop's plug-in 3d interface.
//
//	Use:
//		Include in all plug-ins using 3D.
//		
//

#ifndef __PI3D__
#define __PI3D__

#if PRAGMA_ONCE
#pragma once
#endif

#include "PIActions.h"
#include "PIGeneral.h"
#include "SPBasic.h"
#include "SPFiles.h"


/* Operation selectors */
#define formatSelectorPrepareRenderer		100
#define formatSelectorRender				101
#define formatSelectorFinishRenderer		102
#define formatSelectorSetObjectPosition		103
#define formatSelectorGoToCamera			104
#define formatSelectorSetLightMode			105
#define formatSelectorSetRenderMode			106
#define formatSelectorSetCrossSection		107
#define formatSelectorUpdateTexture			108
#define formatSelectorSetObjectScale		109
#define formatSelectorPreprocessU3D			110
#define formatSelectorGetDataFromEngine		111
#define formatSelectorReloadScene			112
#define formatSelectorGetTexturePointer		113
#define formatSelectorUpdateTextureInMem	114
#define formatSelectorSaveTexture			115
#define formatSelectorReleaseResources		116

/*	We keep various pieces of information about the file format in the PiMI resource.
	Here is the structure of that resource. */

/******************************************************************************/
#define PIFmtSupports3DProperty				'fm3d'		/* 'fm3d' plug-in supports 3D */


#define k3DXAxis 0
#define k3DYAxis 1
#define k3DZAxis 2

#define kMax3DTextures				4096
#define kMax3DInterfaceStringLength 512
#define kMax3DURLStringLength		2048

#define kRenderQualityHigh 0
#define kRenderQualityLow 1

#define k3DToolOrbit 0
#define k3DToolRotate 1
#define k3DToolPan 2
#define k3DToolWalk 3
#define k3DToolZoom 4
#define k3DToolCrossSection 5

#define k3DDataTypeU3D 0
#define k3DDataType3DS 1
#define k3DDataTypeDAE 2
#define k3DDataTypeKMZ 3

#define k3DMapTypeTexture 0x0001
#define k3DMapTypeReflection 0x0002
#define k3DMapTypeBump 0x0004
#define k3DMapTypeSpecular 0x0008
#define k3DMapTypeOpacity 0x0010
#define k3DMapTypeShininess 0x0020
#define k3DMapTypeSelfIllumination 0x0040


/*	Keys in the 3D Scene Descriptor */

/******************************************************************************/
#define keyZ				'Z   '

#define keyGlobalAmbientRed  'GamR'
#define keyGlobalAmbientGreen	'GamG'
#define keyGlobalAmbientBlue	'GamB'

#define keyGlobalUnits		'Gunt'
#define keyGlobalAnimStart	'Gast'
#define keyGlobalFrames		'Gfrm'
#define keyGlobalFPS		'Gfps'
#define keyGlobalRangeName	'GRNm'

#define keyLightList		'lite'
#define keyLightClass		'litc'
#define keyCameraList		'caml'
#define keyCameraClass		'camc'
#define keyMeshList			'mshl'
#define keyMeshClass		'msho'
#define keyMaterialList		'mtll'
#define keyMaterialClass	'mtlo'

#define keyVertexCount		'verc'
#define keyPolyCount		'plyc'

#define keyMultiple			'mult'
#define keyTargetX			'tarx'
#define keyTargetY			'tary'
#define keyTargetZ			'tarz'
#define keyHotspot			'hots'
#define keyShadow			'shdw'
#define keyAttenuation		'attn'
#define keyAttenType		'attt'
#define keyAttenA			'atta'
#define keyAttenB			'attb'
#define keyAttenC			'attc'
#define keyOuterRadius		'orad'
#define keyInnerRadius		'irad'
#define keyBank				'bank'
#define keyOrtho			'orth'
#define keyApsect			'aspr'
#define keyZoomFactor		'zmfc'
#define keyFlags			'flag'
#define keySmoothing		'smth'
#define key3DIndexList		'indl'
#define key3DIndex			'indx'
#define keyHidden			'hidn'
#define keyHasMatrix		'hmat'
#define keyHasSmoothing		'hsmt'
#define keyAmbientRed		'ared'
#define keyAmbientGreen		'agrn'
#define keyAmbientBlue		'ablu'
#define keyDiffuseRed		'dred'
#define keyDiffuseGreen		'dgrn'
#define keyDiffuseBlue		'dblu'
#define keySpecularRed		'sred'
#define keySpecularGreen	'sgrn'
#define keySpecularBlue		'sblu'
#define keyEmissiveRed		'ered'
#define keyEmissiveGreen	'egrn'
#define keyEmissiveBlue		'eblu'
#define keyShininess		'shin'
#define keyShininess2		'shi2'
#define keyReflection		'refl'
#define keySelfIllumination	'self'
#define keyShading			'shad'
#define keyTwoSide			'twos'
#define keyWireframe		'wire'
#define keyDecal			'decl'
#define key3DSStyle			'msty'
#define keyWireframeSize	'wfsz'
#define keyUScale			'uscl'
#define keyVScale			'vscl'
#define keyUOffset			'uoff'
#define keyVOffset			'voff'
#define keyFullMapName		'fMnM'
#define keyVertexList		'verl'
#define keyNormalList		'nrml'
#define keyUVList			'uvl '
#define keylineColorList	'lncl'
#define keylineMaterialList 'lnmt'
#define keyVertexColorList	'vnvl'
#define keyLineList			'lnfl'
#define keyFaceList			'facl'
#define keyFaceIndexes		'faci'
#define keyMapList			'mapl'
#define keyMapClass			'mapo'
#define keyMaterialIndList	'matl'
#define keyMaterialIndObj	'mato'

#define keyKeys				'KeyS'
#define keyFrameNumber		'FrNm'
#define keyFlagsAnim		'FlAn'
#define keyNameAnim			'FlNn'
#define keyTension			'KtEn'
#define keyBias				'KBia'
#define keyEasto			'Keto'
#define keyEasefrom			'Kfro'

#define keyRotationRad		  'RoRd'
#define keyAnimVectorX        'vctX'
#define keyAnimVectorY        'vctY'
#define keyAnimVectorZ        'vctZ'
#define keyAnimVectorObject   'vctO'
#define keyAnimationDataClass 'AniC'
#define keyAnimActionClass    'AnAc'
#define keyInstanceName		  'InsN'
#define keyFlags1			  'flgO'
#define keyFlags2			  'flgT'
#define keyU3DNodeID		  'NoID'
#define keyU3DParentID        'PrID'
#define keyU3DType			  'tYpE'
#define keyPivotX			  'PvtX'
#define keyPivotY			  'PvtY'
#define keyPivotZ			  'PvtZ'
#define keyPivotObject        'PvtO'
#define keyPivotList		  'PvLs'
#define keyHasPivot			  'PvPr'
#define keyHasRange			  'RgBl'
#define keyHasLocalMatrix	  'LcMt'
#define keyRangeName		  'RgNm'
#define keyframePositionList  'PosL'
#define keyframeScaleList     'SclL'
#define keyframeRotationList  'RotL'
#define keyframeFOVList		  'FOVl'
#define keyframeRollList	  'RolL'
#define keyKeyFrameList		  'KeFL'
#define keyKeyFrameClass      'KeCS'
#define keyAnimationList	  'AnLs'
#define keyframePositionActionList 'acPs'
#define keyframeScaleActionList    'acSc'
#define keyframeRotationActionList 'acRt'
#define keyframeFOVActionList      'acFV'
#define keyframeRollActionList     'acRl'
#define keyframeHotActionList		'acHt'
#define keyframeHotList				'HotL'
#define keyframeFalloffActionList	'acFo'
#define keyframeFalloffList			'FalL'
#define keyframeHideActionList		'acHd'
#define keyframeHideList			'HidL'
#define keyframeMorphActionList		'acMp'
#define keyframeMorphList			'MorL'
#define keyframeColorActionList		'acCl'
#define keyframeColorList			'ColL'

#define keyAnimFrameNumber			'KAfn'
#define keyAnimFrameFlags			'KAff'
#define keyAnimFrameTension			'KAft'
#define keyAnimFrameContinuity		'KAfc'
#define keyAnimFrameBias			'KAfb'
#define keyAnimFrameEaseTo			'KAet'
#define keyAnimFrameEaseFrom		'KAef'
#define keyAnimFrameRotation		'KAro'
#define keyAnimFrameValue			'KAvl'
#define keyAnimFrameVectorX			'KAvx'
#define keyAnimFrameVectorY			'KAvy'
#define keyAnimFrameVectorZ			'KAvz'
#define keyAnimFrameUseQuat			'KAuq'
#define keyAnimFrameQuatW			'KAqw'
#define keyAnimFrameQuatX			'KAqx'
#define keyAnimFrameQuatY			'KAqy'
#define keyAnimFrameQuatZ			'KAqz'
#define keyAnimFrameClass			'KAFC'
#define keyAnimFrameList			'KAFL'

#define keventToolStartStr "keventToolStart"
#define keventToolStopStr "keventToolStop"
#define krenderStateStr "krenderState"
#define krenderFunctionStr "krenderFunction"
#define ktool3DStr "tool3D"

#define kFrameReaderClass						"FrameReader"
#define kFrameReaderTypeKey						"frameReaderType"

#define kDescVersionKey							"descVersion"
#define kDocumentSizeKey						"documentSize"
#define key3DSceneKey							"key3DScene"
#define key3DDataKey							"key3DData" 
#define key3DFileNameKey						"key3DFileName"
#define key3DFileListKey						"fileList"
#define key3DMeshTexturePathKey					"key3DMeshTexturePath"
#define key3DTexturesExternalKey				"key3DTexturesExternal" 
#define key3DTexturesVisibleKey					"key3DTexturesVisible" 
#define key3DTextureListKey						"key3DTextureList"
#define key3DTextureObjectKey					"key3DTextureObject"
#define key3DTextureNameKey						"key3DTextureName"
#define key3DTexturePathKey						"key3DTexturePath"
#define key3DTextureDataKey						"key3DTextureData"
#define key3DTextureVisibleKey					"key3DTextureVisible"
#define key3DTextureTypeKey						"key3DTextureType"
#define key3DDurationKey						"key3DDuration"
#define key3DScriptKey							"key3DScript"
#define key3DState								"key3DState"
#define key3DPositionKey						"key3DPosition"
#define key3DStateListKey						"key3DStateList"
#define key3DStateNameKey						"key3DStateName"
#define key3DXPosKey							"key3DXPos"
#define key3DYPosKey							"key3DYPos"
#define key3DZPosKey							"key3DZPos"
#define key3DXAngleKey							"key3DXAngle"
#define key3DYAngleKey							"key3DYAngle"
#define key3DZAngleKey							"key3DZAngle"
#define key3DFOVKey								"key3DFOV"
#define key3DSpeedKey							"key3DSpeed"
#define key3DCameraDistanceKey					"key3DCameraDistance"
#define key3DCurrentCameraPositionKey			"key3DCurrentCameraPosition" 
#define key3DCurrentFOVKey						"key3DCurrentFOV" 
#define key3DCurrentPositionKey					"key3DCurrentPosition" 
#define key3DCurrentOrthographicKey				"key3DOrthographic" 
#define key3DCurrentOrthographicScaleKey		"key3DOrthographicScale" 
#define key3DCurrentRenderModeKey				"key3DRenderMode"
#define key3DCurrentLightModeKey				"key3DLightMode"
#define key3DCurrentTimeKey						"key3DTime"
#define key3DCurrentCrossSectionKey				"key3DCrossSection"
#define key3DCrossSectionPlaneColorKey			"key3DCrossSectionPlaneColor"
#define key3DCrossSectionIntersectionColorKey	"key3DCrossSectionIntersectionColor"
#define key3DCrossSectionOffsetKey				"key3DCrossSectionOffset"
#define key3DCrossSectionPlaneTilt1Key			"key3DCrossSectionPlaneTilt1"
#define key3DCrossSectionPlaneTilt2Key			"key3DCrossSectionPlaneTilt2"
#define key3DCrossSectionPlaneOpacityKey		"key3DCrossSectionPlaneOpacity"
#define key3DCrossSectionAlignmentKey			"key3DCrossSectionAlignment"
#define key3DCrossSectionEnabledKey				"key3DCrossSectionEnabled"
#define key3DCrossSectionPlaneFlipKey			"key3DCrossSectionPlaneFlip"
#define key3DCrossSectionPlaneVisibleKey		"key3DCrossSectionPlaneVisible"
#define key3DCrossSectionIntersectionVisibleKey	"key3DCrossSectionIntersectionVisible"
#define key3DCurrentObjectXScaleKey				"key3DCurrentObjectXScale"
#define key3DCurrentObjectYScaleKey				"key3DCurrentObjectYScale"
#define key3DCurrentObjectZScaleKey				"key3DCurrentObjectZScale"
#define key3DAuxilaryColorKey					"key3DAuxilaryColor"
#define key3DFaceColorKey						"key3DFaceColor"
#define key3DOpacityKey							"key3DOpacity"
#define key3DLineWidthKey						"key3DLineWidth"
#define key3DCreaseValueKey						"key3DCreaseValue"
#define key3DViewIndexKey						"key3DViewIndex"

#define key3DTextureFunctionPtr					"key3DTextureFunction"
#define key3DTextureSizeFunctionPtr				"key3DTextureSizeFunction"

#define keyPolyCount							'plyc'

#define toolBadMode -31000	/* the tool is not read for external commands */


#if WIN32
#pragma pack(push,4)
#endif

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#endif


/// Callback from device plugin to ask 3D tool to update
typedef MACPASCAL OSErr (*RenderDeviceIdleFunction) ();
													
typedef int16			U3DAlignment;
typedef int16			RenderQuality;
typedef int16			ToolID;
typedef uint16			U3DInterfaceString[512];

typedef struct U3DBoundingBox
	{
	nativeFloat				minX;	
	nativeFloat				maxX;	
	nativeFloat				centerX;	
	
	nativeFloat				minY;	
	nativeFloat				maxY;	
	nativeFloat				centerY;	
	
	nativeFloat				minZ;	
	nativeFloat				maxZ;	
	nativeFloat				centerZ;	

	}
U3DBoundingBox, *U3DBoundingBoxPtr;
	
typedef struct U3DPosition
	{
	nativeFloat				x;								// x location
	nativeFloat				y;								// y location
	nativeFloat				z;								// z location
	nativeFloat				xAngle;							// x angle
	nativeFloat				yAngle;							// y angle
	nativeFloat				zAngle;							// z angle	

/* Reserved for future expansion */
	char					reserved [128]; 				// Set to zero 
	}
U3DPosition, *U3DPositionPtr;

typedef struct U3DCrossSection
	{
	nativeFloat				planeColor[3];
	nativeFloat				intersectionColor[3];
	nativeFloat				offset;
	nativeFloat				planeTilt1;
	nativeFloat				planeTilt2;
	nativeFloat				planeOpacity;
	U3DAlignment			alignmentMode;
	Boolean					crossSectionEnabled;
	Boolean					planeFlip;
	Boolean					planeVisible;
	Boolean					intersectionVisible;

/* Reserved for future expansion */
	char					reserved [512]; 				// Set to zero 
	}
U3DCrossSection, *U3DCrossSectionPtr;


typedef struct 
	{
	uint32					length;							// number of textures
	uint16					**textureNames;					// texture names
	uint16					**texturePaths;					// texture paths
	uint32					*textureType;					// flags to indicate the type of texture
/* Reserved for future expansion */
	char					reserved [508]; 				// Set to zero 
	} 
U3DTextureArray;

typedef struct RenderDrawBuffer
	{
	void				*pixels;
	int32				rowBytes;
	VPoint				sceneSize;
	VRect				tileRect;
	PlaneMap			planeMap;							/* Maps plug-in plane numbers to host plane
														numbers.  The host initializes this is a linear
														mapping.  The plug-in may change this mapping if
														it sees the data in a different order. */
/* Reserved for future expansion */
	char				reserved [512]; 					// Set to zero 

} 
RenderDrawBuffer, *RenderDrawBufferPtr;

typedef struct 
	{
//State Data - set to defaults by plugin and saved in PSD
	U3DPosition			currentCameraPosition;				//Current camera position
	nativeFloat			currentFieldOfView;					//Field of view of camera (degrees)
	nativeFloat			currentObjectXScale;				//Scale applied to the object by the user
	nativeFloat			currentObjectYScale;				//Scale applied to the object by the user
	nativeFloat			currentObjectZScale;				//Scale applied to the object by the user
	U3DPosition			currentObjectPosition;				//Current object position
	Boolean				currentOrthographic;				//Is the camera currently in orthographic mode
	uint32				currentRenderMode;					//Current render mode
	uint32				currentLightMode;					//Current light mode
	nativeFloat			currentTime;						//Current time of the play head in seconds
	U3DCrossSection		currentCrossSection;				//Data to define the current cross section															
	nativeFloat			currentAuxilaryColor[3];			//Line colors in illustration modes
	nativeFloat			currentFaceColor[3];				//Face colors in illustration modes
	nativeFloat			currentOpacity;						//Model opacity
	nativeFloat			currentLineWidth;					//Width of wireframe lines
	nativeFloat			currentCreaseValue;					//Threshold angle for illustration angle
	nativeFloat			currentOrthoScale;					//Scale for orthorgraphic view 1/x

	
//Interaction Data - set by Photoshop per render
	RenderQuality		quality;							//Quality level of final output
	uint32				currentTexture;						//Set before calling update texture
	uint16				currentTexturePath[2048];			//Set before calling update texture

	U3DBoundingBox		currentBoundingBox;					//Set by Plugin on SetObjectPosition
	
	
	RenderDrawBuffer	drawBuffer;							//Used for passing texture updates
	
	RenderDeviceIdleFunction	deviceIdler;				//Set by plugin to request idling
	int32						currentToolMode;			//Kept current by PS.  Set by controller plugin when requesting tool mode change.
	int32						currentToolID;				//Kept current by PS.  Set by controller plugin when requesting tool id change.
	
/* Reserved for future expansion */
	char				reserved [500]; 					// Set to zero 
	}
RenderState, *RenderStatePtr;


typedef struct 
	{
	uint32					length;							// number of cameras
	uint16					**positionNames;				// position names
	RenderState				*viewStates;					// states of current view

/* Reserved for future expansion */
	char					reserved [512]; 				// Set to zero 
	} 
U3DStateArray;

typedef struct 
	{
	void				*engine;							//Pointer to a 3D engine instance
															//This should be passed back from PrepareU3DRenderer
															//It will be provided in all render calls
	Boolean				supportsOrthographic;				//Can this renderer do orthographic?
	Boolean				supportsCrossSection;				//Can this renderer render a cross section?
	uint16				numRenderModes;
	uint16				**renderModeList;					//List of known render modes
	uint16				numLightModes;
	uint16				**lightModeList;					//List of known light modes
	Boolean				useHardware;						//Flag to determine whether or not to use hardware acceleration

/* Reserved for future expansion */
	char				reserved [512]; 					// Set to zero 

	}
RendererInfo, *RendererInfoPtr;


typedef struct RenderRecord
	{
//File Data	- retrieved when the file is loaded and saved in PSD
	SPPlatformFileSpecificationW	*fileSpec2;							//Location of 3D data on disk
	PIDescriptorHandle				sceneDescriptor;					//This will contain the scene data
	SPBasicSuite					*sSPBasic;							//SuitePea basic suite
	uint8							*data;								//Pointer to File Data
																		//This should be filled in on ReadContinue and will be provided on PrepareU3DRenderer and on all writing calls
	Boolean							mustAllocateData;					//If the host allocated the data, we need to allocate a copy of our own
	uint32							dataSize;							//Size of the U3D data
	uint16							dataType;							//Type of data
	nativeFloat						duration;							//Duration of animation in the file in seconds
	U3DStateArray					stateList;							//List of known states
	U3DTextureArray					textureList;						//List of textures in the file
	Boolean							texturesExternal;					//Set to true if the textures were not contained inside the original 3D file
	uint16							*savedScript;						//For File Loading: Any script associated with the file
	
	RenderDrawBuffer				drawBuffer;
//State Data - set to defaults by plugin and saved in PSD	
	RenderState						renderState;
	
//Engine Data - not needed for file and not saved per instance - sent to PS in prepareRenderer
	RendererInfo					rendererInfo;

//Initial Bounding Box of Scene	
	U3DBoundingBox					initialBoundingBox;					//Bounding box on initial load
	
	uint16							*meshTexturePath;					//Path to texture to use for the mesh

/* Reserved for future expansion */
	char							reserved [512]; 					// Set to zero 
	}
RenderRecord, *RenderRecordPtr;

/// Callback from Filter plugin to get texture data
typedef MACPASCAL OSErr (*GetTextureSizeFunction) (int32 index, RenderDrawBuffer *buffer);
typedef MACPASCAL OSErr (*GetTextureFunction) (int32 index, RenderDrawBuffer *buffer);
typedef MACPASCAL OSErr (*RenderUpdate) (RenderStatePtr renderState, const int16 selector);

#if WIN32
#pragma pack(pop)
#endif

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#endif

#endif  // __PIFormat_h__
