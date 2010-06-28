#define _CRT_SECURE_NO_DEPRECATE 1
#include "PSScene.h"
#include "PITerminology.h"
#include "PIActions.h"
#include "PI3D.h"
#include <math.h>

#pragma warning(disable:4800)

#ifndef __ppc__
#define LOW_BYTE_FIRST 1
#endif

#if (LOW_BYTE_FIRST == 1)

#  define SWAP32_IF_BIGENDIAN(number)
#  define SWAP64_IF_BIGENDIAN(number)

#else
/* Swap bytes in 32 bit value.  */
#define U3D_BYTE_SWAP_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |               \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
/* Swap bytes in 64 bit value.  */
#define U3D_BYTE_SWAP_64(x) \
	(  ((((long long)x) & 0xff00000000000000ull) >> 56)                                    \
	| ((((long long)x) & 0x00ff000000000000ull) >> 40)                                 \
	| ((((long long)x) & 0x0000ff0000000000ull) >> 24)                                  \
	| ((((long long)x) & 0x000000ff00000000ull) >> 8)                                   \
	| ((((long long)x) & 0x00000000ff000000ull) << 8)                                   \
	| ((((long long)x) & 0x0000000000ff0000ull) << 24)                                  \
	| ((((long long)x) & 0x000000000000ff00ull) << 40)                                  \
	| ((((long long)x) & 0x00000000000000ffull) << 56))


#  define SWAP64_IF_BIGENDIAN(num) num = U3D_BYTE_SWAP_64(num)
#  define SWAP32_IF_BIGENDIAN(num) num = U3D_BYTE_SWAP_32(num)

#endif


void* PI3DMemoryAlloc(size_t size)
{
	errno = 0;
	void *result = malloc(size); //may fail or produce NULL, we need to figure out how to pass an erro back up the chain to Photoshop
	if (errno != 0)
		result = NULL;
	return result;
}

void PI3DMemoryFree(void* p)
{
	free(p);
}
/* Swap bytes in 64 bit value.  */
static void PI3DByteSwapDouble(double *x)
{
#ifdef __ppc__
    uint64 words = *(uint64 *)(x);
    SWAP64_IF_BIGENDIAN(words);
	*x = *((double*)&words);
#endif
}
/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/
/* Insert a new node into the list */
void PI3DListAdd (PI3DList **root, PI3DList *new_node)
{
#if 1
	new_node->next = NULL;
	if (*root == NULL)
	{
		*root = new_node;
	}
	else
	{
		PI3DList		*p = *root;
		while (p->next)
		{
			p = (PI3DList *)p->next;
		}
		p->next = new_node;
	}
#else
	new_node->next = *root;
	*root = new_node;
#endif
}

/* Find the node with the specified name in the scene */
void *PI3DListFindAll(PI3DScene *scene, const char *name)
{
	PI3DList *p = NULL;
	p = (PI3DList*)PI3DListSearch((PI3DList **)&scene->cameraList, name);
	if (p) return p;
	p = (PI3DList*)PI3DListSearch((PI3DList **)&scene->keyFrameList, name);
	if (p) return p;
	p = (PI3DList*)PI3DListSearch((PI3DList **)&scene->lightList, name);
	if (p) return p;
	p = (PI3DList*)PI3DListSearch((PI3DList **)&scene->matPropList, name);
	if (p) return p;
	p = (PI3DList*)PI3DListSearch((PI3DList **)&scene->meshList, name);
	return p;
}

/* Find the node with the specified name */
void *PI3DListSearch (PI3DList **root, const char *name)
{
	PI3DList *p;
	for (p = *root; p != (PI3DList *) NULL; p = (PI3DList *) p->next)
	{
		if (strcmp (p->name, name) == 0)
		{
			break;
		}
	}
	return (void *)p;
}

/* Delete the entire list */
void PI3DListDelete (PI3DList **root)
{
	PI3DList *temp;

	while (*root != (PI3DList *) NULL) 
	{
		temp = *root;
		*root = (PI3DList *) (*root)->next;
		PI3DMemoryFree (temp);
	}
}
/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/
void PI3DSceneInitGlobals(PI3DScene *scene)
{
	scene->globalAmbientColor.red = 0.1f;
	scene->globalAmbientColor.green = 0.1f;
	scene->globalAmbientColor.blue = 0.1f;
	scene->globalNumFrames = 0;
	scene->globalAnimationStartTime = 0;
	scene->globalFPS = 30.0f;
	scene->globalUnits = 1.0f; //inches
	strcpy (scene->globalRange, "");
}

PI3DMaterial *PI3DCreateMaterial()
{
	PI3DMaterial *new_mprop;
	PI3DColor defaultDiffuseSpec;
	PI3DColor defaultAmbient;
	PI3DColor black;

	black.blue = 0.0;
	black.green = 0.0;
	black.red = 0.0;
	defaultAmbient.blue = 30.0/255.0;
	defaultAmbient.green = 30.0/255.0;
	defaultAmbient.red = 30.0/255.0;
	defaultDiffuseSpec.blue = 192.0/255.0;
	defaultDiffuseSpec.green = 192.0/255.0;
	defaultDiffuseSpec.red= 192.0/255.0;

	new_mprop = (PI3DMaterial *) PI3DMemoryAlloc (sizeof(*new_mprop));
	strcpy (new_mprop->name, "");
	new_mprop->ambient = defaultAmbient;
	new_mprop->diffuse = defaultDiffuseSpec;
	new_mprop->specular = defaultDiffuseSpec;
	new_mprop->emissive = black;
	new_mprop->shininess = 0.2;
	new_mprop->glossiness =1.0;
	new_mprop->transparency = 0.0;
	new_mprop->reflection = 0.15;
	new_mprop->selfIllumination = 0;
	new_mprop->shading = PI3DPhong;
	new_mprop->decal = false;
	new_mprop->wireframe = false;
	new_mprop->wireframesize = 0.0;
	new_mprop->twoSided = true;
	strcpy (new_mprop->maps[PI3DTextureMap].map, "");
	new_mprop->maps[PI3DTextureMap].strength = 1.0;
	new_mprop->maps[PI3DTextureMap].uscale = 1.0;
	new_mprop->maps[PI3DTextureMap].vscale = 1.0;
	new_mprop->maps[PI3DTextureMap].uoffset = 0.0;
	new_mprop->maps[PI3DTextureMap].voffset = 0.0;
	new_mprop->maps[PI3DTextureMap].angle = 0.0;
	new_mprop->maps[PI3DTextureMap].flags = PI3DColorMap;
	new_mprop->maps[PI3DTextureMap].mapIs3DSStyle = false;
	strcpy (new_mprop->maps[PI3DBumpMap].map, "");
	new_mprop->maps[PI3DBumpMap].strength = 1.0;
	new_mprop->maps[PI3DBumpMap].uscale = 1.0;
	new_mprop->maps[PI3DBumpMap].vscale = 1.0;
	new_mprop->maps[PI3DBumpMap].uoffset = 0.0;
	new_mprop->maps[PI3DBumpMap].voffset = 0.0;
	new_mprop->maps[PI3DBumpMap].angle = 0.0;
	new_mprop->maps[PI3DBumpMap].flags = PI3DColorMap;
	new_mprop->maps[PI3DBumpMap].mapIs3DSStyle = false;
	strcpy (new_mprop->maps[PI3DReflectionMap].map, "");
	new_mprop->maps[PI3DReflectionMap].strength = 1.0;
	new_mprop->maps[PI3DReflectionMap].uscale = 1.0;
	new_mprop->maps[PI3DReflectionMap].vscale = 1.0;
	new_mprop->maps[PI3DReflectionMap].uoffset = 0.0;
	new_mprop->maps[PI3DReflectionMap].voffset = 0.0;
	new_mprop->maps[PI3DReflectionMap].angle = 0.0;
	new_mprop->maps[PI3DReflectionMap].flags = PI3DColorMap;
	new_mprop->maps[PI3DReflectionMap].mapIs3DSStyle = false;
	strcpy (new_mprop->maps[PI3DSpecularMap].map, "");
	new_mprop->maps[PI3DSpecularMap].strength = 1.0;
	new_mprop->maps[PI3DSpecularMap].uscale = 1.0;
	new_mprop->maps[PI3DSpecularMap].vscale = 1.0;
	new_mprop->maps[PI3DSpecularMap].uoffset = 0.0;
	new_mprop->maps[PI3DSpecularMap].voffset = 0.0;
	new_mprop->maps[PI3DSpecularMap].angle = 0.0;
	new_mprop->maps[PI3DSpecularMap].flags = PI3DColorMap;  
	new_mprop->maps[PI3DSpecularMap].mapIs3DSStyle = false;
	strcpy (new_mprop->maps[PI3DOpacityMap].map, "");
	new_mprop->maps[PI3DOpacityMap].strength = 1.0;
	new_mprop->maps[PI3DOpacityMap].uscale = 1.0;
	new_mprop->maps[PI3DOpacityMap].vscale = 1.0;
	new_mprop->maps[PI3DOpacityMap].uoffset = 0.0;
	new_mprop->maps[PI3DOpacityMap].voffset = 0.0;
	new_mprop->maps[PI3DOpacityMap].angle = 0.0;
	new_mprop->maps[PI3DOpacityMap].flags = PI3DColorMap;
	new_mprop->maps[PI3DOpacityMap].mapIs3DSStyle = false;
	strcpy (new_mprop->maps[PI3DShininessMap].map, "");
	new_mprop->maps[PI3DShininessMap].strength = 0.0;
	new_mprop->maps[PI3DShininessMap].uscale = 1.0;
	new_mprop->maps[PI3DShininessMap].vscale = 1.0;
	new_mprop->maps[PI3DShininessMap].uoffset = 0.0;
	new_mprop->maps[PI3DShininessMap].voffset = 0.0;
	new_mprop->maps[PI3DShininessMap].angle = 0.0;
	new_mprop->maps[PI3DShininessMap].flags = PI3DColorMap;  
	new_mprop->maps[PI3DShininessMap].mapIs3DSStyle = false;
	strcpy (new_mprop->maps[PI3DSelfIlluminationMap].map, "");
	new_mprop->maps[PI3DSelfIlluminationMap].strength = 1.0;
	new_mprop->maps[PI3DSelfIlluminationMap].uscale = 1.0;
	new_mprop->maps[PI3DSelfIlluminationMap].vscale = 1.0;
	new_mprop->maps[PI3DSelfIlluminationMap].uoffset = 0.0;
	new_mprop->maps[PI3DSelfIlluminationMap].voffset = 0.0;
	new_mprop->maps[PI3DSelfIlluminationMap].angle = 0.0;
	new_mprop->maps[PI3DSelfIlluminationMap].flags = PI3DColorMap;  //falco new_mprop->aProperty = PI3DProperty::New ();
	new_mprop->maps[PI3DSelfIlluminationMap].mapIs3DSStyle = false;
	return new_mprop;
}

PI3DCamera *PI3DCreateCamera (void)
{
	PI3DCamera *new_cprop;

	new_cprop = (PI3DCamera *) PI3DMemoryAlloc (sizeof(*new_cprop));
	strcpy (new_cprop->name, "");
	new_cprop->aspectratio = 0.0;
	new_cprop->bank = 0.0;
	new_cprop->lens = 42.0;
	new_cprop->ortho = false;
	new_cprop->pos[0] = 0.0;
	new_cprop->pos[1] = 0.0;
	new_cprop->pos[2] = 0.0;
	new_cprop->target[0] = 0.0;
	new_cprop->target[1] = 0.0;
	new_cprop->target[2] = 0.0;
	new_cprop->zoomfactor = 0.0;
	return new_cprop;
}

PI3DLight *PI3DCreateLight (void)
{
	PI3DLight *new_lprop;

	new_lprop = (PI3DLight *) PI3DMemoryAlloc (sizeof(*new_lprop));
	strcpy (new_lprop->name, "");
	new_lprop->attenuation = false;
	new_lprop->attenuationAbc.a = 0.0;
	new_lprop->attenuationAbc.b = 0.0;
	new_lprop->attenuationAbc.c = 0.0;
	new_lprop->attenuationType = PI3DLinearAttenuation;
	new_lprop->col.red = 0.0;
	new_lprop->col.green = 0.0;
	new_lprop->col.blue = 0.0;
	new_lprop->falloff = 180.0;
	new_lprop->hotspot = 0.7f*new_lprop->falloff;
	new_lprop->innerRadius = 0;
	new_lprop->multiple = 1.0;
	new_lprop->outerRadius = 0;
	new_lprop->pos[0] = 0.0;
	new_lprop->pos[1] = 0.0;
	new_lprop->pos[2] = 0.0;
	new_lprop->target[0] = 0.0;
	new_lprop->target[1] = 0.0;
	new_lprop->target[2] = 0.0;
	new_lprop->shadowFlag = false;
	new_lprop->type = PI3DDirectionalLight;
	return new_lprop;
}

PI3DKeyFrame *PI3DCreateKeyframe()
{
	PI3DKeyFrame *new_kfprop;

	new_kfprop = (PI3DKeyFrame *) PI3DMemoryAlloc (sizeof(*new_kfprop));
	strcpy (new_kfprop->name, "");
	strcpy (new_kfprop->instancename, "");
	new_kfprop->flags1 = 0;
	new_kfprop->flags2 = 0;
	new_kfprop->nodeID = 0;
	new_kfprop->parentNodeID = 0;
	new_kfprop->nodeType = PI3DObjectNode;
	new_kfprop->pivot[0] = 0.0;
	new_kfprop->pivot[1] = 0.0;
	new_kfprop->pivot[2] = 0.0;
	new_kfprop->usepivot = true;
	new_kfprop->matrixPresent = false;
	new_kfprop->userange = false;
	new_kfprop->positionData = NULL;
	new_kfprop->rotationData = NULL;
	new_kfprop->scaleData = NULL;
	new_kfprop->fOVData = NULL;
	new_kfprop->rollData = NULL;
	new_kfprop->fallData = NULL;
	new_kfprop->colData = NULL;
	new_kfprop->hotData = NULL;
	new_kfprop->morphData = NULL;
	new_kfprop->hideData = NULL;
	return new_kfprop;
}

