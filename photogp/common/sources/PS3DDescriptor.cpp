#define _CRT_SECURE_NO_DEPRECATE 1
#include "PS3DDescriptor.h"
#include "PITerminology.h"
#include "PIStringTerminology.h"

//This is ugly.  We need to expose some of the internals of Photoshop in order to make this work.
//But, it's very late in the cycle and I can't change it now.
//Generally, noone should have to look in here at all.
//Developers should just fill out the data structure in the header and then call PI3DMakeLayerDescriptor or
//take a descriptor and pass it to PI3DGetLayerDataFromDescriptor.  The rest of this is just voodoo that they can hopefully ignore.
//1/26/07 - Pete

OSErr		PI3DMakeStateDescriptor(PI3DDescriptorProcs *procs,RenderState *renderState, uint16 *name, PIActionDescriptor stateDescriptor);
OSErr		PI3DMakeCrossSectionDescriptor(PI3DDescriptorProcs *procs,U3DCrossSection *crossSection,PIActionDescriptor crossSectionDescriptor);
OSErr		PI3DMakePositionDescriptor(PI3DDescriptorProcs *procs,U3DPosition *camera,PIActionDescriptor positionDescriptor);
OSErr		PI3DMakeFrameReaderDescriptor(PI3DDescriptorProcs *procs,PI3DLayer *theLayer, PIActionDescriptor theFrameReaderDescriptor);

OSErr		PI3DParseFrameReaderDescriptor(PI3DDescriptorProcs *procs, PIActionDescriptor theFrameReaderDescriptor,PI3DLayer *theLayer);

OSErr		PI3DGetStateFromDescriptor(PI3DDescriptorProcs *procs, PIActionDescriptor stateDesc, uint16 *viewName,RenderState *state);	
OSErr		PI3DGetCrossSectionFromDescriptor(PI3DDescriptorProcs *procs,PIActionDescriptor crossSectionDescriptor,U3DCrossSection *crossSection);
OSErr		PI3DGetPositionFromDescriptor(PI3DDescriptorProcs *procs,PIActionDescriptor positionDescriptor,U3DPosition *camera);
#define kOriginKey								"origin"

#define kFrameReaderKey							"frameReader"
#define kFrameReaderClass						"FrameReader"
#define kFrameReaderTypeKey						"frameReaderType"
#define kDocumentSizeKey						"documentSize"

#define kDescVersionKey							"descVersion"
#define kPixelSourceClass						"PixelSource"
#define kPixelSourceTypeKey						"pixelSourceType"

const int32 k3DPixelSourceDescriptorVersion = 1;
const OSType kPixelSourceType3D = '3dPS';

const int32 kA3FRDescriptorVersion = 1;
const OSType kFrameReaderTypeAcrobat3D = 'A3FR';
	
