/*
    File: PSScene.h

    Copyright 2006 by Adobe Systems, Inc.
	
	This file defines the basic data structure of a 3D scene in Photoshop
	    
 */

#pragma once 

#include "PITypes.h"
#include "PIActions.h"

#define PI3D			3.1415926535897932384626433832795
#define D2R				( PI3D / 180 )
#define R2D				( 180 / PI3D )

typedef nativeFloat		PI3DVector[3];
typedef nativeFloat		PI3DPoint[2];
typedef nativeFloat		PI3DMatrix[4][4];

#define kNumSupportedTextureTypes		7
#define PI3DTextureMap					0
#define PI3DReflectionMap				1
#define PI3DBumpMap						2
#define PI3DSpecularMap					3
#define PI3DOpacityMap					4
#define PI3DShininessMap				5
#define PI3DSelfIlluminationMap			6

typedef enum {
	PI3DLinearAttenuation=0,
	PI3DABCAttenuation
} PI3DAttenuationEnum;

typedef struct
{
	nativeFloat a;
	nativeFloat b;
	nativeFloat c;
} PI3DABCAtt;

typedef struct
{
	nativeFloat w;
	nativeFloat x;
	nativeFloat y;
	nativeFloat z;
} PI3DQuat;

typedef struct 
{
	PI3DVector axis; // origin, direction and angle
	nativeFloat angle;
} PI3DAngleAxis;

typedef enum {
	PI3DWireFrame=0x0001,
	PI3DFlat,
	PI3DGouraud,
	PI3DPhong,
	PI3DMetal
} PI3DShadingEnum; // shading type, NOT USED EXCEPT FOR WIREFRAME RIGHT NOW

typedef enum {
	PI3DSpotLight=0x0001,
	PI3DPointLight,
	PI3DInfiniteLight, 
	PI3DDirectionalLight // will be added to the global ambient, not used as seperate source
} PI3DLightEnum;

typedef enum {
	PI3DClampX = 0x0001,
	PI3DClampY = 0x0002,
	PI3DStyle_3DS = 0x0004, //3D Studio style map
	PI3DModulate = 0x0008,
	PI3DColorMap = 0x0010,
	PI3DBumpNormal= 0x0010,
	PI3DInvert = 0x0020
} PI3DTextureMapFlags;

typedef enum {
	PI3DAmbientNode,	
	PI3DObjectNode,
	PI3DCameraNode,
	PI3DTargetNode,
	PI3DLightNode,
	PI3DLtargetNode,
	PI3DSpotlightNode,
	PI3DGroupNode //used for PI3D and other formats that have parent nodes which are not meshes or some other object
} PI3DNodeTypes;

typedef struct {
    char				name[80];
    void				*next;
} PI3DList;

typedef struct {
    nativeFloat			red;
	nativeFloat			green;
	nativeFloat			blue;
} PI3DColor;

typedef struct {
	int64				start;
	int64				end;
	int64				length;
	uint16				tag;
} PI3DChunk;

typedef struct {
	uint8				red;
	uint8				green;
	uint8				blue;
} PI3DColor24;

/* Face flags */
//Edge 1														0x001
//Edge 2														0x002
//Edge 3														0x004
//Edges															0x007
//Double sided													0x010
//Selected triangle												0x20
//Triangle visible												0x40
//Triangle visible as edge										0x80
//Next face is hole, and additional space for points allocated  0x100
//This face is hole, in previous face							0x200
//This is triangle strip										0x400
//Subdivision patch.											0x800

typedef struct {
	int32				numPoints;		/* number of points in the face */
    int32				*points; 
	int32				flags;			/* Face flags - see above */
	int32				smoothing;		/* smoothing value */
	int32				material;		/* material index */
	int32				*normals;		/* normals if you have them */
	int32				*colors;		/* colors if you have them */
	int32				*textures; 

	void				*padding;
} PI3DFace;