/* Create a new mesh */
PI3DMesh *PI3DCreateMesh (char *name, int vertices, int faces, int textures, int normals, int lines, int colors)
{
	PI3DMesh *new_mesh;

	new_mesh = (PI3DMesh *) PI3DMemoryAlloc (sizeof(*new_mesh));
	strcpy (new_mesh->name, name);
	new_mesh->next = NULL;
	new_mesh->flags = 0x57;

	new_mesh->smoothingGroupPresent = false;
	new_mesh->matrixPresent = false;

	new_mesh->vertices = vertices;

	if (vertices <= 0)
	{
		new_mesh->vertex = NULL;
	}
	else
	{
		new_mesh->vertex = (PI3DVector *) PI3DMemoryAlloc(vertices * sizeof(*new_mesh->vertex));
	}

	new_mesh->colors = colors;

	if (colors <= 0)
	{
		new_mesh->color = NULL;
	}
	else
	{
		new_mesh->color = (PI3DVector *) PI3DMemoryAlloc(colors * sizeof(*new_mesh->color));
	}

	new_mesh->lines = lines;

	if (lines <= 0)
	{
		new_mesh->linecolors = NULL;
	}
	else
	{
		new_mesh->linecolors = (PI3DVector *) PI3DMemoryAlloc(lines * sizeof(*new_mesh->linecolors));
	}

	new_mesh->textures = textures;

	if (textures <= 0)
	{
		new_mesh->texture = NULL;
	}
	else
	{
		new_mesh->texture = (PI3DPoint *) PI3DMemoryAlloc(textures * sizeof(*new_mesh->texture));
	}

	new_mesh->normals = normals;

	if (normals <= 0)
	{
		new_mesh->normal = NULL;
	}
	else
	{
		new_mesh->normal = (PI3DVector *) PI3DMemoryAlloc(normals * sizeof(*new_mesh->normal));
	}


	new_mesh->faces = faces;

	if (faces <= 0)
	{
		new_mesh->face = NULL;
		//new_mesh->mtl = NULL;
	}
	else
	{
		new_mesh->face = (PI3DFace *) PI3DMemoryAlloc (faces * sizeof(*new_mesh->face));
		//new_mesh->mtl = (PI3DMaterial **) PI3DMemoryAlloc (faces * sizeof(*new_mesh->mtl));
	}

	if (lines <= 0)
	{
		new_mesh->line = NULL;
		new_mesh->linematerial = NULL;
	}
	else
	{
		new_mesh->line = (PI3DVector *) PI3DMemoryAlloc (lines * sizeof(*new_mesh->line));
		new_mesh->linematerial = (int32 *) PI3DMemoryAlloc (lines * sizeof(*new_mesh->linematerial));
	}

	new_mesh->hidden = 0;
	new_mesh->shadow = 1;

	return new_mesh;
}

/* Create a new animation */
PI3DAnimationData *PI3DCreateAnimation (int32 flags, int32 keys)
{
	PI3DAnimationData *new_anim;

	new_anim = (PI3DAnimationData *) PI3DMemoryAlloc (sizeof(*new_anim));
	new_anim->flags = flags;
	new_anim->keys = keys;

	if (keys <= 0)
	{
		new_anim->aFrameData = NULL;
	}
	else
	{
		new_anim->aFrameData = (PI3DFrameVector *) PI3DMemoryAlloc(keys * sizeof(*new_anim->aFrameData));
		for (int x = 0;x< keys; x++)
			memset((void*)&new_anim->aFrameData[x],0,sizeof(*new_anim->aFrameData));
	}

	return new_anim;
}
/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

/* Add a vertex to a mesh */
Boolean PI3DAddVertexToMesh(PI3DMesh *mesh, PI3DVector v, int32 &maxVertices)
{
	if (mesh->vertices == 0)
	{ // the list is empty - create a starter list
		mesh->vertex = (PI3DVector *)PI3DMemoryAlloc(sizeof(PI3DVector) * maxVertices);
	}
	else if (mesh->vertices == maxVertices)
	{ // the list is full - expand it
		maxVertices *= 2;

		PI3DVector	*newList = (PI3DVector *)PI3DMemoryAlloc(sizeof(PI3DVector) * maxVertices);
		if (newList == NULL)
		{
			maxVertices = mesh->vertices + 100;
			newList = (PI3DVector *)PI3DMemoryAlloc(sizeof(PI3DVector) * maxVertices);
		}
		if (newList == NULL)
			return false;
		for (int32 i = 0; i < mesh->vertices; i++)
		{
			newList[i][0] = mesh->vertex[i][0];
			newList[i][1] = mesh->vertex[i][1];
			newList[i][2] = mesh->vertex[i][2];
		}
		PI3DMemoryFree(mesh->vertex);
		mesh->vertex = newList;
	}
	// switch coordinates around to make OBJ "up" correspond to Photoshop's "up"
	mesh->vertex[mesh->vertices][0] = v[0];
	mesh->vertex[mesh->vertices][1] = -v[2];
	mesh->vertex[mesh->vertices][2] = v[1];
	mesh->vertices++;
	return true;
}
/* Add a normal to a mesh */
Boolean PI3DAddNormalToMesh(PI3DMesh *mesh, PI3DVector n, int32 &maxNormals)
{
	if (mesh->normals == 0)
	{ // the list is empty - create a starter list
		mesh->normal = (PI3DVector *)PI3DMemoryAlloc(sizeof(PI3DVector) * maxNormals);
	}
	else if (mesh->normals == maxNormals)
	{ // the list is full - expand it
		maxNormals *= 2;

		PI3DVector	*newList = (PI3DVector *)PI3DMemoryAlloc(sizeof(PI3DVector) * maxNormals);
		if (newList == NULL)
		{
			maxNormals = mesh->normals + 100;
			newList = (PI3DVector *)PI3DMemoryAlloc(sizeof(PI3DVector) * maxNormals);
		}
		if (newList == NULL)
			return false;
		for (int32 i = 0; i < mesh->normals; i++)
		{
			newList[i][0] = mesh->normal[i][0];
			newList[i][1] = mesh->normal[i][1];
			newList[i][2] = mesh->normal[i][2];
		}
		PI3DMemoryFree(mesh->normal);
		mesh->normal = newList;
	}
	// switch coordinates around to make OBJ "up" correspond to Photoshop's "up"
	mesh->normal[mesh->normals][0] = n[0];
	mesh->normal[mesh->normals][1] = -n[2];
	mesh->normal[mesh->normals][2] = n[1];
	mesh->normals++;
	return true;
}
/* Add a texture coordinate (UV) to a mesh */
Boolean PI3DAddUVToMesh(PI3DMesh *mesh, PI3DPoint uv, int32 &maxUVs)
{
	if (mesh->textures == 0)
	{ // the list is empty - create a starter list
		mesh->texture = (PI3DPoint *)PI3DMemoryAlloc(sizeof(PI3DPoint) * maxUVs);
	}
	else if (mesh->textures == maxUVs)
	{ // the list is full - expand it
		maxUVs *= 2;

		PI3DPoint	*newList = (PI3DPoint *)PI3DMemoryAlloc(sizeof(PI3DPoint) * maxUVs);
		if (newList == NULL)
		{
			maxUVs = mesh->textures + 100;
			newList = (PI3DPoint *)PI3DMemoryAlloc(sizeof(PI3DPoint) * maxUVs);
		}
		if (newList == NULL)
			return false;
		for (int32 i = 0; i < mesh->textures; i++)
		{
			newList[i][0] = mesh->texture[i][0];
			newList[i][1] = mesh->texture[i][1];
		}
		PI3DMemoryFree(mesh->texture);
		mesh->texture = newList;
	}
	// switch coordinates around to make OBJ "up" correspond to Photoshop's "up"
	mesh->texture[mesh->textures][0] = uv[0];
	mesh->texture[mesh->textures][1] = uv[1];
	mesh->textures++;
	return true;
}

/* Add a face to a mesh */
Boolean PI3DAddFaceToMesh(PI3DMesh *mesh, PI3DFace *f, int32 &maxFaces)
{
	if (mesh->faces == 0)
	{ // the list is empty - create a starter list
		mesh->face = (PI3DFace *)PI3DMemoryAlloc(sizeof(PI3DFace) * maxFaces);
	}
	else if (mesh->faces == maxFaces)
	{ // the list is full - expand it
		maxFaces *= 2;

		PI3DFace		*newFaces = (PI3DFace *)PI3DMemoryAlloc(sizeof(PI3DFace) * maxFaces);
		if (newFaces == NULL)
		{
			maxFaces = mesh->faces + 100;
			newFaces = (PI3DFace *)PI3DMemoryAlloc(sizeof(PI3DFace) * maxFaces);
		}
		if (newFaces == NULL)
			return false;
		for (int32 i = 0; i < mesh->faces; i++)
		{
			newFaces[i] = mesh->face[i];
		}
		PI3DMemoryFree(mesh->face);
		mesh->face = newFaces;
	}
	mesh->face[mesh->faces] = *f;
	mesh->faces++;
	return true;
}


/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

/* Add a new material to the material list */
PI3DMaterial *PI3DUpdateMaterials (PI3DScene *scene, const char *new_material, int ext)
{
	PI3DMaterial *p;

	p = (PI3DMaterial *) PI3DListSearch((PI3DList **)&scene->matPropList, new_material);

	if (p == NULL)
	{
		p = PI3DCreateMaterial();
		strcpy (p->name, new_material);
		//¥¥¥p->external = ext;
		PI3DListAdd((PI3DList **)&scene->matPropList, reinterpret_cast<PI3DList *>(p));
	}
	return p;
}

/* Find a material by name in the scene, and return its index number. */
/* If the material is not found, return -1. */
int32 PI3DFindMaterial(PI3DScene *scene, const char *name)
{
	int32		i = 0;
	PI3DMaterial	*p;
	for (p = scene->matPropList; p != NULL; p = (PI3DMaterial *)p->next)
	{
		if (strcmp(p->name, name) == 0)
		{
			return i;
		}
		i++;
	}
	return -1;
}

/* Fetch a material from a scene by its index number. */
PI3DMaterial *PI3DGetMaterial(PI3DScene *scene, int32 index)
{
	int32		i = 0;
	PI3DMaterial	*p;
	for (p = scene->matPropList; p != NULL; p = (PI3DMaterial *)p->next)
	{
		if (i == index)
		{
			return p;
		}
		i++;
	}
	return NULL;
}

/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/
void PI3DInvertMatrix(PI3DMatrix & matrix, PI3DMatrix & mout)
{
	double pos, neg, t;
	PI3DIdentityMatrix(mout);
	// Calculate the determinant of submatrix A and determine if the
	// the matrix is singular as limited by the double precision
	// floating-point data representation.

	pos = neg = 0.0;
	t = matrix[0][0] * matrix[1][1] * matrix[2][2];
	if ( t >= 0.0 )
		pos += t;
	else
		neg += t;

	t = matrix[1][0] * matrix[2][1] * matrix[0][2];
	if ( t >= 0.0 )
		pos += t;
	else
		neg += t;

	t = matrix[2][0] * matrix[0][1] * matrix[1][2];
	if ( t >= 0.0 )
		pos += t;
	else
		neg += t;

	t = -matrix[2][0] * matrix[1][1] * matrix[0][2];
	if ( t >= 0.0 )
		pos += t;
	else
		neg += t;

	t = -matrix[1][0] * matrix[0][1] * matrix[2][2];
	if ( t >= 0.0 )
		pos += t;
	else
		neg += t;

	t = -matrix[0][0] * matrix[2][1] * matrix[1][2];
	if ( t >= 0.0 )
		pos += t;
	else
		neg += t;

	double det = pos + neg;

	// Is the submatrix A singular?
	if ( fabs(det) < ((pos - neg) * 1.0e-12) )
	{
		//Set();
		return ;
	}
	if ( det == 0 )
	{
		//Set();
		return ;

	}
	// Calculate inverse(A) = adj(A) / det(A)

	double det_1 = 1.0 / det;
	mout[0][0] = ( (matrix[1][1] * matrix[2][2] - matrix[2][1] * matrix[1][2] ) * det_1);
	mout[0][1] = ( - (matrix[0][1] * matrix[2][2] - matrix[2][1] * matrix[0][2] ) * det_1);
	mout[0][2] = ( (matrix[0][1] * matrix[1][2] - matrix[1][1] * matrix[0][2] ) * det_1);
	mout[1][0] = ( - (matrix[1][0] * matrix[2][2] - matrix[2][0] * matrix[1][2] ) * det_1);
	mout[1][1] = ( (matrix[0][0] * matrix[2][2] - matrix[2][0] * matrix[0][2] ) * det_1);
	mout[1][2] = ( - (matrix[0][0] * matrix[1][2] - matrix[1][0] * matrix[0][2] ) * det_1);
	mout[2][0] = ( (matrix[1][0] * matrix[2][1] - matrix[2][0] * matrix[1][1] ) * det_1);
	mout[2][1] = ( - (matrix[0][0] * matrix[2][1] - matrix[2][0] * matrix[0][1] ) * det_1);
	mout[2][2] = ( (matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1] ) * det_1);

	/* Calculate -C * inverse(A) */
	mout[3][0] = - (matrix[3][0] * mout[0][0] +
		matrix[3][1] * mout[1][0] +
		matrix[3][2] * mout[2][0] );

	mout[3][1] = - (matrix[3][0] * mout[0][1] +
		matrix[3][1] * mout[1][1] +
		matrix[3][2] * mout[2][1] );


	mout[3][2] = - (matrix[3][0] * mout[0][2] +
		matrix[3][1] * mout[1][2] +
		matrix[3][2] * mout[2][2] );
}

void PI3DTransformPoint(const PI3DVector& p, const PI3DMatrix & matrix, PI3DVector & pout)
{
	pout[0] = (p[0] * matrix[0][0] + p[1] * matrix[1][0] + p[2] * matrix[2][0] + matrix[3][0]);
	pout[1] = (p[0] * matrix[0][1] + p[1] * matrix[1][1] + p[2] * matrix[2][1] + matrix[3][1]);
	pout[2] = (p[0] * matrix[0][2] + p[1] * matrix[1][2] + p[2] * matrix[2][2] + matrix[3][2]);
}

void PI3DCopyMatrix(PI3DMatrix & matrixsrc, PI3DMatrix & matrixdst)
{
	for(uint32 i=0;i<4;i++)
		for(uint32 j=0;j<4;j++)
			matrixdst[i][j] = matrixsrc[i][j];
}

void PI3DIdentityMatrix(PI3DMatrix & matrix)
{
	memset(matrix, 0, sizeof(PI3DMatrix));
	matrix[0][0] = matrix[1][1] = matrix[2][2] = matrix[3][3] = 1.0;
};

