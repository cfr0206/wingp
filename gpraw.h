#ifndef __GPRAW__
#define	__GPRAW__

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

#endif