typedef struct {
    //Standard list fields
	char				name[80];
	void				*next;
	
	PI3DLightEnum		type;				/* light type */
    PI3DVector			pos;				/* light position */
    PI3DVector			target;				/* light target location */
    PI3DColor			col;				/* light colour */
    nativeFloat			hotspot;			/* Hotspot angle (degrees) */
    nativeFloat			falloff;			/* Falloff angle (degrees) */
    int32				shadowFlag;			/* Shadow flag (not used) */
	Boolean				attenuation;		/* Use attenuation */
	PI3DAttenuationEnum	attenuationType;	/* linear or ABC */
	PI3DABCAtt			attenuationAbc;		/* use if you have ABC attenuation */
	nativeFloat			outerRadius;		/* outer range */
	nativeFloat			innerRadius;		/* inner range */
	nativeFloat			multiple;			/* multiplier */

	char				padding[512];
} PI3DLight;


/* Camera */
typedef struct {
    //Standard list fields
	char				name[80];
	void				*next;
	
    PI3DVector			pos;				/* Camera location */
    PI3DVector			target;				/* Camera target */
    nativeFloat			bank;				/* Banking angle (degrees) */
    nativeFloat			lens;				/* Camera lens size (mm) */
	Boolean				ortho;				/* use orthographic */
	nativeFloat			aspectratio;		/* Aspect Ratio */
	nativeFloat			zoomfactor;			/* zoom factor */
	
	char				padding[512];
} PI3DCamera;


typedef struct {
	char					map[2048];
    nativeFloat				strength;		/* intensity of the map */
	nativeFloat				uscale;  
	nativeFloat				vscale;
	nativeFloat				uoffset;
	nativeFloat				voffset;
	nativeFloat				angle;
	PI3DTextureMapFlags		flags;
	Boolean					mapIs3DSStyle;	/* Needed only by the 3ds importer */
	
	char					padding[512];
} PI3DTextureMatProp;

typedef struct {
    //Standard list fields
	char				name[80];
	void				*next;
	
    PI3DColor			ambient;
    PI3DColor			diffuse;
    PI3DColor			specular;
	PI3DColor			emissive;
    nativeFloat			shininess;
	nativeFloat			glossiness;			/* specular level */
    nativeFloat			transparency;
    nativeFloat			reflection;
    int32				selfIllumination;	/* unused */
	PI3DShadingEnum		shading;
	PI3DTextureMatProp	maps[kNumSupportedTextureTypes];
	Boolean				twoSided;
	Boolean				wireframe;
	Boolean				decal;
	nativeFloat			wireframesize;
	uint32				importdata;			/* reserved, do not use */
	
	char				padding[512];
} PI3DMaterial;


typedef struct {
    //Standard list fields
	char				name[80];
	void				*next;
	
    int32				vertices;					/* Number of vertices */
    PI3DVector			*vertex;					/* List of object vertices */
    int32				textures;					/* Number of textures */
    PI3DPoint			*texture;					/* List of object textures */
    int32				faces;						/* Number of faces */
    PI3DFace			*face;						/* List of object faces */
	int32				lines;						/* number of lines */
	PI3DVector			*linecolors;				/* per line color - rgb*/
	PI3DVector			*line;						/* line pts */
	int32				*linematerial;				/* line material index */
	int32				flags;						/* NOT USED */
    int32				hidden;						/* Hidden flag */
    int32				shadow;						/* Shadow flag */
	PI3DMatrix			matrix;						/* Mesh transformation matrix */
	Boolean				matrixPresent;				/* is there a transformation matrix */
	Boolean				smoothingGroupPresent;		/* is there a smoothing group */
    int32				normals;					/* Number of normals */
    PI3DVector			*normal;					/* List of object normals */
	int32				colors;						/* Number of colors */
	PI3DVector			*color;						/* List of vertex colors */

	char				padding[504];
} PI3DMesh;

/* Frame vector flags */
//Start cyclic							0x010
//End cyclic							0x10
//Linear								0x00000
//Bezier								0x10000 NOT USED RIGHT NOW
//TCB(Use the tension continuity...)	0x20000 

typedef struct {
	nativeFloat			framenumber;
	int32				flags;			/* Frame vector flags - see above*/
	nativeFloat			tension;
	nativeFloat			continuity;
	nativeFloat			bias;
	nativeFloat			easeto;
	nativeFloat			easefrom;
	nativeFloat			rotation;		//radians
	nativeFloat			value;			//FOV, HOT, FALL, ROLL - in degrees
	PI3DVector			v;				//POS, SCL, ROT(angleaxis value)
	Boolean				usequat;		//if true using q if not using v
	PI3DQuat			q;				//ROT(quaternion)
	
	char				padding[512];
} PI3DFrameVector;