void PI3DInvertQuat(const PI3DQuat& quat, PI3DQuat & quatout)
{
	double norm =quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z;
	if ( norm > 0.0f )
	{
		norm = 1.0 / norm;
		quatout.w = (float)(quat.w*norm);
		quatout.x = (float)( -(quat.x*norm));
		quatout.y = (float)( -(quat.y*norm));
		quatout.z = (float)( -(quat.z*norm));
	}
}

void PI3DMultiplyQuat(const PI3DQuat& u, const PI3DQuat& q, PI3DQuat &result)
{
	result.w = u.w*q.w - u.x*q.x - u.y*q.y - u.z*q.z;
	result.x = u.w*q.x + u.x*q.w + u.y*q.z - u.z*q.y;
	result.y = u.w*q.y + u.y*q.w + u.z*q.x - u.x*q.z;
	result.z = u.w*q.z + u.z*q.w + u.x*q.y - u.y*q.x;
};

void PI3DQuatRotateMatrix(const PI3DQuat& quat, PI3DMatrix & matrixout)
{
	nativeFloat wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	// calculate coefficients
	x2 = quat.x + quat.x;
	y2 = quat.y + quat.y;
	z2 = quat.z + quat.z;
	xx = quat.x * x2;
	xy = quat.x * y2;
	xz = quat.x * z2;
	yy = quat.y * y2;
	yz = quat.y * z2;
	zz = quat.z * z2;
	wx = quat.w * x2;
	wy = quat.w * y2;
	wz = quat.w * z2;

	matrixout[0][0] = 1.0 - (yy + zz);
	matrixout[0][1] = xy - wz;
	matrixout[0][2] = xz + wy;
	matrixout[0][3] = 0.0;

	matrixout[1][0] = xy + wz;
	matrixout[1][1] = 1.0 - (xx + zz);
	matrixout[1][2] = yz - wx;
	matrixout[1][3] = 0.0;

	matrixout[2][0] = xz - wy;
	matrixout[2][1] = yz + wx;
	matrixout[2][2] = 1.0 - (xx + yy);
	matrixout[2][3] = 0.0;

	matrixout[3][0] = 0;
	matrixout[3][1] = 0;
	matrixout[3][2] = 0;
	matrixout[3][3] = 1;

};

void PI3DMatrixMultiply( const PI3DMatrix &_a, const PI3DMatrix & _b, PI3DMatrix & matrixout)
{
	for ( int i = 0; i < 4; i++ )
	{
		for ( int j = 0; j < 4; j++ )
		{
			matrixout[i][j] = (
				_a[i][0] * _b[0][j] +
				_a[i][1] * _b[1][j] +
				_a[i][2] * _b[2][j] +
				_a[i][3] * _b[3][j]);
		}
	}
};


void PI3DTransform(const PI3DMatrix & matrixin1, const PI3DMatrix & matrixin2, PI3DMatrix & matrixout)
{
	PI3DMatrixMultiply(matrixin1, matrixin2, matrixout);
}

void PI3DSetScale(const PI3DVector& s, PI3DMatrix & matrixout)
{
	memset(matrixout, 0, sizeof(PI3DMatrix));
	matrixout[0][0] = s[0];
	matrixout[1][1] = s[1];
	matrixout[2][2] = s[2];
	matrixout[3][3] = 1.0;
}

void PI3DScale(PI3DMatrix & matrixin, const PI3DVector& s, PI3DMatrix & matrixout)
{
	if ( (s[0] == 1.0f) && (s[1] == 1.0f) && (s[2] == 1.0f) )
	{
		PI3DCopyMatrix(matrixin,matrixout);
		return ;
	}
	PI3DMatrix m;
	PI3DSetScale(s,m);
	PI3DTransform(matrixin, m, matrixout);
};

void PI3DSetTranslate(const PI3DVector& s, PI3DMatrix & matrixout)
{
	PI3DIdentityMatrix(matrixout);
	matrixout[3][0] = s[0];
	matrixout[3][1] = s[1];
	matrixout[3][2] = s[2];	
}

void PI3DOffset(const PI3DVector& p, PI3DMatrix & matrix)
{
	matrix[3][0] += p[0];
	matrix[3][1] += p[1];
	matrix[3][2] += p[2];
};

void PI3DTranslate(PI3DMatrix & matrixin, const PI3DVector& s, PI3DMatrix & matrixout)
{
	PI3DMatrix m;
	PI3DSetTranslate(s,m);
	PI3DTransform(matrixin, m, matrixout);
};

void PI3DVectorTransform(const PI3DMatrix & matrix , const PI3DVector & p, PI3DVector & dst)
{
	dst[0] = (nativeFloat)(p[0] * matrix[0][0] + p[1] * matrix[1][0] + p[2] * matrix[2][0]);
	dst[1] = (nativeFloat)(p[0] * matrix[0][1] + p[1] * matrix[1][1] + p[2] * matrix[2][1]);
	dst[2] = (nativeFloat)(p[0] * matrix[0][2] + p[1] * matrix[1][2] + p[2] * matrix[2][2]);
};

void PI3DMatrixToPosition(PI3DMatrix & matrix,PI3DVector & position, PI3DVector & angle)
{

	nativeFloat cy = sqrt( matrix[0][0] * 
		matrix[0][0] +
		matrix[1][0] * 
		matrix[1][0] );

	angle[1] = -asin( matrix[0][2]);        /* Calculate Y-axis angle */
	double C           =  cos( angle[1] );
	angle[1]    *= R2D;
	double trx,trya;

	if ( fabs( C ) > 0.005 )             /* Gimball lock? */
	{
		trx      =  matrix[2][2] / C;           /* No, so get X-axis angle */
		trya      = -matrix[1][2]  / C;

		angle[0]  = atan2( trya, trx ) * R2D;

		trx      =  matrix[0][0] / C;            /* Get Z-axis angle */
		trya      = -matrix[0][1] / C;

		angle[2]  = atan2( trya, trx ) * R2D;
	}
	else                                 /* Gimball lock has occurred */
	{
		angle[0]  = 0;                      /* Set X-axis angle to zero */

		trx      = matrix[1][1];                 /* And calculate Z-axis angle */
		trya      = matrix[1][0];

		angle[2]  = atan2( trya, trx ) * R2D;
	}

	position[0]=matrix[3][0];
	position[1]=matrix[3][1];
	position[2]=matrix[3][2];
}

nativeFloat PI3DDotProduct(const PI3DVector& v1, const PI3DVector& v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void PI3DNormalize(PI3DVector & v)
{
	nativeFloat r = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if ( r == 1.0f )
		return;

	r = sqrtf((float)r);

	if ( r )
	{
		v[0] /= r;
		v[1] /= r;
		v[2] /= r;
	}
	else
	{
		v[0] = v[1] = 0;
		v[2] = 1.0;
	}
};

/******************************************************************************/

void PI3DCleanupName (char *name)
{
	char *tmp = (char *) PI3DMemoryAlloc (strlen(name)+2);
	int  i;

	/* Remove any leading blanks or quotes */
	i = 0;
	while ((name[i] == ' ' || name[i] == '"') && name[i] != '\0')
	{
		i++;
	}
	strcpy (tmp, &name[i]);

	/* Remove any trailing blanks or quotes */
	for (i = static_cast<int>(strlen(tmp))-1; i >= 0; i--)
	{
		if (isprint(tmp[i]) && !isspace(tmp[i]) && tmp[i] != '"')
		{
			break;
		}
		else
		{
			tmp[i] = '\0';
		}
	}

	strcpy (name, tmp);

	/* Prefix the letter 'N' to materials that begin with a digit */
	if (!isdigit (name[0]))
	{
		strcpy (tmp, name);
	}
	else 
	{
		tmp[0] = 'N';
		strcpy (&tmp[1], name);
	}

	/* Replace all illegal charaters in name with underscores */
	for (i = 0; tmp[i] != '\0'; i++)
	{
		if (!isalnum(tmp[i]))
		{
			tmp[i] = '_';
		}
	}

	strcpy (name, tmp);

	PI3DMemoryFree (tmp);
}

PI3DScene *PI3DCreateScene(void *userData)
{
	PI3DImport *theImporter=(PI3DImport*)PI3DMemoryAlloc(sizeof(PI3DImport));
	theImporter->userData=userData;
	PI3DScene *scene = (PI3DScene*)PI3DMemoryAlloc(sizeof(PI3DScene));
	memset(scene,0,sizeof(PI3DScene));
	PI3DSceneInitGlobals(scene);
	theImporter->scene=scene;
	if (PI3DParseFile (theImporter) == 0)
	{
#ifdef DEBUG
		wchar_t message[256];
		size_t copiedsize = mbstowcs(message, chunkErrorMsg, 256);
		pContext->Message(0,message);
#endif
		return 0;
	}
	PI3DMemoryFree(theImporter);
	return scene;

}
void PI3DKillScene(PI3DScene *scene)
{
	if(!scene)
		return;

	//Taking this out since the data is now being passed in
	// walk the light list and delete PI3D objects
	if(scene->lightList)
		PI3DListDelete((PI3DList **)&scene->lightList);
	scene->lightList = NULL;

	// walk the camera list and delete PI3D objects
	if(scene->cameraList)
		PI3DListDelete((PI3DList **)&scene->cameraList);
	scene->cameraList = NULL;

	// walk the mesh list and delete malloced datra and PI3D objects
	PI3DMesh *mesh;
	for (mesh = scene->meshList; mesh != (PI3DMesh *) NULL; 
		mesh = (PI3DMesh *) mesh->next)
	{
		if (mesh->vertex)
		{
			PI3DMemoryFree (mesh->vertex);
		}
		if (mesh->normal)
		{
			PI3DMemoryFree (mesh->normal);
		}
		if (mesh->face)
		{
			for(int i=0;i<mesh->faces;i++)
			{
				PI3DMemoryFree(mesh->face[i].points);
				PI3DMemoryFree(mesh->face[i].normals);
				PI3DMemoryFree(mesh->face[i].textures);
				PI3DMemoryFree(mesh->face[i].colors);
			}

			PI3DMemoryFree (mesh->face);
		}
		if (mesh->texture)
		{
			PI3DMemoryFree (mesh->texture);
		}
		if (mesh->color)
		{
			PI3DMemoryFree (mesh->color);
		}
		if (mesh->linecolors)
		{
			PI3DMemoryFree (mesh->linecolors);
		}
		if (mesh->line)
		{
			PI3DMemoryFree (mesh->line);
		}
		if (mesh->linematerial)
		{
			PI3DMemoryFree (mesh->linematerial);
		}
	}
	if(scene->meshList)
		PI3DListDelete((PI3DList **)&scene->meshList);
	scene->meshList = NULL;

	// walk the mesh list and delete malloced datra and PI3D objects
	PI3DKeyFrame* keyframe;
	for (keyframe = scene->keyFrameList; keyframe != (PI3DKeyFrame *) NULL; 
		keyframe = (PI3DKeyFrame *) keyframe->next)
	{
		if (keyframe->positionData)
		{
			if(keyframe->positionData->aFrameData)
				PI3DMemoryFree (keyframe->positionData->aFrameData);
				
			PI3DListDelete((PI3DList **)&keyframe->positionData);
		}
		if (keyframe->rotationData)
		{
			if(keyframe->rotationData->aFrameData)
				PI3DMemoryFree (keyframe->rotationData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->rotationData);
		}
		if (keyframe->scaleData)
		{
			if(keyframe->scaleData->aFrameData)
				PI3DMemoryFree (keyframe->scaleData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->scaleData);
		}
		if (keyframe->rollData)
		{
			if(keyframe->rollData->aFrameData)
				PI3DMemoryFree (keyframe->rollData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->rollData);
		}
		if (keyframe->fOVData)
		{
			if(keyframe->fOVData->aFrameData)
				PI3DMemoryFree (keyframe->fOVData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->fOVData);
		}
		if (keyframe->hotData)
		{
			if(keyframe->hotData->aFrameData)
				PI3DMemoryFree (keyframe->hotData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->hotData);
		}
		if (keyframe->colData)
		{
			if(keyframe->colData->aFrameData)
				PI3DMemoryFree (keyframe->colData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->colData);
		}
		if (keyframe->fallData)
		{
			if(keyframe->fallData->aFrameData)
				PI3DMemoryFree (keyframe->fallData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->fallData);
		}
		if (keyframe->hideData)
		{
			if(keyframe->hideData->aFrameData)
				PI3DMemoryFree (keyframe->hideData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->hideData);
		}
		if (keyframe->morphData)
		{
			if(keyframe->morphData->aFrameData)
				PI3DMemoryFree (keyframe->morphData->aFrameData);
			PI3DListDelete((PI3DList **)&keyframe->morphData);
		}
	}
	if(scene->keyFrameList)
		PI3DListDelete((PI3DList **)&scene->keyFrameList);
	scene->keyFrameList = NULL;
	
	if(scene->matPropList)
		PI3DListDelete((PI3DList **)&scene->matPropList);
	scene->matPropList = NULL;

	PI3DMemoryFree( scene);

}



short PI3DParseAnimDataIntoDescriptor(PSActionDescriptorProcs *actionDescriptorProcs,PSActionListProcs *actionListProcs, PI3DAnimationData* pAnimData, PIActionDescriptor* pReturnDescriptor)
{
	actionDescriptorProcs->Make(pReturnDescriptor);

	// name
	actionDescriptorProcs->PutString(*pReturnDescriptor,keyNameAnim, pAnimData->name);
	//flags
	actionDescriptorProcs->PutInteger(*pReturnDescriptor, keyFlags, pAnimData->flags);
	//keys
	actionDescriptorProcs->PutInteger(*pReturnDescriptor, keyKeys, pAnimData->keys);

	//animation frame data
	PIActionList animframeActionList;
	actionListProcs->Make(&animframeActionList);
	for(int32 index=0;index<pAnimData->keys;index++)
	{
		PIActionDescriptor animframeDescriptor;
		actionDescriptorProcs->Make(&animframeDescriptor);

		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameNumber, pAnimData->aFrameData[index].framenumber);
		actionDescriptorProcs->PutInteger(animframeDescriptor, keyAnimFrameFlags, pAnimData->aFrameData[index].flags);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameTension, pAnimData->aFrameData[index].tension);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameContinuity, pAnimData->aFrameData[index].continuity);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameBias, pAnimData->aFrameData[index].bias);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameEaseTo, pAnimData->aFrameData[index].easeto);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameEaseFrom, pAnimData->aFrameData[index].easefrom);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameRotation, pAnimData->aFrameData[index].rotation);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameValue, pAnimData->aFrameData[index].value);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameVectorX, pAnimData->aFrameData[index].v[0]);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameVectorY, pAnimData->aFrameData[index].v[1]);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameVectorZ, pAnimData->aFrameData[index].v[2]);
		actionDescriptorProcs->PutBoolean(animframeDescriptor, keyAnimFrameUseQuat, pAnimData->aFrameData[index].usequat);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameQuatW, pAnimData->aFrameData[index].q.w);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameQuatX, pAnimData->aFrameData[index].q.x);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameQuatY, pAnimData->aFrameData[index].q.y);
		actionDescriptorProcs->PutFloat(animframeDescriptor, keyAnimFrameQuatZ, pAnimData->aFrameData[index].q.z);

		actionListProcs->PutObject(animframeActionList,keyAnimFrameClass,animframeDescriptor);
	}
	actionDescriptorProcs->PutList(*pReturnDescriptor,keyAnimFrameList,animframeActionList);		


	return 1;
}