OSErr		PI3DGetPositionFromDescriptor(PI3DDescriptorProcs *procs,PIActionDescriptor positionDescriptor,U3DPosition *position)
	{
	Boolean hasKey;
	DescriptorTypeID typeID;
	procs->actionControlProcs->StringIDToTypeID(key3DXPosKey,&typeID);
	procs->actionDescriptorProcs->HasKey(positionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(positionDescriptor,typeID,&position->x);
		
	procs->actionControlProcs->StringIDToTypeID(key3DYPosKey,&typeID);
	procs->actionDescriptorProcs->HasKey(positionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(positionDescriptor,typeID,&position->y);
		
	procs->actionControlProcs->StringIDToTypeID(key3DZPosKey,&typeID);
	procs->actionDescriptorProcs->HasKey(positionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(positionDescriptor,typeID,&position->z);
		
	procs->actionControlProcs->StringIDToTypeID(key3DXAngleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(positionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(positionDescriptor,typeID,&position->xAngle);
		
	procs->actionControlProcs->StringIDToTypeID(key3DYAngleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(positionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(positionDescriptor,typeID,&position->yAngle);
		
	procs->actionControlProcs->StringIDToTypeID(key3DZAngleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(positionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(positionDescriptor,typeID,&position->zAngle);
		
	return 0;
	}


OSErr		PI3DGetCrossSectionFromDescriptor(PI3DDescriptorProcs *procs,PIActionDescriptor crossSectionDescriptor,U3DCrossSection *crossSection)
	{
	Boolean hasKey;
	DescriptorTypeID typeID;
	DescriptorClassID classID;
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneColorKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor colorDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(crossSectionDescriptor, typeID, &classID, &colorDesc);	
		if(colorDesc)
			{
			procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&crossSection->planeColor[0]);
				
			procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&crossSection->planeColor[1]);
				
			procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&crossSection->planeColor[2]);
				
			procs->actionDescriptorProcs->Free(colorDesc);	
			}
		}

	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionIntersectionColorKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor colorDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(crossSectionDescriptor, typeID, &classID, &colorDesc);	
		if(colorDesc)
			{
			procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&crossSection->intersectionColor[0]);
				
			procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&crossSection->intersectionColor[1]);
				
			procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&crossSection->intersectionColor[2]);
				
			procs->actionDescriptorProcs->Free(colorDesc);	
			}
		}
	
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionOffsetKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(crossSectionDescriptor,typeID,&crossSection->offset);
		
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneTilt1Key,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(crossSectionDescriptor,typeID,&crossSection->planeTilt1);
		
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneTilt2Key,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(crossSectionDescriptor,typeID,&crossSection->planeTilt2);
		
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneOpacityKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(crossSectionDescriptor,typeID,&crossSection->planeOpacity);
	
	int32 tempInt;
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionAlignmentKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetInteger(crossSectionDescriptor,typeID,&tempInt);
	crossSection->alignmentMode = tempInt;
	
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionEnabledKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetBoolean(crossSectionDescriptor,typeID,&crossSection->crossSectionEnabled);
		
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneFlipKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetBoolean(crossSectionDescriptor,typeID,&crossSection->planeFlip);
		
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneVisibleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetBoolean(crossSectionDescriptor,typeID,&crossSection->planeVisible);
		
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionIntersectionVisibleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(crossSectionDescriptor,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetBoolean(crossSectionDescriptor,typeID,&crossSection->intersectionVisible);
		
	return 0;
	}
	
OSErr		PI3DGetStateFromDescriptor(PI3DDescriptorProcs *procs, PIActionDescriptor stateDesc, uint16 *viewName, RenderState *state)
	{
	Boolean hasKey;
	DescriptorTypeID typeID;
	DescriptorClassID classID;
	ASZString tempZString;
	
	procs->actionControlProcs->StringIDToTypeID(key3DStateNameKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(viewName && hasKey)
		{
		procs->actionDescriptorProcs->GetZString( stateDesc,typeID,&tempZString);
		int32 strLen=procs->zStringProcs->LengthAsUnicodeCString(tempZString);
		procs->zStringProcs->AsUnicodeCString(tempZString,viewName,strLen+1,false);
		procs->zStringProcs->Release(tempZString);
		}

	procs->actionControlProcs->StringIDToTypeID(key3DCurrentCameraPositionKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor positionDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(stateDesc, typeID, &classID, &positionDesc);	
		if(positionDesc)
			PI3DGetPositionFromDescriptor(procs, positionDesc, &state->currentCameraPosition);	
		procs->actionDescriptorProcs->Free(positionDesc);	
		}

	procs->actionControlProcs->StringIDToTypeID(key3DCurrentFOVKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentFieldOfView);
	state->currentFieldOfView *= R2D;
					
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentPositionKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor positionDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(stateDesc, typeID, &classID, &positionDesc);	
		if(positionDesc)
			PI3DGetPositionFromDescriptor(procs, positionDesc, &state->currentObjectPosition);	
		procs->actionDescriptorProcs->Free(positionDesc);	
		}
	
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentObjectXScaleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentObjectXScale);
					
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentObjectYScaleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentObjectYScale);
		
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentObjectZScaleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentObjectZScale);
		
	procs->actionControlProcs->StringIDToTypeID(key3DAuxilaryColorKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor colorDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(stateDesc, typeID, &classID, &colorDesc);	
		if(colorDesc)
			{
			procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&state->currentAuxilaryColor[0]);
				
			procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&state->currentAuxilaryColor[1]);
				
			procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&state->currentAuxilaryColor[2]);
				
			procs->actionDescriptorProcs->Free(colorDesc);	
			}
		}
	
	procs->actionControlProcs->StringIDToTypeID(key3DAuxilaryColorKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor colorDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(stateDesc, typeID, &classID, &colorDesc);	
		if(colorDesc)
			{
			procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&state->currentAuxilaryColor[0]);
				
			procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&state->currentAuxilaryColor[1]);
				
			procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
			procs->actionDescriptorProcs->HasKey(colorDesc,typeID,&hasKey);
			if(hasKey)
				procs->actionDescriptorProcs->GetFloat(colorDesc,typeID,&state->currentAuxilaryColor[2]);
				
			procs->actionDescriptorProcs->Free(colorDesc);	
			}
		}
		
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentTimeKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentTime);
			
	
	procs->actionControlProcs->StringIDToTypeID(key3DOpacityKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentOpacity);
		
			
	procs->actionControlProcs->StringIDToTypeID(key3DLineWidthKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentLineWidth);
		
			
	procs->actionControlProcs->StringIDToTypeID(key3DCreaseValueKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentCreaseValue);
		
			
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentOrthographicScaleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetFloat(stateDesc,typeID,&state->currentOrthoScale);
				
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentOrthographicKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetBoolean(stateDesc,typeID,&state->currentOrthographic);
	
	int32 tempInt;
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentRenderModeKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetInteger(stateDesc,typeID,&tempInt);
	state->currentRenderMode = 	tempInt;
	
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentLightModeKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		procs->actionDescriptorProcs->GetInteger(stateDesc,typeID,&tempInt);
	state->currentLightMode = 	tempInt;
		
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentCrossSectionKey,&typeID);
	procs->actionDescriptorProcs->HasKey(stateDesc,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor crossSectionDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(stateDesc, typeID, &classID, &crossSectionDesc);	
		if(crossSectionDesc)
			PI3DGetCrossSectionFromDescriptor(procs, crossSectionDesc, &state->currentCrossSection);	
		procs->actionDescriptorProcs->Free(crossSectionDesc);	
		}
	
	
	return 0;
	}