typedef struct {
	//Standard list fields
	char				name[80];				/* name of animation segment */
	void				*next;

	int32				flags;
	int32				keys;					/* number of keyframes */
	PI3DFrameVector*	aFrameData;
	
	char				padding[512];
} PI3DAnimationData;

/* A key frame object */
typedef struct {
    //Standard list fields
	char				name[80];
	void				*next;

	char				instancename[80];		/* instance name for this object */
	int32				nodeID;					/* node id */
	int32				parentNodeID;			/* parent node id - 65535 means scene is parent */
	int32				flags1;
	int32				flags2;

	PI3DVector			pivot;
	Boolean				usepivot;				/* if not using pivot, local matrix will be set for keyframes that do not have animation data */
	Boolean				matrixPresent;
	PI3DMatrix			localmatrix;

	Boolean				userange;
	PI3DNodeTypes		nodeType;
	
	PI3DVector			bboxmin;				/* bounding box */
	PI3DVector			bboxmax;

    PI3DAnimationData	*positionData;
    PI3DAnimationData	*rotationData;
    PI3DAnimationData	*scaleData;
	PI3DAnimationData	*colData;
	PI3DAnimationData	*fOVData;
	PI3DAnimationData	*hotData;				/* not in use */
	PI3DAnimationData	*fallData;				/* not in use */
	PI3DAnimationData	*rollData;
	PI3DAnimationData	*hideData;				/* not in use */
	PI3DAnimationData	*morphData;				/* not in use */

	char				padding[512];
} PI3DKeyFrame;


typedef struct {
	PI3DLight			*lightList;
	PI3DCamera			*cameraList;
	PI3DMesh			*meshList;
	PI3DMaterial		*matPropList;
	PI3DKeyFrame		*keyFrameList;
	PI3DColor			globalAmbientColor;
	nativeFloat			globalUnits;					/* measurement units in inches */
	char				globalRange[80];				/* name of animation segment to activate */
	int32				globalAnimationStartTime;
	int32				globalNumFrames;
	nativeFloat			globalFPS;						/* frames per second */

	char				padding[512];
} PI3DScene;

typedef struct {
	PI3DScene			*scene;
	void				*userData;						/* we use this for the file stream */
	
	char				padding[512];
} PI3DImport;