short PI3DParseSceneIntoDescriptor(PSActionDescriptorProcs *actionDescriptorProcs,PSActionListProcs *actionListProcs,PI3DScene	*scene,PIDescriptorHandle *sceneDescriptor)
{
	
	if(!sceneDescriptor || !scene)
		return FALSE;	
	
	int32		polyCount=0;
	PI3DList		*theList=NULL;
	long		index,index2,i,j;

	//Traverse the scene and build the descriptor

	PIActionDescriptor actionDescriptor=NULL;
	actionDescriptorProcs->Make(&actionDescriptor);
	if(!actionDescriptor)
		return FALSE;

	int32 tempInt;

	tempInt=1;
	OSErr err = actionDescriptorProcs->PutInteger(actionDescriptor, keyVersionMajor, tempInt);
	tempInt=0;
	err = actionDescriptorProcs->PutInteger(actionDescriptor, keyVersionMinor, tempInt);

	//globals
	actionDescriptorProcs->PutFloat(actionDescriptor,keyGlobalAmbientRed,scene->globalAmbientColor.red);
	actionDescriptorProcs->PutFloat(actionDescriptor,keyGlobalAmbientGreen,scene->globalAmbientColor.green);
	actionDescriptorProcs->PutFloat(actionDescriptor,keyGlobalAmbientBlue,scene->globalAmbientColor.blue);

	actionDescriptorProcs->PutFloat(actionDescriptor,keyGlobalUnits,scene->globalUnits);
	actionDescriptorProcs->PutInteger(actionDescriptor,keyGlobalAnimStart,scene->globalAnimationStartTime);
	actionDescriptorProcs->PutInteger(actionDescriptor,keyGlobalFrames,scene->globalNumFrames);
	actionDescriptorProcs->PutFloat(actionDescriptor,keyGlobalFPS,scene->globalFPS);
	actionDescriptorProcs->PutString(actionDescriptor,keyGlobalRangeName,scene->globalRange);

	//Dump the spot lights	
	PIActionList lightActionList;
	actionListProcs->Make(&lightActionList);
	theList=(PI3DList*)scene->lightList;
	while(theList)
		{
		PI3DLight *lightList = (PI3DLight*)theList;
		PIActionDescriptor lightDescriptor;
		actionDescriptorProcs->Make(&lightDescriptor);
		actionDescriptorProcs->PutString(lightDescriptor,keyName,lightList->name);

		//position
		actionDescriptorProcs->PutFloat(lightDescriptor,keyX,lightList->pos[0]);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyY,lightList->pos[1]);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyZ,lightList->pos[2]);

		//target
		actionDescriptorProcs->PutFloat(lightDescriptor,keyTargetX,lightList->target[0]);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyTargetY,lightList->target[1]);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyTargetZ,lightList->target[2]);

		//color
		actionDescriptorProcs->PutFloat(lightDescriptor,keyRed,lightList->col.red);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyGreen,lightList->col.green);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyBlue,lightList->col.blue);

		//hotspot
		actionDescriptorProcs->PutFloat(lightDescriptor,keyHotspot,lightList->hotspot);

		//falloff
		actionDescriptorProcs->PutFloat(lightDescriptor,keyFalloff,lightList->falloff);

		//shadowFlag
		actionDescriptorProcs->PutInteger(lightDescriptor,keyShadow,lightList->shadowFlag);

		//attenuation
		actionDescriptorProcs->PutBoolean(lightDescriptor,keyAttenuation,lightList->attenuation);

		//attenuation type
		actionDescriptorProcs->PutInteger(lightDescriptor,keyAttenType, lightList->attenuationType);

		//attenuation ABC
		actionDescriptorProcs->PutFloat(lightDescriptor,keyAttenA,lightList->attenuationAbc.a);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyAttenB,lightList->attenuationAbc.b);
		actionDescriptorProcs->PutFloat(lightDescriptor,keyAttenC,lightList->attenuationAbc.c);

		//outer_radius
		actionDescriptorProcs->PutFloat(lightDescriptor,keyOuterRadius,lightList->outerRadius);

		//inner_radius
		actionDescriptorProcs->PutFloat(lightDescriptor,keyInnerRadius,lightList->innerRadius);

		//multiple
		actionDescriptorProcs->PutFloat(lightDescriptor,keyMultiple,lightList->multiple);

		//type
		actionDescriptorProcs->PutInteger(lightDescriptor,keyType, lightList->type);
		
		actionListProcs->PutObject(lightActionList,keyLightClass,lightDescriptor);
		theList=(PI3DList*)theList->next;
		}
	actionDescriptorProcs->PutList(actionDescriptor,keyLightList,lightActionList);

	//Dump the cameras
	PIActionList cameraActionList;
	actionListProcs->Make(&cameraActionList);
	theList=(PI3DList*)scene->cameraList;
	while(theList)
		{	
		PI3DCamera    *CameraList=(PI3DCamera*)theList;
		PIActionDescriptor cameraDescriptor;
		actionDescriptorProcs->Make(&cameraDescriptor);
		actionDescriptorProcs->PutString(cameraDescriptor,keyName,CameraList->name);

		//position
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyX,CameraList->pos[0]);
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyY,CameraList->pos[1]);
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyZ,CameraList->pos[2]);

		//target
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyTargetX,CameraList->target[0]);
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyTargetY,CameraList->target[1]);
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyTargetZ,CameraList->target[2]);

		//bank
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyBank,CameraList->bank);

		//lens
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyLens,CameraList->lens);

		//matrix
		/*PIActionList matrixList;
		actionListProcs->Make(&matrixList);
		for( i=0;i<4;i++)
			for( j=0;j<4;j++)
				actionListProcs->PutFloat(matrixList,CameraList->matrix[i][j]);	
		actionDescriptorProcs->PutList(cameraDescriptor,keyMatrix,matrixList);

		//matrixPresent
		actionDescriptorProcs->PutBoolean(cameraDescriptor,keyHasMatrix,CameraList->matrixPresent);*/

		//ortho
		actionDescriptorProcs->PutBoolean(cameraDescriptor,keyOrtho,CameraList->ortho);

		//aspect ratio
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyApsect,CameraList->aspectratio);

		//zoom factor
		actionDescriptorProcs->PutFloat(cameraDescriptor,keyZoomFactor,CameraList->zoomfactor);


		actionListProcs->PutObject(cameraActionList,keyCameraClass,cameraDescriptor);
		theList=(PI3DList*)theList->next;
		}
	actionDescriptorProcs->PutList(actionDescriptor,keyCameraList,cameraActionList);

	//Dump the meshes
	PIActionList meshActionList;
	actionListProcs->Make(&meshActionList);
	theList=(PI3DList*)scene->meshList;
	while(theList)
		{	
		
		PI3DMesh    *MeshList=(PI3DMesh*)theList;
		PIActionDescriptor meshDescriptor;
		actionDescriptorProcs->Make(&meshDescriptor);
		actionDescriptorProcs->PutString(meshDescriptor,keyName,MeshList->name);
		
		//Vertices
		//Endianness on ppc...sorry Mac people, this is gonna hurt
#ifdef __ppc__
		for(index=0;index<MeshList->vertices;index++)
			{
			PI3DByteSwapDouble(&MeshList->vertex[index][0]);
			PI3DByteSwapDouble(&MeshList->vertex[index][1]);
			PI3DByteSwapDouble(&MeshList->vertex[index][2]);
			}
#endif
		if(MeshList->vertices)
			actionDescriptorProcs->PutData(meshDescriptor,keyVertexList,MeshList->vertices*sizeof(PI3DVector),MeshList->vertex);
#ifdef __ppc__
		for(index=0;index<MeshList->vertices;index++)
			{
			PI3DByteSwapDouble(&MeshList->vertex[index][0]);
			PI3DByteSwapDouble(&MeshList->vertex[index][1]);
			PI3DByteSwapDouble(&MeshList->vertex[index][2]);
			}
#endif
		
		//Normals
#ifdef __ppc__
		for(index=0;index<MeshList->normals;index++)
			{
			PI3DByteSwapDouble(&MeshList->normal[index][0]);
			PI3DByteSwapDouble(&MeshList->normal[index][1]);
			PI3DByteSwapDouble(&MeshList->normal[index][2]);
			}
#endif
		if(MeshList->normals)
			actionDescriptorProcs->PutData(meshDescriptor,keyNormalList,MeshList->normals*sizeof(PI3DVector),MeshList->normal);
#ifdef __ppc__
		for(index=0;index<MeshList->normals;index++)
			{
			PI3DByteSwapDouble(&MeshList->normal[index][0]);
			PI3DByteSwapDouble(&MeshList->normal[index][1]);
			PI3DByteSwapDouble(&MeshList->normal[index][2]);
			}
#endif

		//lines, linecolors
#ifdef __ppc__
		for(index=0;index<MeshList->lines;index++)
		{
			PI3DByteSwapDouble(&MeshList->linecolors[index][0]);
			PI3DByteSwapDouble(&MeshList->linecolors[index][1]);
			PI3DByteSwapDouble(&MeshList->linecolors[index][2]);
			PI3DByteSwapDouble(&MeshList->line[index][0]);
			PI3DByteSwapDouble(&MeshList->line[index][1]);
			PI3DByteSwapDouble(&MeshList->line[index][2]);
		}
#endif
		if(MeshList->lines)
			actionDescriptorProcs->PutData(meshDescriptor,keylineColorList,MeshList->lines*sizeof(PI3DVector),MeshList->linecolors);

		if(MeshList->lines)
			actionDescriptorProcs->PutData(meshDescriptor,keyLineList,MeshList->lines*sizeof(PI3DVector),MeshList->line);

#ifdef __ppc__
		for(index=0;index<MeshList->lines;index++)
		{
			PI3DByteSwapDouble(&MeshList->linecolors[index][0]);
			PI3DByteSwapDouble(&MeshList->linecolors[index][1]);
			PI3DByteSwapDouble(&MeshList->linecolors[index][2]);
			PI3DByteSwapDouble(&MeshList->line[index][0]);
			PI3DByteSwapDouble(&MeshList->line[index][1]);
			PI3DByteSwapDouble(&MeshList->line[index][2]);
		}
#endif

		//linematerials
#ifdef __ppc__
		for(index=0;index<MeshList->lines;index++)
		{
			SWAP32_IF_BIGENDIAN(MeshList->linematerial[index]);
		}
#endif
		if(MeshList->lines)
			actionDescriptorProcs->PutData(meshDescriptor,keylineMaterialList,MeshList->lines*sizeof(int32),MeshList->linematerial);

#ifdef __ppc__
		for(index=0;index<MeshList->lines;index++)
		{
			SWAP32_IF_BIGENDIAN(MeshList->linematerial[index]);
		}
#endif
		//Colors
#ifdef __ppc__
		for(index=0;index<MeshList->colors;index++)
		{
			PI3DByteSwapDouble(&MeshList->color[index][0]);
			PI3DByteSwapDouble(&MeshList->color[index][1]);
			PI3DByteSwapDouble(&MeshList->color[index][2]);
		}
#endif
		if(MeshList->colors)
			actionDescriptorProcs->PutData(meshDescriptor,keyVertexColorList,MeshList->colors*sizeof(PI3DVector),MeshList->color);
#ifdef __ppc__
		for(index=0;index<MeshList->colors;index++)
		{
			PI3DByteSwapDouble(&MeshList->color[index][0]);
			PI3DByteSwapDouble(&MeshList->color[index][1]);
			PI3DByteSwapDouble(&MeshList->color[index][2]);
		}
#endif
		//UVs
#ifdef __ppc__
		for(index=0;index<MeshList->textures;index++)
			{
			PI3DByteSwapDouble(&MeshList->texture[index][0]);
			PI3DByteSwapDouble(&MeshList->texture[index][1]);
			}
#endif
		if(MeshList->textures)
			actionDescriptorProcs->PutData(meshDescriptor,keyUVList,MeshList->textures*sizeof(PI3DPoint),MeshList->texture);

#ifdef __ppc__
		for(index=0;index<MeshList->textures;index++)
			{
			PI3DByteSwapDouble(&MeshList->texture[index][0]);
			PI3DByteSwapDouble(&MeshList->texture[index][1]);
			}
#endif
					
		//faces
		uint32 numPoints=0;
		uint32 currentPoint=0;
		polyCount += MeshList->faces;
		for(index=0;index<MeshList->faces;index++)
			{
			numPoints+=MeshList->face[index].numPoints;
#ifdef __ppc__
			SWAP32_IF_BIGENDIAN(MeshList->face[index].flags);
			SWAP32_IF_BIGENDIAN(MeshList->face[index].smoothing);
			SWAP32_IF_BIGENDIAN(MeshList->face[index].numPoints);
			SWAP32_IF_BIGENDIAN(MeshList->face[index].material);
#endif		
			}
		
		if(MeshList->faces)
			actionDescriptorProcs->PutData(meshDescriptor,keyFaceList,MeshList->faces*sizeof(PI3DFace),MeshList->face);
		uint32 *faceDataBlob=NULL;
		
		if(numPoints)
			{
			faceDataBlob=(uint32*)PI3DMemoryAlloc (numPoints * sizeof (int32) * 4);
			if(!faceDataBlob)
				return FALSE;
			}
			
		for(index=0;index<MeshList->faces;index++)
			{
#ifdef __ppc__
			SWAP32_IF_BIGENDIAN(MeshList->face[index].flags);
			SWAP32_IF_BIGENDIAN(MeshList->face[index].smoothing);
			SWAP32_IF_BIGENDIAN(MeshList->face[index].numPoints);
			SWAP32_IF_BIGENDIAN(MeshList->face[index].material);
			
#endif		
			for(index2=0;index2<MeshList->face[index].numPoints;index2++)
				{
				faceDataBlob[currentPoint]=MeshList->face[index].points[index2];
#ifdef __ppc__
				SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
#endif			
				currentPoint++;
				faceDataBlob[currentPoint]=MeshList->face[index].normals[index2];
#ifdef __ppc__
				SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
#endif			
				currentPoint++;
				faceDataBlob[currentPoint]=MeshList->face[index].textures[index2];
#ifdef __ppc__
				SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
#endif			
				currentPoint++;
				faceDataBlob[currentPoint]=MeshList->face[index].colors[index2];
#ifdef __ppc__
				SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
#endif			
				currentPoint++;
				}
			}
		
		if(numPoints)
			actionDescriptorProcs->PutData(meshDescriptor,keyFaceIndexes,numPoints * sizeof (int32) * 4,faceDataBlob);
		
		if(faceDataBlob)
			PI3DMemoryFree (faceDataBlob);
	
		//matrix
		PIActionList matrixList;
		actionListProcs->Make(&matrixList);
		for( i=0;i<4;i++)
			for( j=0;j<4;j++)
				actionListProcs->PutFloat(matrixList,MeshList->matrix[i][j]);	
		actionDescriptorProcs->PutList(meshDescriptor,keyMatrix,matrixList);

		//flags
		actionDescriptorProcs->PutInteger(meshDescriptor,keyFlags,MeshList->flags);

		//hidden
		actionDescriptorProcs->PutInteger(meshDescriptor,keyHidden,MeshList->hidden);

		//shadow
		actionDescriptorProcs->PutInteger(meshDescriptor,keyShadow,MeshList->shadow);

		//matrixPresent
		actionDescriptorProcs->PutBoolean(meshDescriptor,keyHasMatrix,MeshList->matrixPresent);

		//smoothingGroupPresent
		actionDescriptorProcs->PutBoolean(meshDescriptor,keyHasSmoothing,MeshList->smoothingGroupPresent);


		actionListProcs->PutObject(meshActionList,keyMeshClass,meshDescriptor);
		theList=(PI3DList*)theList->next;
		}
	actionDescriptorProcs->PutList(actionDescriptor,keyMeshList,meshActionList);

	//Dump the material properties
	PIActionList materialActionList;
	actionListProcs->Make(&materialActionList);
	theList=(PI3DList*)scene->matPropList;
	while(theList)
	{	
		PI3DMaterial    *MatPropList=(PI3DMaterial*)theList;
		PIActionDescriptor materialDescriptor;
		actionDescriptorProcs->Make(&materialDescriptor);
		actionDescriptorProcs->PutString(materialDescriptor,keyName,MatPropList->name);

		//ambient
		actionDescriptorProcs->PutFloat(materialDescriptor,keyAmbientRed, MatPropList->ambient.red);
		actionDescriptorProcs->PutFloat(materialDescriptor,keyAmbientGreen, MatPropList->ambient.green);
		actionDescriptorProcs->PutFloat(materialDescriptor,keyAmbientBlue, MatPropList->ambient.blue);

		//diffuse
		actionDescriptorProcs->PutFloat(materialDescriptor,keyDiffuseRed, MatPropList->diffuse.red);
		actionDescriptorProcs->PutFloat(materialDescriptor,keyDiffuseGreen, MatPropList->diffuse.green);
		actionDescriptorProcs->PutFloat(materialDescriptor,keyDiffuseBlue, MatPropList->diffuse.blue);

		//specular
		actionDescriptorProcs->PutFloat(materialDescriptor,keySpecularRed, MatPropList->specular.red);
		actionDescriptorProcs->PutFloat(materialDescriptor,keySpecularGreen, MatPropList->specular.green);
		actionDescriptorProcs->PutFloat(materialDescriptor,keySpecularBlue, MatPropList->specular.blue);

		//emissive
		actionDescriptorProcs->PutFloat(materialDescriptor,keyEmissiveRed, MatPropList->emissive.red);
		actionDescriptorProcs->PutFloat(materialDescriptor,keyEmissiveGreen, MatPropList->emissive.green);
		actionDescriptorProcs->PutFloat(materialDescriptor,keyEmissiveBlue, MatPropList->emissive.blue);

		//shininess
		actionDescriptorProcs->PutFloat(materialDescriptor,keyShininess, MatPropList->shininess);

		//glossiness
		actionDescriptorProcs->PutFloat(materialDescriptor,keyShininess2, MatPropList->glossiness);

		//transparency
		actionDescriptorProcs->PutFloat(materialDescriptor,keyTransparency, MatPropList->transparency);

		//reflection
		actionDescriptorProcs->PutFloat(materialDescriptor,keyReflection, MatPropList->reflection);

		//selfIllumination
		actionDescriptorProcs->PutInteger(materialDescriptor,keySelfIllumination, MatPropList->selfIllumination);

		//shading
		actionDescriptorProcs->PutInteger(materialDescriptor,keyShading, MatPropList->shading);

		//twoSided
		actionDescriptorProcs->PutBoolean(materialDescriptor,keyTwoSide, MatPropList->twoSided);

		//wireframe
		actionDescriptorProcs->PutBoolean(materialDescriptor,keyWireframe, MatPropList->wireframe);

		//decal
		actionDescriptorProcs->PutBoolean(materialDescriptor,keyDecal, MatPropList->decal);

		//wireframesize
		actionDescriptorProcs->PutFloat(materialDescriptor,keyWireframeSize, MatPropList->wireframesize);

		//maps
		PIActionList mapActionList;
		actionListProcs->Make(&mapActionList);
		for(index=0;index<kNumSupportedTextureTypes;index++)
		{
			PIActionDescriptor mapDescriptor;
			actionDescriptorProcs->Make(&mapDescriptor);

			//map type
			actionDescriptorProcs->PutInteger(mapDescriptor, keyType, index);

			//map name
			actionDescriptorProcs->PutString(mapDescriptor, keyName, MatPropList->maps[index].map);

			//map strength
			actionDescriptorProcs->PutFloat(mapDescriptor, keyStrength, MatPropList->maps[index].strength);

			//map uscale
			actionDescriptorProcs->PutFloat(mapDescriptor, keyUScale, MatPropList->maps[index].uscale);

			//map vscale
			actionDescriptorProcs->PutFloat(mapDescriptor, keyVScale, MatPropList->maps[index].vscale);

			//map uoffset
			actionDescriptorProcs->PutFloat(mapDescriptor, keyUOffset, MatPropList->maps[index].uoffset);

			//map voffset
			actionDescriptorProcs->PutFloat(mapDescriptor, keyVOffset, MatPropList->maps[index].voffset);

			//map angle
			actionDescriptorProcs->PutFloat(mapDescriptor, keyAngle, MatPropList->maps[index].angle);

			//map flags
			actionDescriptorProcs->PutInteger(mapDescriptor, keyFlags, MatPropList->maps[index].flags);

			// map 3ds style
			actionDescriptorProcs->PutBoolean(mapDescriptor, key3DSStyle, MatPropList->maps[index].mapIs3DSStyle);


			actionListProcs->PutObject(mapActionList,keyMapClass,mapDescriptor);
		}
		actionDescriptorProcs->PutList(materialDescriptor,keyMapList,mapActionList);		

		actionListProcs->PutObject(materialActionList,keyMaterialClass,materialDescriptor);
		theList=(PI3DList*)theList->next;
	}
	actionDescriptorProcs->PutList(actionDescriptor,keyMaterialList,materialActionList);

	//Dump the key frames
	PIActionList keyframeActionList;
	actionListProcs->Make(&keyframeActionList);
	theList=(PI3DList*)scene->keyFrameList;
	while(theList)
	{	
		PI3DKeyFrame    *KeyFrameList=(PI3DKeyFrame*)theList;
		PIActionDescriptor keyframeDescriptor;
		actionDescriptorProcs->Make(&keyframeDescriptor);

		//keyframe props
		actionDescriptorProcs->PutString(keyframeDescriptor,keyName,KeyFrameList->name);
		actionDescriptorProcs->PutString(keyframeDescriptor,keyInstanceName,KeyFrameList->instancename);
		actionDescriptorProcs->PutInteger(keyframeDescriptor,keyFlags1, KeyFrameList->flags1);
		actionDescriptorProcs->PutInteger(keyframeDescriptor,keyFlags2, KeyFrameList->flags2);
		actionDescriptorProcs->PutInteger(keyframeDescriptor,keyU3DNodeID, KeyFrameList->nodeID);
		actionDescriptorProcs->PutInteger(keyframeDescriptor,keyU3DParentID, KeyFrameList->parentNodeID);
		actionDescriptorProcs->PutInteger(keyframeDescriptor,keyU3DType, KeyFrameList->nodeType);
		actionDescriptorProcs->PutInteger(keyframeDescriptor,keyFlags2, KeyFrameList->flags2);

		//pivot pt
		PIActionList PivotActionList;
		actionListProcs->Make(&PivotActionList);
		PIActionDescriptor pivotDescriptor;
		actionDescriptorProcs->Make(&pivotDescriptor);
		actionDescriptorProcs->PutFloat(pivotDescriptor,keyPivotX, KeyFrameList->pivot[0]);
		actionDescriptorProcs->PutFloat(pivotDescriptor,keyPivotY, KeyFrameList->pivot[1]);
		actionDescriptorProcs->PutFloat(pivotDescriptor,keyPivotZ, KeyFrameList->pivot[2]);
		actionListProcs->PutObject(PivotActionList,keyPivotObject,pivotDescriptor);
		actionDescriptorProcs->PutList(keyframeDescriptor,keyPivotList,PivotActionList);

		//matrix
		PIActionList matrixList;
		actionListProcs->Make(&matrixList);
		for( i=0;i<4;i++)
			for( j=0;j<4;j++)
				actionListProcs->PutFloat(matrixList,KeyFrameList->localmatrix[i][j]);	
		actionDescriptorProcs->PutList(keyframeDescriptor,keyMatrix,matrixList);

		//pivot present
		actionDescriptorProcs->PutBoolean(keyframeDescriptor,keyHasPivot,KeyFrameList->usepivot);

		//matrix present
		actionDescriptorProcs->PutBoolean(keyframeDescriptor,keyHasLocalMatrix,KeyFrameList->matrixPresent);

		//animation range
		actionDescriptorProcs->PutBoolean(keyframeDescriptor,keyHasRange,KeyFrameList->userange);

		//position data
		int32 bDidit = 0;
		if (KeyFrameList->positionData)
		{
			PIActionList PositionActionList;
			actionListProcs->Make(&PositionActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->positionData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor PositionDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &PositionDescriptor);
				actionListProcs->PutObject(PositionActionList,keyframePositionActionList,PositionDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframePositionList, PositionActionList);		
		}
		//scale data
		if (KeyFrameList->scaleData)
		{
			PIActionList ScaleActionList;
			actionListProcs->Make(&ScaleActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->scaleData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor ScaleDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &ScaleDescriptor);
				actionListProcs->PutObject(ScaleActionList,keyframeScaleActionList,ScaleDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeScaleList, ScaleActionList);		
		}
		//rotation data
		if (KeyFrameList->rotationData)		
		{
			PIActionList RotationActionList;
			actionListProcs->Make(&RotationActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->rotationData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor RotationDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &RotationDescriptor);
				actionListProcs->PutObject(RotationActionList,keyframeRotationActionList,RotationDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeRotationList, RotationActionList);
		}
		//FOV data
		if (KeyFrameList->fOVData)		
		{
			PIActionList FOVActionList;
			actionListProcs->Make(&FOVActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->fOVData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor FOVDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &FOVDescriptor);
				actionListProcs->PutObject(FOVActionList,keyframeFOVActionList,FOVDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeFOVList, FOVActionList);		
		}
		//Roll data
		if (KeyFrameList->rollData)
		{
			PIActionList RollActionList;
			actionListProcs->Make(&RollActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->rollData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor RollDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &RollDescriptor);
				actionListProcs->PutObject(RollActionList,keyframeRollActionList,RollDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeRollList, RollActionList);					
		}
		//Col data
		if (KeyFrameList->colData)
		{
			PIActionList ColorActionList;
			actionListProcs->Make(&ColorActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->colData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor ColorDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &ColorDescriptor);
				actionListProcs->PutObject(ColorActionList,keyframeColorActionList,ColorDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeColorList, ColorActionList);					
		}
		//Hot data
		if (KeyFrameList->hotData)
		{
			PIActionList hotActionList;
			actionListProcs->Make(&hotActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->hotData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor HotDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &HotDescriptor);
				actionListProcs->PutObject(hotActionList,keyframeHotActionList,HotDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeHotList, hotActionList);		
		}
		//Falloff data
		if (KeyFrameList->fallData)
		{
			PIActionList FallActionList;
			actionListProcs->Make(&FallActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->fallData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor FallDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &FallDescriptor);
				actionListProcs->PutObject(FallActionList,keyframeFalloffActionList,FallDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeFalloffList, FallActionList);		
		}
		//Hide data
		if (KeyFrameList->hideData)
		{
			PIActionList HideActionList;
			actionListProcs->Make(&HideActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->hideData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor HideDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &HideDescriptor);
				actionListProcs->PutObject(HideActionList,keyframeHideActionList,HideDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeHideList, HideActionList);		
		}
		//Morph data
		if (KeyFrameList->morphData)
		{
			PIActionList MorphActionList;
			actionListProcs->Make(&MorphActionList);
			PI3DList *theAList=(PI3DList*)KeyFrameList->morphData;
			while(theAList)
			{	
				PI3DAnimationData  *AnimationList=(PI3DAnimationData*)theAList;
				PIActionDescriptor MorphDescriptor;
				bDidit = PI3DParseAnimDataIntoDescriptor(actionDescriptorProcs, actionListProcs, AnimationList, &MorphDescriptor);
				actionListProcs->PutObject(MorphActionList,keyframeMorphActionList,MorphDescriptor);
				theAList=(PI3DList*)theAList->next;
			}
			actionDescriptorProcs->PutList(keyframeDescriptor, keyframeMorphList, MorphActionList);		
		}

		actionListProcs->PutObject(keyframeActionList,keyKeyFrameClass,keyframeDescriptor);
		theList=(PI3DList*)theList->next;
	}
	actionDescriptorProcs->PutList(actionDescriptor,keyKeyFrameList,keyframeActionList);
	actionDescriptorProcs->PutInteger(actionDescriptor,keyPolyCount,polyCount);
	
	*sceneDescriptor = NULL;
	actionDescriptorProcs->AsHandle(actionDescriptor, sceneDescriptor);
	actionDescriptorProcs->Free(actionDescriptor);
	
	if(!*sceneDescriptor)
		return false;

	return TRUE;

}