OSErr		PI3DParseFrameReaderDescriptor(PI3DDescriptorProcs *procs, PIActionDescriptor theFrameReaderDescriptor,PI3DLayer *theLayer)
	{
	Boolean hasKey;
	DescriptorTypeID typeID;
	DescriptorClassID classID;
	
	//Layer origin
	theLayer->sceneOrigin.h=0;
	theLayer->sceneOrigin.v=0;
	procs->actionControlProcs->StringIDToTypeID(koffsetStr,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		PIActionDescriptor originDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(theFrameReaderDescriptor, typeID, &classID, &originDesc);
		if(originDesc)
			{
			procs->actionControlProcs->StringIDToTypeID(kwidthStr,&typeID);
			procs->actionDescriptorProcs->HasKey(originDesc,typeID,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(originDesc,typeID, &theLayer->sceneOrigin.h);
				 
			procs->actionControlProcs->StringIDToTypeID(kheightStr,&typeID);
			procs->actionDescriptorProcs->HasKey(originDesc,typeID,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(originDesc,typeID, &theLayer->sceneOrigin.v);
			procs->actionDescriptorProcs->Free(originDesc);
			}
		}
	
	//Scene size
	procs->actionControlProcs->StringIDToTypeID(kDocumentSizeKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		PIActionDescriptor sizeDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(theFrameReaderDescriptor, typeID, &classID, &sizeDesc);
		if(sizeDesc)
			{
			procs->actionControlProcs->StringIDToTypeID(kwidthStr,&typeID);
			procs->actionDescriptorProcs->HasKey(sizeDesc,typeID,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(sizeDesc,typeID, &theLayer->sceneSize.h);
				 
			procs->actionControlProcs->StringIDToTypeID(kheightStr,&typeID);
			procs->actionDescriptorProcs->HasKey(sizeDesc,typeID,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(sizeDesc,typeID, &theLayer->sceneSize.v);
			procs->actionDescriptorProcs->Free(sizeDesc);
			}
		}
	
	//Duration
	procs->actionControlProcs->StringIDToTypeID(key3DDurationKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		procs->actionDescriptorProcs->GetFloat(theFrameReaderDescriptor,typeID,&theLayer->duration);
	
		
	int32 strLen;
	ASZString tempZString;
	
	//File name
	procs->actionControlProcs->StringIDToTypeID(key3DFileNameKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		procs->actionDescriptorProcs->GetZString( theFrameReaderDescriptor,typeID,&tempZString);
		strLen=procs->zStringProcs->LengthAsUnicodeCString(tempZString);
		theLayer->scenePath = (uint16*)PI3DMemoryAlloc((strLen+2)*2);
		strLen=procs->zStringProcs->AsUnicodeCString(tempZString,theLayer->scenePath,strLen+1,false);
		strLen=procs->zStringProcs->Release(tempZString);
		}
	
	int32 index;
	uint32 listSize;
	//View index and view list
	procs->actionControlProcs->StringIDToTypeID(key3DViewIndexKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		procs->actionDescriptorProcs->GetInteger(theFrameReaderDescriptor,typeID,&theLayer->stateIndex);
	
	procs->actionControlProcs->StringIDToTypeID(key3DStateListKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		//states
		PIActionList stateActionList;
		procs->actionDescriptorProcs->GetList(theFrameReaderDescriptor,typeID,&stateActionList);
		if(stateActionList)
			{
			procs->actionListProcs->GetCount(stateActionList,&listSize);
			
			theLayer->stateList.length=listSize;
			theLayer->stateList.positionNames =  (uint16**) PI3DMemoryAlloc(theLayer->stateList.length*sizeof(uint16*));
			theLayer->stateList.viewStates = (RenderState*) PI3DMemoryAlloc(theLayer->stateList.length*sizeof(RenderState));
		
			for(index=0;index<listSize;index++)
				{
				PIActionDescriptor stateDescriptor;
				procs->actionControlProcs->StringIDToTypeID(key3DState,&typeID);
				
				theLayer->stateList.positionNames[index] =  (uint16*) PI3DMemoryAlloc(1024);
				theLayer->stateList.positionNames[index][0]=0;
				procs->actionListProcs->GetObject(stateActionList,index,&typeID,&stateDescriptor);
				if(stateDescriptor)
					PI3DGetStateFromDescriptor(procs, stateDescriptor, theLayer->stateList.positionNames[index], &theLayer->stateList.viewStates[index]);	
				
				procs->actionDescriptorProcs->Free(stateDescriptor);
				}
				
			procs->actionListProcs->Free(stateActionList);
			}
		}
	
	//Textures
	procs->actionControlProcs->StringIDToTypeID(key3DTexturesExternalKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		procs->actionDescriptorProcs->GetBoolean(theFrameReaderDescriptor,typeID,&theLayer->texturesExternal);
		
	procs->actionControlProcs->StringIDToTypeID(key3DTexturesVisibleKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		procs->actionDescriptorProcs->GetBoolean(theFrameReaderDescriptor,typeID,&theLayer->texturesEnabled);
	
	procs->actionControlProcs->StringIDToTypeID(key3DTextureListKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		//textures
		PIActionList textureActionList;
		procs->actionDescriptorProcs->GetList(theFrameReaderDescriptor,typeID,&textureActionList);
		if(textureActionList)
			{
			procs->actionListProcs->GetCount(textureActionList,&listSize);
			
			theLayer->textureList.length=listSize;
			theLayer->textureList.textureNames =  (uint16**) PI3DMemoryAlloc(theLayer->textureList.length*sizeof(uint16*));
			theLayer->textureList.texturePaths =  (uint16**) PI3DMemoryAlloc(theLayer->textureList.length*sizeof(uint16*));
			theLayer->textureList.textureType = (uint32*) PI3DMemoryAlloc(theLayer->textureList.length*sizeof(uint32));
			theLayer->textureEnabled = (Boolean*) PI3DMemoryAlloc(theLayer->textureList.length*sizeof(Boolean));
		
			for(index=0;index<listSize;index++)
				{
				PIActionDescriptor textureDescriptor;
				procs->actionControlProcs->StringIDToTypeID(key3DTextureObjectKey,&typeID);
				procs->actionListProcs->GetObject(textureActionList,index,&typeID,&textureDescriptor);
				
				//Original texture path
				procs->actionControlProcs->StringIDToTypeID(key3DTextureNameKey,&typeID);
				procs->actionDescriptorProcs->GetZString( textureDescriptor,typeID,&tempZString);
				strLen=procs->zStringProcs->LengthAsUnicodeCString(tempZString);
				theLayer->textureList.textureNames[index] = (uint16*)PI3DMemoryAlloc((strLen+2)*2);
				strLen=procs->zStringProcs->AsUnicodeCString(tempZString,theLayer->textureList.textureNames[index],strLen+1,false);
				strLen=procs->zStringProcs->Release(tempZString);

				//Temp texture path
				procs->actionControlProcs->StringIDToTypeID(key3DTexturePathKey,&typeID);
				procs->actionDescriptorProcs->GetZString( textureDescriptor,typeID,&tempZString);
				strLen=procs->zStringProcs->LengthAsUnicodeCString(tempZString);
				theLayer->textureList.texturePaths[index] = (uint16*)PI3DMemoryAlloc((strLen+2)*2);
				strLen=procs->zStringProcs->AsUnicodeCString(tempZString,theLayer->textureList.texturePaths[index],strLen+1,false);
				strLen=procs->zStringProcs->Release(tempZString);

				//Enabled
				procs->actionControlProcs->StringIDToTypeID(key3DTextureVisibleKey,&typeID);
				procs->actionDescriptorProcs->HasKey(textureDescriptor,typeID,&hasKey);
				if (hasKey)
					procs->actionDescriptorProcs->GetBoolean(textureDescriptor,typeID,&theLayer->textureEnabled[index]);
				else
					theLayer->textureEnabled[index]=true;
				
				//Type
				procs->actionControlProcs->StringIDToTypeID(key3DTextureTypeKey,&typeID);
				procs->actionDescriptorProcs->HasKey(textureDescriptor,typeID,&hasKey);
				if (hasKey)
					{
					int32 tempInt=0;
					procs->actionDescriptorProcs->GetInteger(textureDescriptor,typeID,&tempInt);
					theLayer->textureList.textureType[index] = tempInt;
					}
				else
					theLayer->textureList.textureType[index]=0;
				
				procs->actionDescriptorProcs->Free(textureDescriptor);
				}
				
			procs->actionListProcs->Free(textureActionList);
			}
		}

	/*
	if (desc->HasKey(key3DMeshTexturePathKey))
		{
		renderData.meshTexturePath = new uint16[kMax3DURLStringLength];
		tempString = desc->GetString(key3DMeshTexturePathKey);		
		tempString.AsWideCharCString(renderData.meshTexturePath,kMax3DURLStringLength, false);
		}
	*/
	
	procs->actionControlProcs->StringIDToTypeID(key3DState,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		PIActionDescriptor stateDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(theFrameReaderDescriptor, typeID, &classID, &stateDesc);	
		if(stateDesc)
			PI3DGetStateFromDescriptor(procs, stateDesc, NULL, &theLayer->currentRenderState);	
		procs->actionDescriptorProcs->Free(stateDesc);
		}

	procs->actionControlProcs->StringIDToTypeID(key3DSceneKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theFrameReaderDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		PIActionDescriptor sceneDesc;
		classID = typeID;
		procs->actionDescriptorProcs->GetObject(theFrameReaderDescriptor, typeID, &classID, &sceneDesc);	
		if(sceneDesc)	
			{
			theLayer->currentScene = PI3DCreateScene(NULL);
			
			//еее We really don't want to have to do this, but for now, we'll go with it
			PIDescriptorHandle sceneAsHandle=NULL;
			procs->actionDescriptorProcs->AsHandle(sceneDesc,&sceneAsHandle);
			if(sceneAsHandle)
				{
				PI3DParseDescriptorIntoScene(procs->actionDescriptorProcs,procs->actionListProcs,sceneAsHandle,&theLayer->currentScene);		
				procs->handleProcs->disposeProc(sceneAsHandle);
				}
			procs->actionDescriptorProcs->Free(sceneDesc);
			}
		}
				
	return 0;
	}
	
OSErr		PI3DGetLayerFromDescriptor(PI3DDescriptorProcs *procs, PIActionDescriptor theLayerDescriptor, PI3DLayer *theLayer)
	{
	
	if(!theLayer || !procs)
		return 1;
	if(!procs->actionDescriptorProcs)
		return 1;
	if(!procs->actionListProcs)
		return 1;
	if(!procs->actionControlProcs)
		return 1;
	if(!procs->zStringProcs)
		return 1;
	if(!procs->handleProcs)
		return 1;
	
	Boolean hasKey;
	DescriptorTypeID typeID;
	DescriptorClassID classID;
	int32 tempInt=0;
	
	procs->actionControlProcs->StringIDToTypeID(key3DTextureFunctionPtr,&typeID);
	procs->actionDescriptorProcs->HasKey(theLayerDescriptor,typeID,&hasKey);
	if (hasKey)
		 procs->actionDescriptorProcs->GetInteger(theLayerDescriptor,typeID, &tempInt);
	theLayer->getTextureFunction=(GetTextureFunction)tempInt;
	tempInt=0;
 
	procs->actionControlProcs->StringIDToTypeID(key3DTextureSizeFunctionPtr,&typeID);
	procs->actionDescriptorProcs->HasKey(theLayerDescriptor,typeID,&hasKey);
	if (hasKey)
		 procs->actionDescriptorProcs->GetInteger(theLayerDescriptor,typeID, &tempInt);
	theLayer->getTextureSizeFunction=(GetTextureSizeFunction)tempInt;

	//Layer origin
	VPoint origin={0, 0};
	procs->actionControlProcs->StringIDToTypeID(kOriginKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theLayerDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		PIActionDescriptor originDesc;
		procs->actionDescriptorProcs->GetObject(theLayerDescriptor, typeID, &classID, &originDesc);
		if(originDesc)
			{
			procs->actionDescriptorProcs->HasKey(originDesc,keyHorizontal,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(originDesc,keyHorizontal, &origin.h);
			procs->actionDescriptorProcs->HasKey(originDesc,keyVertical,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(originDesc,keyVertical, &origin.v);
			procs->actionDescriptorProcs->Free(originDesc);
			}
		}
	
	//Layer Scale
	FloatPoint scale = {1.0,1.0};
	procs->actionControlProcs->StringIDToTypeID(kscaleStr,&typeID);
	procs->actionDescriptorProcs->HasKey(theLayerDescriptor,typeID,&hasKey);
	if(hasKey)
		{
		PIActionDescriptor scaleDesc;
		procs->actionDescriptorProcs->GetObject(theLayerDescriptor, typeID, &classID, &scaleDesc);
		if(scaleDesc)
			{
			procs->actionControlProcs->StringIDToTypeID(kxStr,&typeID);
			procs->actionDescriptorProcs->HasKey(scaleDesc,typeID,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetFloat(scaleDesc,typeID, &scale.x);
				 
			procs->actionControlProcs->StringIDToTypeID(kyStr,&typeID);
			procs->actionDescriptorProcs->HasKey(scaleDesc,typeID,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetFloat(scaleDesc,typeID, &scale.y);
				 
			procs->actionDescriptorProcs->Free(scaleDesc);
			}

		}
	
	//Layer Size
	VPoint size = theLayer->layerSize;
	procs->actionDescriptorProcs->HasKey(theLayerDescriptor,keySizeKey,&hasKey);
	if (hasKey)
		{
		PIActionDescriptor sizeDesc;
		procs->actionDescriptorProcs->GetObject(theLayerDescriptor, keySizeKey, &classID, &sizeDesc);
		if(sizeDesc)
			{
			procs->actionDescriptorProcs->HasKey(sizeDesc,keyHorizontal,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(sizeDesc,keyHorizontal, &size.h);
			procs->actionDescriptorProcs->HasKey(sizeDesc,keyVertical,&hasKey);
			if (hasKey)
				 procs->actionDescriptorProcs->GetInteger(sizeDesc,keyVertical, &size.v);
			
			procs->actionDescriptorProcs->Free(sizeDesc);
			}
		}	
	
	theLayer->layerOrigin = origin;
	theLayer->layerScale = scale;
	theLayer->layerSize = size;
	
	//The 3D-specific layer data
	procs->actionControlProcs->StringIDToTypeID(kFrameReaderKey,&typeID);
	procs->actionDescriptorProcs->HasKey(theLayerDescriptor,typeID,&hasKey);
	if (hasKey)
		{
		PIActionDescriptor frameReaderDesc;
		procs->actionDescriptorProcs->GetObject(theLayerDescriptor, typeID, &classID, &frameReaderDesc);
		if(frameReaderDesc)
			{
			//Parse the frameReaderDesc
			PI3DParseFrameReaderDescriptor(procs, frameReaderDesc, theLayer);
			procs->actionDescriptorProcs->Free(frameReaderDesc);
			}
		}
	return 0;
	}
	
OSErr		PI3DInitialize3DLayer(PI3DLayer *layerData,VPoint *docSize)
	{
	//Set everythign to zero
	memset(layerData,0,sizeof(PI3DLayer));
	
	//Set up everything that is not zero
	layerData->layerSize.h=docSize->h;
	layerData->layerSize.v=docSize->v;
	layerData->layerScale.x=1.0;
	layerData->layerScale.y=1.0;
	layerData->duration=10.0f;
	layerData->sceneSize.h=docSize->h;
	layerData->sceneSize.v=docSize->v;

	layerData->stateIndex=1;
	layerData->texturesEnabled=true;
	
	layerData->currentRenderState.currentFieldOfView=42.0;//42 degrees is vertical fov for a 18mm lens on a Rebel XT
	layerData->currentRenderState.currentObjectXScale=1.0;
	layerData->currentRenderState.currentObjectYScale=1.0;
	layerData->currentRenderState.currentObjectZScale=1.0;
	layerData->currentRenderState.currentAuxilaryColor[0]=1.0;
	layerData->currentRenderState.currentAuxilaryColor[1]=1.0;
	layerData->currentRenderState.currentAuxilaryColor[2]=1.0;
	layerData->currentRenderState.currentOpacity=1.0;
	layerData->currentRenderState.currentLineWidth=1.0;
	layerData->currentRenderState.currentCreaseValue=80;
	
	return 0;
	}
	
OSErr		PI3DKill3DLayer(PI3DLayer *layerData)
	{
	uint16 i;
	if(layerData->currentScene)
		PI3DKillScene(layerData->currentScene);
	layerData->currentScene=NULL;
	
	if(layerData->scenePath)
		PI3DMemoryFree(layerData->scenePath);
		
	//Cameras
	for(i=0;i<layerData->stateList.length; i++)
		if(layerData->stateList.positionNames[i])
			PI3DMemoryFree(layerData->stateList.positionNames[i]);	
	if(layerData->stateList.positionNames)
		PI3DMemoryFree(layerData->stateList.positionNames);
	layerData->stateList.positionNames=NULL;

	if(layerData->stateList.viewStates)
		PI3DMemoryFree(layerData->stateList.viewStates);	
	layerData->stateList.viewStates=NULL;
		
	//Texture Paths
	for(uint16 i=0;i<layerData->textureList.length; i++)
		{
		if(layerData->textureList.textureNames)
			{
			if(layerData->textureList.textureNames[i])
				PI3DMemoryFree(layerData->textureList.textureNames[i]);
			}
		
		if(layerData->textureList.texturePaths)
			{
			if(layerData->textureList.texturePaths[i])
				PI3DMemoryFree(layerData->textureList.texturePaths[i]);
			}	
		
		}
		
	if(layerData->textureList.textureNames)
		PI3DMemoryFree(layerData->textureList.textureNames);
	layerData->textureList.textureNames = NULL;
	
	if(layerData->textureList.textureType)
		PI3DMemoryFree(layerData->textureList.textureType);
	layerData->textureList.textureType = NULL;
	
	if(layerData->textureList.texturePaths)
		PI3DMemoryFree(layerData->textureList.texturePaths);
	layerData->textureList.texturePaths=NULL;

	if(layerData->textureEnabled)
		PI3DMemoryFree(layerData->textureEnabled);
	layerData->textureEnabled=NULL;
	return 0;
	}

OSErr		PI3DMakeLayerDescriptor(PI3DDescriptorProcs *procs, PI3DLayer *theLayer, PIActionDescriptor theLayerDescriptor)
	{
	// All pixel sources need to use kPixelSourceClass and add kPixelSourceTypeKey.
	// Could move this responsibility to the base class for real enforcement
	if(!procs->actionDescriptorProcs)
		return 1;
	if(!procs->actionListProcs)
		return 1;
	if(!procs->actionControlProcs)
		return 1;
	if(!procs->zStringProcs)
		return 1;

	DescriptorTypeID typeID;	
	DescriptorTypeID typeID2;	
	procs->actionControlProcs->StringIDToTypeID(kPixelSourceTypeKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(theLayerDescriptor, typeID, kPixelSourceType3D);
	
	procs->actionControlProcs->StringIDToTypeID(kDescVersionKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(theLayerDescriptor, typeID, k3DPixelSourceDescriptorVersion);
	
	//Size
	PIActionDescriptor sizeDesc=NULL;
	procs->actionDescriptorProcs->Make(&sizeDesc);
	if(!sizeDesc)
		return 1;
	
	procs->actionDescriptorProcs->PutInteger(sizeDesc,keyHorizontal, theLayer->layerSize.h);
	procs->actionDescriptorProcs->PutInteger(sizeDesc,keyVertical, theLayer->layerSize.v);
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,keySizeKey,keySizeKey, sizeDesc);
	procs->actionDescriptorProcs->Free(sizeDesc);
	
	//Origin
	PIActionDescriptor originDesc=NULL;
	procs->actionDescriptorProcs->Make(&originDesc);
	if(!originDesc)
		return 1;
	
	procs->actionDescriptorProcs->PutInteger(originDesc,keyHorizontal, theLayer->layerOrigin.h);
	procs->actionDescriptorProcs->PutInteger(originDesc,keyVertical, theLayer->layerOrigin.v);
	procs->actionControlProcs->StringIDToTypeID(kOriginKey,&typeID);
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,typeID,typeID, originDesc);
	procs->actionDescriptorProcs->Free(originDesc);
	
	//Scale
	PIActionDescriptor scaleDesc=NULL;
	procs->actionDescriptorProcs->Make(&scaleDesc);
	if(!scaleDesc)
		return 1;
	
	procs->actionControlProcs->StringIDToTypeID(kxStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(scaleDesc,typeID, theLayer->layerScale.x);
	procs->actionControlProcs->StringIDToTypeID(kyStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(scaleDesc,typeID, theLayer->layerScale.y);
	procs->actionControlProcs->StringIDToTypeID(kscaleStr,&typeID);
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,typeID,typeID, scaleDesc);
	procs->actionDescriptorProcs->Free(scaleDesc);

	
	//PActionDescriptor &readerDesc = fFrameReader->MakeDescriptor(docLocation);
	PIActionDescriptor theFrameReaderDescriptor=NULL;
	procs->actionDescriptorProcs->Make(&theFrameReaderDescriptor);
	if(!theFrameReaderDescriptor)
		return 1;
	procs->actionControlProcs->StringIDToTypeID(kFrameReaderKey,&typeID);
	procs->actionControlProcs->StringIDToTypeID(kFrameReaderClass,&typeID2);
	PI3DMakeFrameReaderDescriptor(procs,theLayer, theFrameReaderDescriptor);
	
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,typeID,typeID2, theFrameReaderDescriptor);
	procs->actionDescriptorProcs->Free(theFrameReaderDescriptor);
	
	//Hmmmm.....do I need this?
	//PActionDescriptor desc = TActionDescriptor::Make(kPixelSourceClass);
	return 0;
	}


OSErr		PI3DMakeCrossSectionDescriptor(PI3DDescriptorProcs *procs, U3DCrossSection *crossSection,PIActionDescriptor crossSectionDescriptor)
	{
	PIActionDescriptor planeColorDesc=NULL;
	procs->actionDescriptorProcs->Make(&planeColorDesc);
	if(!planeColorDesc)
		return 1;
		
	DescriptorTypeID typeID;	
	procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(planeColorDesc,typeID,crossSection->planeColor[0]);
	
	procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(planeColorDesc,typeID,crossSection->planeColor[1]);
	
	procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(planeColorDesc,typeID,crossSection->planeColor[2]);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneColorKey,&typeID);
	procs->actionDescriptorProcs->PutObject(crossSectionDescriptor,typeID,typeID,planeColorDesc);
	procs->actionDescriptorProcs->Free(planeColorDesc);
	
	PIActionDescriptor intersectionColorDesc=NULL;
	procs->actionDescriptorProcs->Make(&intersectionColorDesc);
	if(!planeColorDesc)
		return FALSE;

	procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(intersectionColorDesc,typeID,crossSection->intersectionColor[0]);
	
	procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(intersectionColorDesc,typeID,crossSection->intersectionColor[1]);
	
	procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(intersectionColorDesc,typeID,crossSection->intersectionColor[2]);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionIntersectionColorKey,&typeID);
	procs->actionDescriptorProcs->PutObject(crossSectionDescriptor,typeID,typeID,intersectionColorDesc);
	procs->actionDescriptorProcs->Free(intersectionColorDesc);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionOffsetKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(crossSectionDescriptor,typeID,crossSection->offset);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneTilt1Key,&typeID);
	procs->actionDescriptorProcs->PutFloat(crossSectionDescriptor,typeID,crossSection->planeTilt1);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneTilt2Key,&typeID);
	procs->actionDescriptorProcs->PutFloat(crossSectionDescriptor,typeID,crossSection->planeTilt2);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneOpacityKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(crossSectionDescriptor,typeID,crossSection->planeOpacity);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionAlignmentKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(crossSectionDescriptor,typeID,crossSection->alignmentMode);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionEnabledKey,&typeID);
	procs->actionDescriptorProcs->PutBoolean(crossSectionDescriptor,typeID,
					 crossSection->crossSectionEnabled != false);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneFlipKey,&typeID);
	procs->actionDescriptorProcs->PutBoolean(crossSectionDescriptor,typeID,
					 crossSection->planeFlip != false);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionPlaneVisibleKey,&typeID);
	procs->actionDescriptorProcs->PutBoolean(crossSectionDescriptor,typeID,
					 crossSection->planeVisible != false);

	procs->actionControlProcs->StringIDToTypeID(key3DCrossSectionIntersectionVisibleKey,&typeID);
	procs->actionDescriptorProcs->PutBoolean(crossSectionDescriptor,typeID,
					 crossSection->intersectionVisible != false);
	return 0;
	}
	

OSErr		PI3DMakePositionDescriptor(PI3DDescriptorProcs *procs,U3DPosition *camera,PIActionDescriptor positionDescriptor)
	{
	
	DescriptorTypeID typeID;	
	procs->actionControlProcs->StringIDToTypeID(key3DXPosKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(positionDescriptor,typeID,camera->x);
	procs->actionControlProcs->StringIDToTypeID(key3DYPosKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(positionDescriptor,typeID,camera->y);
	procs->actionControlProcs->StringIDToTypeID(key3DZPosKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(positionDescriptor,typeID,camera->z);
	
	procs->actionControlProcs->StringIDToTypeID(key3DXAngleKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(positionDescriptor,typeID,camera->xAngle);
	procs->actionControlProcs->StringIDToTypeID(key3DYAngleKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(positionDescriptor,typeID,camera->yAngle);
	procs->actionControlProcs->StringIDToTypeID(key3DZAngleKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(positionDescriptor,typeID,camera->zAngle);
	

	return 0;
	}
	
OSErr		PI3DMakeStateDescriptor(PI3DDescriptorProcs *procs,RenderState *state, uint16 *name, PIActionDescriptor stateDescriptor)
	{
	//State
	ASZString tempString;
	DescriptorTypeID typeID;
	
	//Name - if there is one
	if(name)
		{
		procs->zStringProcs->MakeFromUnicode(name, PI3DStringLength16(name),&tempString);
		procs->actionControlProcs->StringIDToTypeID(key3DStateNameKey,&typeID);
		procs->actionDescriptorProcs->PutZString(stateDescriptor,typeID,tempString);
		procs->zStringProcs->Release(tempString);
		}
	
	//Camera position
	PIActionDescriptor currentCameraObjectDesc=NULL;
	procs->actionDescriptorProcs->Make(&currentCameraObjectDesc);
	if(!currentCameraObjectDesc)
		return 1;
	
	PI3DMakePositionDescriptor(procs,&state->currentCameraPosition,currentCameraObjectDesc);
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentCameraPositionKey,&typeID);
	procs->actionDescriptorProcs->PutObject(stateDescriptor,typeID,typeID, currentCameraObjectDesc);
	procs->actionDescriptorProcs->Free(currentCameraObjectDesc);
	
	//FOV
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentFOVKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentFieldOfView * D2R);
	
	//Obejct position
	PIActionDescriptor currentPositionDesc=NULL;
	procs->actionDescriptorProcs->Make(&currentPositionDesc);
	if(!currentPositionDesc)
		return 1;
	
	PI3DMakePositionDescriptor(procs,&state->currentObjectPosition,currentPositionDesc);
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentPositionKey,&typeID);
	procs->actionDescriptorProcs->PutObject(stateDescriptor,typeID,typeID, currentPositionDesc);
	procs->actionDescriptorProcs->Free(currentPositionDesc);
	
	//Scale
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentObjectXScaleKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentObjectXScale);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentObjectYScaleKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentObjectYScale);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentObjectZScaleKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentObjectZScale);
	
	//Orthographic
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentOrthographicKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(stateDescriptor,typeID, state->currentOrthographic);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentOrthographicScaleKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID, state->currentOrthoScale);
	
	//Line color
	PIActionDescriptor auxColorDesc=NULL;
	procs->actionDescriptorProcs->Make(&auxColorDesc);
	if(!auxColorDesc)
		return FALSE;
		
	procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(auxColorDesc,typeID,state->currentAuxilaryColor[0]);
	
	procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(auxColorDesc,typeID,state->currentAuxilaryColor[1]);
	
	procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(auxColorDesc,typeID,state->currentAuxilaryColor[2]);
	
	procs->actionControlProcs->StringIDToTypeID(key3DAuxilaryColorKey,&typeID);
	procs->actionDescriptorProcs->PutObject(stateDescriptor,typeID,typeID,auxColorDesc);
	procs->actionDescriptorProcs->Free(auxColorDesc);
	
	
	//Face Color
	PIActionDescriptor faceColorDesc=NULL;
	procs->actionDescriptorProcs->Make(&faceColorDesc);
	if(!faceColorDesc)
		return FALSE;
		
	procs->actionControlProcs->StringIDToTypeID(kredStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(faceColorDesc,typeID,state->currentFaceColor[0]);
	
	procs->actionControlProcs->StringIDToTypeID(kgreenStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(faceColorDesc,typeID,state->currentFaceColor[1]);
	
	procs->actionControlProcs->StringIDToTypeID(kblueStr,&typeID);
	procs->actionDescriptorProcs->PutFloat(faceColorDesc,typeID,state->currentFaceColor[2]);
	
	procs->actionControlProcs->StringIDToTypeID(key3DFaceColorKey,&typeID);
	procs->actionDescriptorProcs->PutObject(stateDescriptor,typeID,typeID,faceColorDesc);
	procs->actionDescriptorProcs->Free(faceColorDesc);
	
	//Misc appearancew stuff
	procs->actionControlProcs->StringIDToTypeID(key3DOpacityKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentOpacity);
	
	procs->actionControlProcs->StringIDToTypeID(key3DLineWidthKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentLineWidth);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCreaseValueKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentCreaseValue);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentRenderModeKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(stateDescriptor,typeID,state->currentRenderMode);
	
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentLightModeKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(stateDescriptor,typeID,state->currentLightMode);

	procs->actionControlProcs->StringIDToTypeID(key3DCurrentTimeKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(stateDescriptor,typeID,state->currentTime);
	
	
	//Cross Section
	PIActionDescriptor currentCrossSectionObjectDesc=NULL;
	procs->actionDescriptorProcs->Make(&currentCrossSectionObjectDesc);
	if(!currentCrossSectionObjectDesc)
		return 1;
	
	PI3DMakeCrossSectionDescriptor(procs,&state->currentCrossSection,currentCrossSectionObjectDesc);
	procs->actionControlProcs->StringIDToTypeID(key3DCurrentCrossSectionKey,&typeID);
	procs->actionDescriptorProcs->PutObject(stateDescriptor,typeID,typeID, currentCrossSectionObjectDesc);
	procs->actionDescriptorProcs->Free(currentCrossSectionObjectDesc);
		
	return 0;
	}
	
OSErr		PI3DMakeFrameReaderDescriptor(PI3DDescriptorProcs *procs,PI3DLayer *theLayer, PIActionDescriptor theLayerDescriptor)
	{	
	
	ASZString tempString;
	DescriptorTypeID typeID;
	
		
	//PActionDescriptor desc = TActionDescriptor::Make(kFrameReaderClass);
	
	procs->actionControlProcs->StringIDToTypeID(kFrameReaderTypeKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(theLayerDescriptor,typeID, kFrameReaderTypeAcrobat3D);
	
	procs->actionControlProcs->StringIDToTypeID(kDescVersionKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(theLayerDescriptor,typeID, kA3FRDescriptorVersion);
	
	//Size
	PIActionDescriptor sizeDesc=NULL;
	procs->actionDescriptorProcs->Make(&sizeDesc);
	if(!sizeDesc)
		return 1;
	
	procs->actionControlProcs->StringIDToTypeID(kwidthStr,&typeID);
	procs->actionDescriptorProcs->PutInteger(sizeDesc,typeID, theLayer->sceneSize.h);
	procs->actionControlProcs->StringIDToTypeID(kheightStr,&typeID);
	procs->actionDescriptorProcs->PutInteger(sizeDesc,typeID, theLayer->sceneSize.v);
	procs->actionControlProcs->StringIDToTypeID(kDocumentSizeKey,&typeID);
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,typeID,typeID, sizeDesc);
	procs->actionDescriptorProcs->Free(sizeDesc);
	
	//Origin
	PIActionDescriptor originDesc=NULL;
	procs->actionDescriptorProcs->Make(&originDesc);
	if(!originDesc)
		return 1;
	
	procs->actionControlProcs->StringIDToTypeID(kwidthStr,&typeID);
	procs->actionDescriptorProcs->PutInteger(originDesc,typeID, theLayer->sceneOrigin.h);
	procs->actionControlProcs->StringIDToTypeID(kheightStr,&typeID);
	procs->actionDescriptorProcs->PutInteger(originDesc,typeID, theLayer->sceneOrigin.v);
	procs->actionControlProcs->StringIDToTypeID(koffsetStr,&typeID);
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,typeID,typeID, originDesc);
	procs->actionDescriptorProcs->Free(originDesc);
		
	//Scene data
	PIDescriptorHandle sceneDescriptorHandle;
	PIActionDescriptor sceneDescriptor=NULL;
	PI3DParseSceneIntoDescriptor(procs->actionDescriptorProcs,procs->actionListProcs,theLayer->currentScene,&sceneDescriptorHandle);	
	procs->actionDescriptorProcs->HandleToDescriptor(sceneDescriptorHandle,&sceneDescriptor);	
	procs->actionControlProcs->StringIDToTypeID(key3DSceneKey,&typeID);
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,typeID,typeID, sceneDescriptor);
	procs->actionDescriptorProcs->Free(sceneDescriptor);
	
	//Currently selected state
	procs->actionControlProcs->StringIDToTypeID(key3DViewIndexKey,&typeID);
	procs->actionDescriptorProcs->PutInteger(theLayerDescriptor,typeID,theLayer->stateIndex);
		
	//File path
	if(theLayer->scenePath)
		{
		procs->zStringProcs->MakeFromUnicode(theLayer->scenePath, PI3DStringLength16(theLayer->scenePath),&tempString);
		procs->actionControlProcs->StringIDToTypeID(key3DFileNameKey,&typeID);
		procs->actionDescriptorProcs->PutZString(theLayerDescriptor,typeID,tempString);
		procs->zStringProcs->Release(tempString);
		}
	
	//Duration
	procs->actionControlProcs->StringIDToTypeID(key3DDurationKey,&typeID);
	procs->actionDescriptorProcs->PutFloat(theLayerDescriptor,typeID,theLayer->duration);
	
	//State List
	PIActionList stateList;
	procs->actionListProcs->Make(&stateList);
	
	for (uint32 positionIndex = 0; positionIndex <theLayer->stateList.length; ++positionIndex)
		{
		PIActionDescriptor stateDesc=NULL;
		procs->actionDescriptorProcs->Make(&stateDesc);
		if(!stateDesc)
			return 1;
		
		PI3DMakeStateDescriptor(procs, &theLayer->stateList.viewStates[positionIndex],theLayer->stateList.positionNames[positionIndex],stateDesc);
		
		procs->actionControlProcs->StringIDToTypeID(key3DState,&typeID);
		procs->actionListProcs->PutObject(stateList,typeID,stateDesc);
		procs->actionDescriptorProcs->Free(stateDesc);
		}
	procs->actionControlProcs->StringIDToTypeID(key3DStateListKey,&typeID);
	procs->actionDescriptorProcs->PutList(theLayerDescriptor, typeID,stateList);
	
	//Texture Data
	procs->actionControlProcs->StringIDToTypeID(key3DTexturesExternalKey,&typeID);
	procs->actionDescriptorProcs->PutBoolean(theLayerDescriptor,typeID, theLayer->texturesExternal != false);
	
	procs->actionControlProcs->StringIDToTypeID(key3DTexturesVisibleKey,&typeID);
	procs->actionDescriptorProcs->PutBoolean(theLayerDescriptor,typeID, theLayer->texturesEnabled != false);
	
	PIActionList textureList;
	procs->actionListProcs->Make(&textureList);
	
	for (int32 textureIndex = 0; textureIndex < theLayer->textureList.length; ++textureIndex)
		{
		PIActionDescriptor textureObjectDesc=NULL;
		procs->actionDescriptorProcs->Make(&textureObjectDesc);
		if(!textureObjectDesc)
			return 1;
		
		//Original texture path
		procs->zStringProcs->MakeFromUnicode(theLayer->textureList.textureNames[textureIndex], PI3DStringLength16(theLayer->textureList.textureNames[textureIndex]),&tempString);
		procs->actionControlProcs->StringIDToTypeID(key3DTextureNameKey,&typeID);
		procs->actionDescriptorProcs->PutZString(textureObjectDesc,typeID,tempString);
		procs->zStringProcs->Release(tempString);
		
		//Temp File Name
		procs->zStringProcs->MakeFromUnicode(theLayer->textureList.texturePaths[textureIndex], PI3DStringLength16(theLayer->textureList.texturePaths[textureIndex]),&tempString);
		procs->actionControlProcs->StringIDToTypeID(key3DTexturePathKey,&typeID);
		procs->actionDescriptorProcs->PutZString(textureObjectDesc,typeID,tempString);
		procs->zStringProcs->Release(tempString);
			
		//Visible
		if(theLayer->textureEnabled)
			{
			procs->actionControlProcs->StringIDToTypeID(key3DTextureVisibleKey,&typeID);
			procs->actionDescriptorProcs->PutBoolean(textureObjectDesc,typeID, theLayer->textureEnabled[textureIndex] != false);
			}
			
		//type
		procs->actionControlProcs->StringIDToTypeID(key3DTextureTypeKey,&typeID);
		procs->actionDescriptorProcs->PutInteger(textureObjectDesc,typeID, theLayer->textureList.textureType[textureIndex]);
		
									  
		procs->actionControlProcs->StringIDToTypeID(key3DTextureObjectKey,&typeID);
		procs->actionListProcs->PutObject(textureList,typeID,textureObjectDesc);
		procs->actionDescriptorProcs->Free(textureObjectDesc);
		}
		
	procs->actionControlProcs->StringIDToTypeID(key3DTextureListKey,&typeID);
	procs->actionDescriptorProcs->PutList(theLayerDescriptor,typeID,textureList);
	
	//Leaving out for now
	//if(fMeshIsFromTexture)
	//	desc->PutString( key3DMeshTexturePathKey, fMeshTexturePath);
	
	//State
	PIActionDescriptor stateDescriptor=NULL;
	procs->actionDescriptorProcs->Make(&stateDescriptor);
	if(!stateDescriptor)
		return 1;
		
	PI3DMakeStateDescriptor(procs, &theLayer->currentRenderState,NULL,stateDescriptor);
	
	procs->actionControlProcs->StringIDToTypeID(key3DState,&typeID);
	procs->actionDescriptorProcs->PutObject(theLayerDescriptor,typeID,typeID,stateDescriptor);
	procs->actionDescriptorProcs->Free(stateDescriptor);
	
	return 0;
	}