#ifdef __cplusplus
extern "C" {
#endif

// Parse your 3d format - this function must be implemented or stubbed out by the user
int					PI3DParseFile (PI3DImport *importer); 

// Utility
void				PI3DCleanupName (char *);	// cleanup malformed name
void*				PI3DMemoryAlloc(size_t size); // allocation routine
void				PI3DMemoryFree(void* p); //deallocation routine

// List helpers
void				PI3DListAdd (PI3DList **root, PI3DList *new_node);
void*				PI3DListSearch (PI3DList **root, const char *name);
void				PI3DListDelete (PI3DList **root);
void*				PI3DListFindAll(PI3DScene* scene, const char *name);

// Initialization helpers
void				PI3DSceneInitGlobals(PI3DScene *scene); // Initialize the globals - do for each format
PI3DMaterial		*PI3DCreateMaterial (void); //create new material
PI3DKeyFrame		*PI3DCreateKeyframe (void);	//create new keyframe
PI3DCamera			*PI3DCreateCamera (void);	//create new camera
PI3DLight			*PI3DCreateLight (void);	//create new light
PI3DMesh			*PI3DCreateMesh (char *name, int vertices, int faces, int textures, int normals, int lines, int colors); //create new mesh
PI3DAnimationData	*PI3DCreateAnimation (int32 flags, int32 keys); //create new animation

// Mesh helpers
Boolean				PI3DAddVertexToMesh(PI3DMesh *mesh, PI3DVector v, int32 &maxVertices);
Boolean				PI3DAddNormalToMesh(PI3DMesh *mesh, PI3DVector n, int32 &maxNormals);
Boolean				PI3DAddUVToMesh(PI3DMesh *mesh, PI3DPoint uv, int32 &maxUVs);
Boolean				PI3DAddFaceToMesh(PI3DMesh *mesh, PI3DFace *f, int32 &maxFaces);

// Material helpers
PI3DMaterial		*PI3DUpdateMaterials (PI3DScene *scene, const char *new_material, int ext); //update material list
int32				PI3DFindMaterial(PI3DScene *scene, const char *name);
PI3DMaterial		*PI3DGetMaterial(PI3DScene *scene, int32 index);

//matrix helpers
void				PI3DIdentityMatrix(PI3DMatrix & matrix);
void				PI3DCopyMatrix(PI3DMatrix & matrixsrc, PI3DMatrix & matrixdst);
void				PI3DInvertMatrix(PI3DMatrix & matrix, PI3DMatrix & mout);
void				PI3DTransformPoint(const PI3DVector& p, const PI3DMatrix & matrix, PI3DVector & pout);
void				PI3DInvertQuat(const PI3DQuat& quat, PI3DQuat & quatout);
void				PI3DQuatRotateMatrix(const PI3DQuat& quat, PI3DMatrix & matrixout);
void				PI3DMatrixMultiply( const PI3DMatrix &_a, const PI3DMatrix & _b, PI3DMatrix & matrixout);
void				PI3DTransform(const PI3DMatrix & matrixin1, const PI3DMatrix & matrixin2, PI3DMatrix & matrixout);
void				PI3DSetScale(const PI3DVector& s, PI3DMatrix & matrixout);
void				PI3DSetTranslate(const PI3DVector& s, PI3DMatrix & matrixout);
void				PI3DScale(PI3DMatrix & matrixin, const PI3DVector& s, PI3DMatrix & matrixout);
void				PI3DOffset(const PI3DVector& p, PI3DMatrix & matrix);
void				PI3DTranslate(PI3DMatrix & matrixin, const PI3DVector& s, PI3DMatrix & matrixout);
void				PI3DMatrixToPosition(PI3DMatrix & matrix,PI3DVector & position, PI3DVector & angle);
nativeFloat			PI3DDotProduct(const PI3DVector& v1, const PI3DVector& v2);
void				PI3DNormalize(PI3DVector & v);
void				PI3DVectorTransform(const PI3DMatrix & matrix , const PI3DVector & p, PI3DVector & dst);
void				PI3DMultiplyQuat(const PI3DQuat& u, const PI3DQuat& q, PI3DQuat &result);

// Scene calls
PI3DScene			*PI3DCreateScene(void *userData); // Call CreateScene to set up the scene graph
void				PI3DKillScene(PI3DScene *scene); // Call KillScene to tear down the scene graph

// Parse the Scene into and out of the descriptors to pass the scene to and from the engine
short				PI3DParseSceneIntoDescriptor(PSActionDescriptorProcs *actionDescriptorProcs,PSActionListProcs *actionListProcs,PI3DScene	*scene,PIDescriptorHandle *sceneDescriptor);
short				PI3DParseDescriptorIntoScene(PSActionDescriptorProcs *actionDescriptorProcs,PSActionListProcs *actionListProcs,PIDescriptorHandle sceneDescriptor,PI3DScene **scene);		

// String/path helpers
void				PI3DGetPathFromSpec(SPPlatformFileSpecificationW *fileSpec,uint16 *path);
uint32				PI3DStringLength16(const uint16 *string);
uint32				PI3DStringLengthW(const wchar_t *string);
void				PI3DStringCopyWC(char *destString,const wchar_t *sourceString);
void				PI3DStringCopy16C(char *destString,const uint16 *sourceString);
void				PI3DStringCopy16W(wchar_t *destString,const uint16 *sourceString);
void				PI3DStringCopyC16(uint16 *destString,const char *sourceString);

void				PI3DTrimRight(char *str);
void				PI3DTrimLeft(char *str);
Boolean				PI3DSplitString(char *leftStr, char *srcStr, bool splitOnWhitespace, char splitChar);
	
	
#ifdef __cplusplus
}
#endif