short PI3DParseAnimDataIntoScene(PSActionDescriptorProcs *actionDescriptorProcs,PSActionListProcs *actionListProcs, PI3DAnimationData* pAnimData, PIActionDescriptor* pDescriptor)
	{

	// name
	actionDescriptorProcs->GetString(*pDescriptor,keyNameAnim,pAnimData->name,80);
	//flags
	actionDescriptorProcs->GetInteger(*pDescriptor, keyFlags, &pAnimData->flags);
	//keys
	actionDescriptorProcs->GetInteger(*pDescriptor, keyKeys, &pAnimData->keys);

	//animation frame data
	pAnimData->aFrameData = (PI3DFrameVector*)PI3DMemoryAlloc (pAnimData->keys * sizeof (PI3DFrameVector));
	memset(pAnimData->aFrameData,0,sizeof(*pAnimData->aFrameData));

	Boolean hasKey = false;
	actionDescriptorProcs->HasKey(*pDescriptor, keyAnimFrameList, &hasKey);
	if(hasKey)
		{
		DescriptorClassID tempType;
		PIActionList animframeActionList;
		actionDescriptorProcs->GetList(*pDescriptor,keyAnimFrameList,&animframeActionList);
		if(animframeActionList)
			{
			for(int32 index=0;index<pAnimData->keys;index++)
				{
				PIActionDescriptor animframeDescriptor;
				tempType = keyAnimFrameClass;
				actionListProcs->GetObject(animframeActionList,index,&tempType,&animframeDescriptor);
				if(animframeDescriptor)
					{
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameNumber, &pAnimData->aFrameData[index].framenumber);
					actionDescriptorProcs->GetInteger(animframeDescriptor, keyAnimFrameFlags, &pAnimData->aFrameData[index].flags);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameTension, &pAnimData->aFrameData[index].tension);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameContinuity, &pAnimData->aFrameData[index].continuity);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameBias, &pAnimData->aFrameData[index].bias);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameEaseTo, &pAnimData->aFrameData[index].easeto);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameEaseFrom, &pAnimData->aFrameData[index].easefrom);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameRotation, &pAnimData->aFrameData[index].rotation);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameValue, &pAnimData->aFrameData[index].value);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameVectorX, &pAnimData->aFrameData[index].v[0]);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameVectorY, &pAnimData->aFrameData[index].v[1]);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameVectorZ, &pAnimData->aFrameData[index].v[2]);
					actionDescriptorProcs->GetBoolean(animframeDescriptor, keyAnimFrameUseQuat, &pAnimData->aFrameData[index].usequat);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameQuatW, &pAnimData->aFrameData[index].q.w);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameQuatX, &pAnimData->aFrameData[index].q.x);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameQuatY, &pAnimData->aFrameData[index].q.y);
					actionDescriptorProcs->GetFloat(animframeDescriptor, keyAnimFrameQuatZ, &pAnimData->aFrameData[index].q.z);
					actionDescriptorProcs->Free(animframeDescriptor);
					}
				}
			actionListProcs->Free(animframeActionList);
			}
		}
	

	return 1;
	}

short PI3DParseDescriptorIntoScene(PSActionDescriptorProcs *actionDescriptorProcs,PSActionListProcs *actionListProcs,PIDescriptorHandle sceneDescriptor,PI3DScene **inScene)
{

	Boolean hasKey = false;
	uint32 listSize=0;
	uint32 index,i,j;
	int32 index2,index3;
	int32 tempInt;
	DescriptorClassID tempType;
	PI3DScene *scene = (PI3DScene*)PI3DMemoryAlloc(sizeof(PI3DScene));
	if(!scene)
		return 0;
		
	memset(scene,0,sizeof(PI3DScene));

	PIActionDescriptor actionDescriptor = NULL;

	actionDescriptorProcs->HandleToDescriptor(sceneDescriptor,&actionDescriptor);

	//globals
	actionDescriptorProcs->GetFloat(actionDescriptor,keyGlobalAmbientRed,&scene->globalAmbientColor.red);
	actionDescriptorProcs->GetFloat(actionDescriptor,keyGlobalAmbientGreen,&scene->globalAmbientColor.green);
	actionDescriptorProcs->GetFloat(actionDescriptor,keyGlobalAmbientBlue,&scene->globalAmbientColor.blue);

	actionDescriptorProcs->GetFloat(actionDescriptor,keyGlobalUnits,&scene->globalUnits);
	actionDescriptorProcs->GetInteger(actionDescriptor,keyGlobalAnimStart,&scene->globalAnimationStartTime);
	actionDescriptorProcs->GetInteger(actionDescriptor,keyGlobalFrames,&scene->globalNumFrames);
	actionDescriptorProcs->GetFloat(actionDescriptor,keyGlobalFPS,&scene->globalFPS);
	actionDescriptorProcs->GetString(actionDescriptor,keyGlobalRangeName,scene->globalRange,40);

	actionDescriptorProcs->HasKey(actionDescriptor, keyLightList, &hasKey);
	if(hasKey)
	{
		//Get the lights	
		PIActionList lightActionList;
		actionDescriptorProcs->GetList(actionDescriptor,keyLightList,&lightActionList);
		if(lightActionList)
		{
			actionListProcs->GetCount(lightActionList,&listSize);
			for(index=0;index<listSize;index++)
			{
				PI3DLight *Light = (PI3DLight*)PI3DMemoryAlloc (sizeof (PI3DLight));
				if(!Light)
					return 0;
					
				PIActionDescriptor lightDescriptor;
				tempType = keyLightClass;
				actionListProcs->GetObject(lightActionList,index,&tempType,&lightDescriptor);
				if(lightDescriptor)
				{
					//name
					actionDescriptorProcs->GetString(lightDescriptor,keyName,Light->name,40);

					//position
					actionDescriptorProcs->GetFloat(lightDescriptor,keyX,&Light->pos[0]);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyY,&Light->pos[1]);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyZ,&Light->pos[2]);

					//target
					actionDescriptorProcs->GetFloat(lightDescriptor,keyTargetX,&Light->target[0]);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyTargetY,&Light->target[1]);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyTargetZ,&Light->target[2]);

					//color
					actionDescriptorProcs->GetFloat(lightDescriptor,keyRed,&Light->col.red);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyGreen,&Light->col.green);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyBlue,&Light->col.blue);

					//hotspot
					actionDescriptorProcs->GetFloat(lightDescriptor,keyHotspot,&Light->hotspot);

					//falloff
					actionDescriptorProcs->GetFloat(lightDescriptor,keyFalloff,&Light->falloff);

					//shadowFlag
					actionDescriptorProcs->GetInteger(lightDescriptor,keyShadow,&Light->shadowFlag);

					//attenuation
					Boolean bAtt;
					actionDescriptorProcs->GetBoolean(lightDescriptor,keyAttenuation,&bAtt);
					Light->attenuation = (bool)bAtt;

					//attenuation type
					int32 e;
					actionDescriptorProcs->GetInteger(lightDescriptor,keyAttenType, &e);
					Light->attenuationType = (PI3DAttenuationEnum)e;

					//attenuation ABC
					actionDescriptorProcs->GetFloat(lightDescriptor,keyAttenA,&Light->attenuationAbc.a);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyAttenB,&Light->attenuationAbc.b);
					actionDescriptorProcs->GetFloat(lightDescriptor,keyAttenC,&Light->attenuationAbc.c);

					//outer_radius
					actionDescriptorProcs->GetFloat(lightDescriptor,keyOuterRadius,&Light->outerRadius);

					//inner_radius
					actionDescriptorProcs->GetFloat(lightDescriptor,keyInnerRadius,&Light->innerRadius);

					//multiple
					actionDescriptorProcs->GetFloat(lightDescriptor,keyMultiple,&Light->multiple);

					//shading
					actionDescriptorProcs->GetInteger(lightDescriptor,keyType, &tempInt);
					Light->type = (PI3DLightEnum)tempInt;

					actionDescriptorProcs->Free(lightDescriptor);
					
					PI3DListAdd((PI3DList **)&scene->lightList, reinterpret_cast<PI3DList *>(Light));
				}
			}
			actionListProcs->Free(lightActionList);
		}
	}

	actionDescriptorProcs->HasKey(actionDescriptor, keyCameraList, &hasKey);
	if(hasKey)
	{
		//Get the cameras
		PIActionList cameraActionList;
		actionDescriptorProcs->GetList(actionDescriptor,keyCameraList,&cameraActionList);
		if(cameraActionList)
			{
			actionListProcs->GetCount(cameraActionList,&listSize);
			for(index=0;index<listSize;index++)
			{
				PI3DCamera *Camera = (PI3DCamera*)PI3DMemoryAlloc (sizeof (PI3DCamera));
				if(!Camera)
					return 0;
					
				PIActionDescriptor cameraDescriptor;
				tempType = keyCameraClass;
				actionListProcs->GetObject(cameraActionList,index,&tempType,&cameraDescriptor);
				if(cameraDescriptor)
				{
					//name
					actionDescriptorProcs->GetString(cameraDescriptor,keyName,Camera->name, 40);

					//position
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyX,&Camera->pos[0]);
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyY,&Camera->pos[1]);
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyZ,&Camera->pos[2]);

					//target
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyTargetX,&Camera->target[0]);
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyTargetY,&Camera->target[1]);
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyTargetZ,&Camera->target[2]);

					//bank
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyBank,&Camera->bank);

					//lens
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyLens,&Camera->lens);

					//matrix
					/*actionDescriptorProcs->HasKey(cameraDescriptor, keyMatrix, &hasKey);
					if (hasKey)
					{
						PIActionList matrixList;
						actionDescriptorProcs->GetList(cameraDescriptor,keyMatrix,&matrixList);
						for( i=0;i<4;i++)
							for( j=0;j<4;j++)
								actionListProcs->GetFloat(matrixList,j+i*4,&Camera->matrix[i][j]);
					}

					//matrixPresent
					Boolean bPresent;
					actionDescriptorProcs->GetBoolean(cameraDescriptor,keyHasMatrix,&bPresent);
					Camera->matrixPresent = (bool)bPresent;*/

					//ortho
					Boolean bOrtho;
					actionDescriptorProcs->GetBoolean(cameraDescriptor,keyOrtho,&bOrtho);
					Camera->ortho = (bool)bOrtho;

					//aspect ratio
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyApsect,&Camera->aspectratio);

					//zoom factor
					actionDescriptorProcs->GetFloat(cameraDescriptor,keyZoomFactor,&Camera->zoomfactor);

					actionDescriptorProcs->Free(cameraDescriptor);
					
					PI3DListAdd((PI3DList **)&scene->cameraList, reinterpret_cast<PI3DList *>(Camera));
				}
			}
			actionListProcs->Free(cameraActionList);
		}
	}


	actionDescriptorProcs->HasKey(actionDescriptor, keyMeshList, &hasKey);
	if(hasKey)
	{
		//Get the meshes
		PIActionList meshActionList;
		actionDescriptorProcs->GetList(actionDescriptor,keyMeshList,&meshActionList);
		if(meshActionList)
		{
			actionListProcs->GetCount(meshActionList,&listSize);
			for(index=0;index<listSize;index++)
			{
				PI3DMesh *Mesh = (PI3DMesh*)PI3DMemoryAlloc (sizeof (PI3DMesh));
				if(!Mesh)
					return 0;
					
				memset(Mesh,0,sizeof(PI3DMesh));
				PIActionDescriptor meshDescriptor;
				tempType = keyMeshClass;
				actionListProcs->GetObject(meshActionList,index,&tempType,&meshDescriptor);
				if(meshDescriptor)
				{
					//name	
					actionDescriptorProcs->GetString(meshDescriptor,keyName,Mesh->name, 40);
					
					//vertices
					actionDescriptorProcs->HasKey(meshDescriptor, keyVertexList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keyVertexList,&Mesh->vertices);
						Mesh->vertices /= sizeof (PI3DVector);
						Mesh->vertex = (PI3DVector*)PI3DMemoryAlloc (Mesh->vertices * sizeof (PI3DVector));
						if(!Mesh->vertex)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keyVertexList,Mesh->vertex);
		#ifdef __ppc__
						for(index2=0;index2<Mesh->vertices;index2++)
							{
							PI3DByteSwapDouble(&Mesh->vertex[index2][0]);
							PI3DByteSwapDouble(&Mesh->vertex[index2][1]);
							PI3DByteSwapDouble(&Mesh->vertex[index2][2]);
							}
		#endif
					}
					
					//normals
					actionDescriptorProcs->HasKey(meshDescriptor, keyNormalList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keyNormalList,&Mesh->normals);
						Mesh->normals /= sizeof (PI3DVector);
						Mesh->normal = (PI3DVector*)PI3DMemoryAlloc (Mesh->normals * sizeof (PI3DVector));
						if(!Mesh->normal)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keyNormalList,Mesh->normal);
		#ifdef __ppc__
						for(index2=0;index2<Mesh->normals;index2++)
							{
							PI3DByteSwapDouble(&Mesh->normal[index2][0]);
							PI3DByteSwapDouble(&Mesh->normal[index2][1]);
							PI3DByteSwapDouble(&Mesh->normal[index2][2]);
							}
		#endif
					}

					//linecolors
					actionDescriptorProcs->HasKey(meshDescriptor, keylineColorList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keylineColorList,&Mesh->lines);
						Mesh->lines /= sizeof (PI3DVector);
						Mesh->linecolors = (PI3DVector*)PI3DMemoryAlloc (Mesh->lines * sizeof (PI3DVector));
						if(!Mesh->linecolors)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keylineColorList,Mesh->linecolors);
		#ifdef __ppc__
						for(index2=0;index2<Mesh->lines;index2++)
						{
							PI3DByteSwapDouble(&Mesh->linecolors[index2][0]);
							PI3DByteSwapDouble(&Mesh->linecolors[index2][1]);
							PI3DByteSwapDouble(&Mesh->linecolors[index2][2]);
						}
		#endif
					}

					//lines
					actionDescriptorProcs->HasKey(meshDescriptor, keyLineList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keyLineList,&Mesh->lines);
						Mesh->lines /= sizeof (PI3DVector);
						Mesh->line = (PI3DVector*)PI3DMemoryAlloc (Mesh->lines * sizeof (PI3DVector));
						if(!Mesh->line)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keyLineList,Mesh->line);
		#ifdef __ppc__
						for(index2=0;index2<Mesh->lines;index2++)
						{
							PI3DByteSwapDouble(&Mesh->line[index2][0]);
							PI3DByteSwapDouble(&Mesh->line[index2][1]);
							PI3DByteSwapDouble(&Mesh->line[index2][2]);
						}
		#endif
					}

					//linematerials
					actionDescriptorProcs->HasKey(meshDescriptor, keylineMaterialList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keylineMaterialList,&Mesh->lines);
						Mesh->lines /= sizeof (int32);
						Mesh->linematerial = (int32*)PI3DMemoryAlloc (Mesh->lines * sizeof (int32));
						if(!Mesh->linematerial)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keylineMaterialList,Mesh->linematerial);
		#ifdef __ppc__
						for(index2=0;index2<Mesh->lines;index2++)
						{
							SWAP32_IF_BIGENDIAN(Mesh->linematerial[index2]);
						}
		#endif
					}

					//vertex colors
					actionDescriptorProcs->HasKey(meshDescriptor, keyVertexColorList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keyVertexColorList,&Mesh->colors);
						Mesh->colors /= sizeof (PI3DVector);
						Mesh->color = (PI3DVector*)PI3DMemoryAlloc (Mesh->colors * sizeof (PI3DVector));
						if(!Mesh->color)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keyVertexColorList,Mesh->color);
		#ifdef __ppc__
						for(index2=0;index2<Mesh->colors;index2++)
						{
							PI3DByteSwapDouble(&Mesh->color[index2][0]);
							PI3DByteSwapDouble(&Mesh->color[index2][1]);
							PI3DByteSwapDouble(&Mesh->color[index2][2]);
						}
		#endif
					}
					//UVs
					actionDescriptorProcs->HasKey(meshDescriptor, keyUVList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keyUVList,&Mesh->textures);
						Mesh->textures /= sizeof (PI3DPoint);
						Mesh->texture = (PI3DPoint*)PI3DMemoryAlloc (Mesh->textures * sizeof (PI3DPoint));
						if(!Mesh->texture)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keyUVList,Mesh->texture);
		#ifdef __ppc__
						for(index2=0;index2<Mesh->textures;index2++)
							{
							PI3DByteSwapDouble(&Mesh->texture[index2][0]);
							PI3DByteSwapDouble(&Mesh->texture[index2][1]);
							}
		#endif
					}

					//faces
					actionDescriptorProcs->HasKey(meshDescriptor, keyFaceList, &hasKey);
					if (hasKey)
					{
						actionDescriptorProcs->GetDataLength(meshDescriptor,keyFaceList,&Mesh->faces);
						Mesh->faces /= sizeof (PI3DFace);
						Mesh->face = (PI3DFace*)PI3DMemoryAlloc (Mesh->faces * sizeof (PI3DFace));
						if(!Mesh->face)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keyFaceList,Mesh->face);
						
						int32 numPoints=0;
						actionDescriptorProcs->GetDataLength(meshDescriptor,keyFaceIndexes,&numPoints);
						numPoints /= sizeof(int32);
						uint32 *faceDataBlob=(uint32*)PI3DMemoryAlloc (numPoints * sizeof (int32));
						if(!faceDataBlob)
							return 0;
							
						actionDescriptorProcs->GetData(meshDescriptor,keyFaceIndexes,faceDataBlob);
						
						uint32 currentPoint=0;
						for(index2=0;index2<Mesh->faces;index2++)
							{
		#ifdef __ppc__
							SWAP32_IF_BIGENDIAN(Mesh->face[index2].flags);
							SWAP32_IF_BIGENDIAN(Mesh->face[index2].smoothing);
							SWAP32_IF_BIGENDIAN(Mesh->face[index2].numPoints);
							SWAP32_IF_BIGENDIAN(Mesh->face[index2].material);
		#endif
							Mesh->face[index2].points=(int32*)PI3DMemoryAlloc (Mesh->face[index2].numPoints * sizeof (int32));
							if(!Mesh->face[index2].points)
								return 0;
								
							Mesh->face[index2].normals=(int32*)PI3DMemoryAlloc (Mesh->face[index2].numPoints * sizeof (int32));
							if(!Mesh->face[index2].normals)
								return 0;
							Mesh->face[index2].textures=(int32*)PI3DMemoryAlloc (Mesh->face[index2].numPoints * sizeof (int32));
							if(!Mesh->face[index2].textures)
								return 0;
							Mesh->face[index2].colors=(int32*)PI3DMemoryAlloc (Mesh->face[index2].numPoints * sizeof (int32));
							if(!Mesh->face[index2].colors)
								return 0;
							for(index3=0;index3<Mesh->face[index2].numPoints;index3++)
								{
		#ifdef __ppc__
								SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
		#endif
								Mesh->face[index2].points[index3]=faceDataBlob[currentPoint];
								currentPoint++;
		#ifdef __ppc__
								SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
		#endif
								Mesh->face[index2].normals[index3]=faceDataBlob[currentPoint];
								currentPoint++;
		#ifdef __ppc__
								SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
		#endif
								Mesh->face[index2].textures[index3]=faceDataBlob[currentPoint];
								currentPoint++;
		#ifdef __ppc__
								SWAP32_IF_BIGENDIAN(faceDataBlob[currentPoint]);
		#endif
								Mesh->face[index2].colors[index3]=faceDataBlob[currentPoint];
								currentPoint++;
								}
							}
						
						PI3DMemoryFree (faceDataBlob);
					}
					
					//matrix
					actionDescriptorProcs->HasKey(meshDescriptor, keyMatrix, &hasKey);
					if (hasKey)
					{
						PIActionList matrixList;
						actionDescriptorProcs->GetList(meshDescriptor,keyMatrix,&matrixList);
						for( i=0;i<4;i++)
							for( j=0;j<4;j++)
								actionListProcs->GetFloat(matrixList,j+i*4,&Mesh->matrix[i][j]);
					}

					//flags
					actionDescriptorProcs->GetInteger(meshDescriptor,keyFlags,&Mesh->flags);

					//hidden
					actionDescriptorProcs->GetInteger(meshDescriptor,keyHidden,&Mesh->hidden);

					//shadow
					actionDescriptorProcs->GetInteger(meshDescriptor,keyShadow,&Mesh->shadow);

					//matrixPresent
					Boolean bPresent;
					actionDescriptorProcs->GetBoolean(meshDescriptor,keyHasMatrix,&bPresent);
					Mesh->matrixPresent = (bool)bPresent;

					//smoothingGroupPresent
					actionDescriptorProcs->GetBoolean(meshDescriptor,keyHasSmoothing,&bPresent);
					Mesh->smoothingGroupPresent = (bool)bPresent;
					
					actionDescriptorProcs->Free(meshDescriptor);
					
					PI3DListAdd((PI3DList **)&scene->meshList, reinterpret_cast<PI3DList *>(Mesh));
				}
			}
		actionListProcs->Free(meshActionList);
		}
	}

	//Dump the material properties
	actionDescriptorProcs->HasKey(actionDescriptor, keyMaterialList, &hasKey);
	if(hasKey)
	{
		//Get the materials
		PIActionList materialActionList;
		actionDescriptorProcs->GetList(actionDescriptor,keyMaterialList,&materialActionList);
		if(materialActionList)
		{
			actionListProcs->GetCount(materialActionList,&listSize);
			for(index=0;index<listSize;index++)
			{
				PI3DMaterial *MatProp = (PI3DMaterial*)PI3DMemoryAlloc (sizeof (PI3DMaterial));
				if(!MatProp)
					return 0;
					
				PIActionDescriptor materialDescriptor;
				tempType = keyMaterialClass;
				actionListProcs->GetObject(materialActionList,index,&tempType,&materialDescriptor);

				//name
				actionDescriptorProcs->GetString(materialDescriptor,keyName,MatProp->name,80);

				//ambient
				actionDescriptorProcs->GetFloat(materialDescriptor,keyAmbientRed, &MatProp->ambient.red);
				actionDescriptorProcs->GetFloat(materialDescriptor,keyAmbientGreen, &MatProp->ambient.green);
				actionDescriptorProcs->GetFloat(materialDescriptor,keyAmbientBlue, &MatProp->ambient.blue);

				//diffuse
				actionDescriptorProcs->GetFloat(materialDescriptor,keyDiffuseRed, &MatProp->diffuse.red);
				actionDescriptorProcs->GetFloat(materialDescriptor,keyDiffuseGreen, &MatProp->diffuse.green);
				actionDescriptorProcs->GetFloat(materialDescriptor,keyDiffuseBlue, &MatProp->diffuse.blue);

				//specular
				actionDescriptorProcs->GetFloat(materialDescriptor,keySpecularRed, &MatProp->specular.red);
				actionDescriptorProcs->GetFloat(materialDescriptor,keySpecularGreen, &MatProp->specular.green);
				actionDescriptorProcs->GetFloat(materialDescriptor,keySpecularBlue, &MatProp->specular.blue);

				//emissive
				actionDescriptorProcs->GetFloat(materialDescriptor,keyEmissiveRed, &MatProp->emissive.red);
				actionDescriptorProcs->GetFloat(materialDescriptor,keyEmissiveGreen, &MatProp->emissive.green);
				actionDescriptorProcs->GetFloat(materialDescriptor,keyEmissiveBlue, &MatProp->emissive.blue);

				//shininess
				actionDescriptorProcs->GetFloat(materialDescriptor,keyShininess, &MatProp->shininess);

				//glossiness
				actionDescriptorProcs->GetFloat(materialDescriptor,keyShininess2, &MatProp->glossiness);

				//transparency
				actionDescriptorProcs->GetFloat(materialDescriptor,keyTransparency, &MatProp->transparency);

				//reflection
				actionDescriptorProcs->GetFloat(materialDescriptor,keyReflection, &MatProp->reflection);

				//selfIllumination
				actionDescriptorProcs->GetInteger(materialDescriptor,keySelfIllumination, &MatProp->selfIllumination);

				//shading
				actionDescriptorProcs->GetInteger(materialDescriptor,keyShading, &tempInt);
				MatProp->shading = (PI3DShadingEnum)tempInt;

				//twoSided
				Boolean bProp;
				actionDescriptorProcs->GetBoolean(materialDescriptor,keyTwoSide, &bProp);
				MatProp->twoSided = (bool)bProp;
				//wireframe
				actionDescriptorProcs->GetBoolean(materialDescriptor,keyWireframe, &bProp);
				MatProp->wireframe = (bool)bProp;
				//decal
				actionDescriptorProcs->GetBoolean(materialDescriptor,keyDecal, &bProp);
				MatProp->decal = (bool)bProp;
				//wireframesize
				actionDescriptorProcs->GetFloat(materialDescriptor,keyWireframeSize, &MatProp->wireframesize);

				//maps
				PIActionList mapActionList;
				actionDescriptorProcs->GetList(materialDescriptor,keyMapList,&mapActionList);
				if(mapActionList)
					{
					for(index2=0;index2<kNumSupportedTextureTypes;index2++)
					{
						PIActionDescriptor mapDescriptor;
						tempType = keyMapClass;
						actionListProcs->GetObject(mapActionList,index2,&tempType,&mapDescriptor);
						
						if(mapDescriptor)
						{
							//map type - not needed
							actionDescriptorProcs->GetInteger(mapDescriptor, keyType, &tempInt);

							//map name
							actionDescriptorProcs->GetString(mapDescriptor, keyName, MatProp->maps[index2].map, 256);

							//map strength
							actionDescriptorProcs->GetFloat(mapDescriptor, keyStrength, &MatProp->maps[index2].strength);

							//map uscale
							actionDescriptorProcs->GetFloat(mapDescriptor, keyUScale, &MatProp->maps[index2].uscale);

							//map vscale
							actionDescriptorProcs->GetFloat(mapDescriptor, keyVScale, &MatProp->maps[index2].vscale);

							//map uoffset
							actionDescriptorProcs->GetFloat(mapDescriptor, keyUOffset, &MatProp->maps[index2].uoffset);

							//map voffset
							actionDescriptorProcs->GetFloat(mapDescriptor, keyVOffset, &MatProp->maps[index2].voffset);

							//map angle
							actionDescriptorProcs->GetFloat(mapDescriptor, keyAngle, &MatProp->maps[index2].angle);

							//map flags
							int32 temp2;
							actionDescriptorProcs->GetInteger(mapDescriptor, keyFlags, &temp2);
							MatProp->maps[index2].flags = (PI3DTextureMapFlags)temp2;

							// map 3ds style
							Boolean bProp;
							actionDescriptorProcs->GetBoolean(mapDescriptor, key3DSStyle, &bProp);
							MatProp->maps[index2].mapIs3DSStyle = (bool)bProp;
							
							actionDescriptorProcs->Free(mapDescriptor);
						}
					}	
				actionListProcs->Free(mapActionList);
				}
				actionDescriptorProcs->Free(materialDescriptor);
				PI3DListAdd((PI3DList **)&scene->matPropList, reinterpret_cast<PI3DList *>(MatProp));
			}
		actionListProcs->Free(materialActionList);
		}
	}

	//Get the key frames
	actionDescriptorProcs->HasKey(actionDescriptor, keyKeyFrameList, &hasKey);
	if(hasKey)
	{
		//Get the materials
		PIActionList KeyFrameActionList;
		actionDescriptorProcs->GetList(actionDescriptor,keyKeyFrameList,&KeyFrameActionList);
		if(KeyFrameActionList)
		{
			actionListProcs->GetCount(KeyFrameActionList,&listSize);
			for(index=0;index<listSize;index++)
			{
				PI3DKeyFrame *KeyframeProp = (PI3DKeyFrame*)PI3DMemoryAlloc (sizeof (PI3DKeyFrame));
				if(!KeyframeProp)
					return 0;
					
				KeyframeProp->positionData = NULL;
				KeyframeProp->scaleData = NULL;
				KeyframeProp->rotationData = NULL;
				KeyframeProp->fOVData = NULL;
				KeyframeProp->rollData = NULL;
				KeyframeProp->hideData = NULL;
				KeyframeProp->hotData = NULL;
				KeyframeProp->colData = NULL;
				KeyframeProp->fallData =  NULL;
				KeyframeProp->morphData =  NULL;
				PIActionDescriptor KeyframeDescriptor;
				tempType = keyKeyFrameClass;
				actionListProcs->GetObject(KeyFrameActionList,index,&tempType,&KeyframeDescriptor);
				
				if(KeyframeDescriptor)
				{
					//keyframe props
					actionDescriptorProcs->GetString(KeyframeDescriptor,keyName,KeyframeProp->name,80);
					actionDescriptorProcs->GetString(KeyframeDescriptor,keyInstanceName,KeyframeProp->instancename,80);
					actionDescriptorProcs->GetInteger(KeyframeDescriptor,keyFlags1, &KeyframeProp->flags1);
					actionDescriptorProcs->GetInteger(KeyframeDescriptor,keyFlags2, &KeyframeProp->flags2);
					actionDescriptorProcs->GetInteger(KeyframeDescriptor,keyU3DNodeID, &KeyframeProp->nodeID);
					actionDescriptorProcs->GetInteger(KeyframeDescriptor,keyU3DParentID, &KeyframeProp->parentNodeID);
					int32 dmmy = 0;
					actionDescriptorProcs->GetInteger(KeyframeDescriptor,keyU3DType, &dmmy);
					KeyframeProp->nodeType = (PI3DNodeTypes)dmmy;
					actionDescriptorProcs->GetInteger(KeyframeDescriptor,keyFlags2, &KeyframeProp->flags2);

					//pivot pt
					PIActionList PivotActionList;
					actionDescriptorProcs->GetList(KeyframeDescriptor,keyPivotList,&PivotActionList);
					if(PivotActionList)
					{
						PIActionDescriptor pivotDescriptor;
						tempType = keyPivotObject;
						actionListProcs->GetObject(PivotActionList,0,&tempType,&pivotDescriptor);
						if(pivotDescriptor)
						{
							actionDescriptorProcs->GetFloat(pivotDescriptor,keyPivotX, &KeyframeProp->pivot[0]);
							actionDescriptorProcs->GetFloat(pivotDescriptor,keyPivotY, &KeyframeProp->pivot[1]);
							actionDescriptorProcs->GetFloat(pivotDescriptor,keyPivotZ, &KeyframeProp->pivot[2]);
							actionDescriptorProcs->Free(pivotDescriptor);
						}
						actionListProcs->Free(PivotActionList);
					}
					//matrix
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyMatrix, &hasKey);
					if (hasKey)
					{
					PIActionList matrixList;
					actionDescriptorProcs->GetList(KeyframeDescriptor,keyMatrix,&matrixList);
					for( i=0;i<4;i++)
						for( j=0;j<4;j++)
						actionListProcs->GetFloat(matrixList,j+i*4,&KeyframeProp->localmatrix[i][j]);
					}

					//pivot_present
					Boolean bPresent;
					actionDescriptorProcs->GetBoolean(KeyframeDescriptor,keyHasPivot,&bPresent);
					KeyframeProp->usepivot = (bool)bPresent;

					//matrix present
					bPresent = FALSE;
					actionDescriptorProcs->GetBoolean(KeyframeDescriptor,keyHasLocalMatrix,&bPresent);
					KeyframeProp->matrixPresent = (bool)bPresent;

					//animation range
					actionDescriptorProcs->GetBoolean(KeyframeDescriptor,keyHasRange,&bPresent);
					KeyframeProp->userange = (bool)bPresent;

					//position data
					OSErr err = noErr;
					int32 bDidit = 0;
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframePositionList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList PositionActionList;
						actionDescriptorProcs->GetList(KeyframeDescriptor,keyframePositionList,&PositionActionList);
						if(PositionActionList)
						{
							actionListProcs->GetCount(PositionActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor PositionDescriptor;
								tempType = keyframePositionActionList;
								actionListProcs->GetObject(PositionActionList,animindex,&tempType,&PositionDescriptor);
								if(PositionDescriptor)
									{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &PositionDescriptor);
								
									actionDescriptorProcs->Free(PositionDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->positionData, reinterpret_cast<PI3DList *>(Animation));
									}
							}
							actionListProcs->Free(PositionActionList);
						}
					}
					//scale data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeScaleList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList ScaleActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeScaleList,&ScaleActionList);
						if(ScaleActionList)
						{
							actionListProcs->GetCount(ScaleActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor ScaleDescriptor;
								tempType = keyframeScaleActionList;
								actionListProcs->GetObject(ScaleActionList,animindex,&tempType,&ScaleDescriptor);
								if(ScaleDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &ScaleDescriptor);
									actionDescriptorProcs->Free(ScaleDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->scaleData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(ScaleActionList);
						}
					}
					//rotation data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeRotationList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList RotationActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeRotationList,&RotationActionList);
						if(RotationActionList)
						{
							actionListProcs->GetCount(RotationActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor RotationDescriptor;
								tempType = keyframeRotationActionList;
								actionListProcs->GetObject(RotationActionList,animindex,&tempType,&RotationDescriptor);
								if(RotationDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &RotationDescriptor);
									actionDescriptorProcs->Free(RotationDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->rotationData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(RotationActionList);
						}
					}
					//FOV data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeFOVList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList FOVActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeFOVList,&FOVActionList);
						if(FOVActionList)
						{
							actionListProcs->GetCount(FOVActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor FOVDescriptor;
								tempType = keyframeFOVActionList;
								actionListProcs->GetObject(FOVActionList,animindex,&tempType,&FOVDescriptor);
								if(FOVDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &FOVDescriptor);
									actionDescriptorProcs->Free(FOVDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->fOVData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(FOVActionList);
						}
					}
					//Roll data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeRollList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList RollActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeRollList,&RollActionList);
						if(RollActionList)
						{
							actionListProcs->GetCount(RollActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor RollDescriptor;
								tempType = keyframeRollActionList;
								actionListProcs->GetObject(RollActionList,animindex,&tempType,&RollDescriptor);
								if(RollDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &RollDescriptor);
									actionDescriptorProcs->Free(RollDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->rollData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(RollActionList);
						}
					}
					//Col data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeColorList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList ColorActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeColorList,&ColorActionList);
						if(ColorActionList)
						{
							actionListProcs->GetCount(ColorActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor ColorDescriptor;
								tempType = keyframeColorActionList;
								actionListProcs->GetObject(ColorActionList,animindex,&tempType,&ColorDescriptor);
								if(ColorDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &ColorDescriptor);
									actionDescriptorProcs->Free(ColorDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->colData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(ColorActionList);
						}
					}
					//Hot data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeHotList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList HotActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeHotList,&HotActionList);
						if(HotActionList)
						{
							actionListProcs->GetCount(HotActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor HotDescriptor;
								tempType = keyframeHotActionList;
								actionListProcs->GetObject(HotActionList,animindex,&tempType,&HotDescriptor);
								if(HotDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &HotDescriptor);
									actionDescriptorProcs->Free(HotDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->hotData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(HotActionList);
						}
					}
					//Fall data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeFalloffList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList FallActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeFalloffList,&FallActionList);
						if(FallActionList)
						{
							actionListProcs->GetCount(FallActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor FallDescriptor;
								tempType = keyframeFalloffActionList;
								actionListProcs->GetObject(FallActionList,animindex,&tempType,&FallDescriptor);
								if(FallDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &FallDescriptor);
									actionDescriptorProcs->Free(FallDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->fallData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(FallActionList);
						}
					}
					//Hide data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeHideList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList HideActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeHideList,&HideActionList);
						if(HideActionList)
						{
							actionListProcs->GetCount(HideActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor HideDescriptor;
								tempType = keyframeHideActionList;
								actionListProcs->GetObject(HideActionList,animindex,&tempType,&HideDescriptor);
								if(HideDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &HideDescriptor);
									actionDescriptorProcs->Free(HideDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->hideData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(HideActionList);
						}
					}
					//morph data
					actionDescriptorProcs->HasKey(KeyframeDescriptor, keyframeMorphList, &hasKey);
					if(hasKey)
					{
						uint32 animlistSize=0;
						uint32 animindex=0;
						PIActionList MorphActionList;
						err = actionDescriptorProcs->GetList(KeyframeDescriptor,keyframeMorphList,&MorphActionList);
						if(MorphActionList)
						{
							actionListProcs->GetCount(MorphActionList,&animlistSize);
							for(animindex=0;animindex<animlistSize;animindex++)
							{
								PI3DAnimationData *Animation = (PI3DAnimationData*)PI3DMemoryAlloc (sizeof (PI3DAnimationData));
								if(!Animation)
									return 0;
								PIActionDescriptor MorphDescriptor;
								tempType = keyframeMorphActionList;
								actionListProcs->GetObject(MorphActionList,animindex,&tempType,&MorphDescriptor);
								if(MorphDescriptor)
								{
									bDidit = PI3DParseAnimDataIntoScene(actionDescriptorProcs, actionListProcs, Animation, &MorphDescriptor);
									actionDescriptorProcs->Free(MorphDescriptor);
									PI3DListAdd((PI3DList **)&KeyframeProp->morphData, reinterpret_cast<PI3DList *>(Animation));
								}
							}
							actionListProcs->Free(MorphActionList);
						}
					}
					
					actionDescriptorProcs->Free(KeyframeDescriptor);
					PI3DListAdd((PI3DList **)&scene->keyFrameList, reinterpret_cast<PI3DList *>(KeyframeProp));
				}
			}
		
		actionListProcs->Free(KeyFrameActionList);
		}
	}

	*inScene = scene;
	return 1;
}

/******************************************************************************/
#if Macintosh
#pragma mark -
#endif
/******************************************************************************/

	
uint32 PI3DStringLengthW(const wchar_t *string)
	{
	if(!string)
		return 0;
		
	uint32 strLen=0;
	
	while(string[strLen] != 0) 
		strLen++;
	
	return strLen;
	}
uint32 PI3DStringLength16(const uint16 *string)
	{
	if(!string)
		return 0;
		
	uint32 strLen=0;
	
	while(string[strLen] != 0) 
		strLen++;
	
	return strLen;
	}	

void PI3DStringCopy16C(char *destString,const uint16 *sourceString)
	{
	if(!destString || !sourceString)
		return;
		
	//¥¥¥ÊWe hope dest has enough memory
	uint32 strLen=PI3DStringLength16(sourceString);
	for(uint16 i=0;i<=strLen;i++)
		destString[i]=(char)sourceString[i];
	
	destString[strLen]=0;
	}
void PI3DStringCopyWC(char *destString,const wchar_t *sourceString)
	{
	if(!destString || !sourceString)
		return;
		
	//¥¥¥ÊWe hope dest has enough memory
	uint32 strLen=PI3DStringLengthW(sourceString);
	for(uint16 i=0;i<=strLen;i++)
		destString[i]=(char)sourceString[i];
	
	destString[strLen]=0;
	}

void PI3DStringCopyC16(uint16 *destString,const char *sourceString)
	{
	if(!destString || !sourceString)
		return;
		
	//¥¥¥ÊWe hope dest has enough memory
	uint32 strLen=strlen(sourceString);
	for(uint16 i=0;i<=strLen;i++)
		destString[i]=(uint16)sourceString[i];
	
	destString[strLen]=0;
	}
	
void PI3DStringCopy16W(wchar_t *destString,const uint16 *sourceString)
	{
	if(!destString || !sourceString)
		return;
		
	//¥¥¥ÊWe hope dest has enough memory
	uint32 strLen=PI3DStringLength16(sourceString);
	for(uint16 i=0;i<=strLen;i++)
		destString[i]=(wchar_t)sourceString[i];
	
	destString[strLen]=0;
	}	
	
//¥¥¥Remember to get rid of this stuff when we get rid of data and dataSize
void PI3DGetPathFromSpec(SPPlatformFileSpecificationW *fileSpec,uint16 *path)
	{
	if(!fileSpec || !path)
		return;
		
	#if MSWindows
	 uint32 strLen=PI3DStringLength16(fileSpec->mReference);
	 for(uint16 i=0;i<=strLen;i++)
		path[i]=fileSpec->mReference[i];
		
	 path[strLen]=0;
	#else
	 CFURLRef url = CFURLCreateFromFSRef( kCFAllocatorDefault, &fileSpec->mReference );
	 CFStringRef cfString = NULL;
	 char tempPath[2048];
	 if ( url != NULL )
		{
		cfString = CFURLCopyFileSystemPath( url, kCFURLPOSIXPathStyle );
		CFStringGetCString(cfString,(char*)tempPath,2048,kCFStringEncodingUTF8);
		int32 len=CFStringGetLength(cfString);
		CFRelease( url );
		path[len]=0;
		
		//This function wants us to fill in a 2 byte path, so we'll copy our utf8 string into it
		PI3DStringCopyC16(path,tempPath);
		}
		
	#endif
	}

/* string splitter */
Boolean PI3DSplitString(char *leftStr, char *srcStr, bool splitOnWhitespace, char splitChar)
{ // returns true if a split occurred
	int32	i;
	int32	leftI = 0;
	int32	srcI = 0;

	leftStr[leftI] = '\0';
	while (true)
	{
		if (srcStr[srcI] == '\0')
		{
			srcStr[0] = '\0';
			break;
		}
		if (srcStr[srcI] == splitChar || (splitOnWhitespace && isspace(srcStr[srcI])))
		{
			while (srcStr[srcI] == splitChar || (splitOnWhitespace && isspace(srcStr[srcI])))
				srcI++;
			i = 0;
			while (srcStr[i + srcI] != '\0')
			{
				srcStr[i] = srcStr[i + srcI];
				i++;
			}
			srcStr[i] = '\0';
			break;
		}
		leftStr[leftI++] = srcStr[srcI++];
	}
	leftStr[leftI] = '\0';
	if (leftStr[0] == '\0')
		return false;
	else
		return true;
}

/* Trim whitespace */
void PI3DTrimLeft(char *str)
{
	int32		i, copyFrom;
	int32		len = strlen(str);

	copyFrom = len;
	for (i = 0; i < len; i++)
	{
		if (!isspace(str[i]))
		{
			copyFrom = i;
			break;
		}
	}

	if (copyFrom < len)
	{
		i = 0;
		while (copyFrom < len)
		{
			str[i] = str[copyFrom];
			i++;
			copyFrom++;
		}
		str[i] = '\0';
	}
	else
		str[0] = '\0';
}

void PI3DTrimRight(char *str)
{
	int32		i;
	int32		len = strlen(str);

	i = len;
	while (i)
	{
		if (!isspace(str[i - 1]))
		{
			str[i] = '\0';
			len = i;
			break;
		}
		i--;
	}
	if (i == 0)
		str[0] = '\0';
}

