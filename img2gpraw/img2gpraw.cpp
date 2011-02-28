#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "gpraw.h"


//*****************************************************************************
#define LOGINIT()                                \
    {                                                \
        FILE *_flog_ = fopen ( log_filename,"w" );     \
        if ( _flog_ )                                    \
            fclose ( _flog_ );                       \
    }

#define LOG(...)                                    \
    if (log_enable)                                     \
    {                                                   \
        FILE *_flog_ = fopen ( log_filename,"a+t" );  \
        if ( _flog_ ) {                                 \
            time_t rawtime;      \
            struct tm * timeinfo;                  \
            time ( &rawtime );                  \
            timeinfo = localtime ( &rawtime );  \
            fprintf ( _flog_,"%04d.%02d.%02d %02d:%02d:%02d: ",1900+timeinfo->tm_year,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);\
            fprintf ( _flog_, __VA_ARGS__ );            \
            fflush(_flog_);                             \
            fclose ( _flog_ );                          \
        }                                               \
    }


//********************** Command line options **********************************
const char* short_options = "i:hlr";

const struct option long_options[] = {
    {"image" ,  required_argument, NULL, 'i'},
    {"help"  ,  no_argument,       NULL, 'h'},
    {"log"   ,  no_argument,       NULL, 'l'},
    {"remove",  no_argument,       NULL, 'r'},
    //    {"flipv",  no_argument,       NULL, 'v'},
    //    {"fliph",  no_argument,       NULL, 'g'},
    {NULL   ,            0,       NULL, 0}
};

typedef enum {TRIM_LEFT = 1, TRIM_RIGHT, TRIM_LEFT_AND_RIGHT} trim_type;
void strtrim(char *str, char *s, trim_type t);
// вместо написания отдельных функций - пара макросов
#define strltrim(str, c) strtrim(str, c, TRIM_LEFT)
#define strrtrim(str, c) strtrim(str, c, TRIM_RIGHT)
#define strlrtrim(str, c) strtrim(str, c, TRIM_LEFT_AND_RIGHT)



//*****************************************************************************
struct imginfo
{
    int pcount;
    int width;
    int height;
    char color_space[32];
    int channels;
    char channel_name[32][32];
    int bitspp;
    int xres;
    int yres;
};
//**************************** Command ****************************************
bool log_enable=false;
char log_filename[1024]= {0};
char *filename=NULL;
bool flipv=false;
bool fliph=false;
bool file_remove=false;//delete src file
//********************** Functions declare ************************************
bool fexist ( char *fn );
void do_help();
void parse_opt ( int argc, char **argv );
void get_imginfo(char * fn, imginfo &ii);
void set_channels_count(imginfo &ii);
void set_channels_name(imginfo &ii);
size_t get_fsize( const char * szFileName );
//*****************************************************************************

char own_path[1024]= {0};

int main ( int argc, char *argv[] ) {
    if ( argc==1 ) {
        do_help();
        return 0;
    }
    LOGINIT();
    parse_opt ( argc, argv );

    char *own=argv[0];
    char *last_del = strrchr ( own,'\\' );
    int len= ( int ) ( last_del-own ) +1;
    strncpy ( own_path,own,len );

    sprintf(log_filename,"%simg2gpraw.log",own_path);

    char tmp[1024]= {0};

    char *env=getenv ( "GPRINT_TEMP" );
    if ( env==NULL ) {
        LOG ( "GPRINT_TEMP = NULL\n" );
        char *own=argv[0];
        char *last_del = strrchr ( own,'\\' );
        int len= ( int ) ( last_del-own ) +1;
        strncpy ( tmp,own,len );
    } else
        strcpy ( tmp,env );
    LOG ( "Temporary folder: %s\n",tmp );


    if ( fexist ( filename ) ==false ) {
        LOG ( "File '%s' open error.\n",filename );
        return 1;
    }
    LOG ( "File '%s' exist.\n",filename );

    imginfo ii;
    get_imginfo(filename, ii);

    int count=ii.pcount;
    LOG ( "Page count: %d\n",count );


    char fnout[1024]= {0};
    char fnext[1024]= {0};

    for ( int page=0; page<count; page++ ) {
        LOG ( "Page: %d\n", page+1 )
        LOG ( "Input image parametrs:\n" );
        LOG ( "\twidth: %d\n", ii.width );
        LOG ( "\theight: %d\n", ii.height );
        LOG ( "\tcolor_space: %s\n", ii.color_space);
        LOG ( "\tbitspp: %d\n", ii.bitspp );
        LOG ( "\tdpi_x: %d\n", ii.xres );
        LOG ( "\tdpi_y: %d\n", ii.yres );

        gpraw_t gpraw;
        gpraw.w=ii.width;
        gpraw.h=ii.height;
        gpraw.planes=ii.channels;
        gpraw.depth=ii.bitspp;
        gpraw.xdpi=ii.xres;
        gpraw.ydpi=ii.yres;

        strcpy ( gpraw.color_space, ii.color_space );
        for(int i=0; i<ii.channels; i++)
            strcpy ( gpraw.channel_name[i], ii.channel_name[i]);

        strcpy ( gpraw.document_name,filename );

        char *ld = strrchr ( filename,'\\' );
        if ( ld==NULL )
            strcpy ( fnext,filename );
        else
            strcpy ( fnext,ld+1 );

        sprintf ( fnout,"%s%s%05d.gpraw", tmp, fnext, page );
        LOG ( "Output filename: %s\n", fnout );

        //write head
        FILE *fout = fopen ( fnout,"wb" );
        fwrite ( &gpraw,sizeof ( gpraw ),1,fout );
	LOG("Write %d byte of head\n",sizeof ( gpraw ));

        char data[1024]= {0};
        if (strcmp ( ii.color_space,"GRAYSCALE" )==0)//convert want "gray:-"
            sprintf(data,"%sconvert.exe %s[%d] %s:- 2>nul",own_path,filename,page, "GRAY");
        else
            sprintf(data,"%sconvert.exe %s[%d] %s:- 2>nul",own_path, filename,page, ii.color_space);

        //decode image
        FILE *fin = popen(data,"rb");
        if (fin==NULL)
        {
            LOG ( "File 'convert.exe' open error\n" );
            exit(1);
        }
        //write decoded image
        size_t cb;
        size_t size_data=0;
        while((cb=fread(data, 1, sizeof(data), fin))!=0)
        {
            fwrite ( data,cb,1,fout );
            size_data+=cb;
        }

        pclose(fin);

        LOG("Write %ld byte of page %d\n",size_data, page+1);
        //decode icm
        char cmd_icm[1024]= {0};
        sprintf(cmd_icm,"%sconvert.exe %s[%d] icm:- 2>nul",own_path, filename,page);
        FILE *fin_icm = popen(cmd_icm, "rb");
        size_t size_icm=0;
        if (fin_icm)
        {
            while((cb=fread(data, 1, sizeof(data), fin_icm))!=0)
            {
                fwrite ( data,cb,1,fout );
                size_icm+=cb;
            }
            if (size_icm!=0)
            {   //correct head
                gpraw.size_icc=size_icm;
                gpraw.offset_icc = sizeof(gpraw_t)+size_data;
                //rewrite head
                fseek(fout,0,SEEK_SET);
                fwrite ( &gpraw,sizeof ( gpraw ),1,fout );
                LOG("Write %d byte of .icm profile of page %d.\n",size_icm,page+1);
            }
            else
            {
                LOG("Page %d has not .icm profile.\n",page+1);
            }
        }
        else
        {
            LOG("Encode icm error.\n");
            exit(1);
        }

        if (fin_icm)
            pclose(fin_icm);

        fflush(fout);
        fclose ( fout );

        LOG ( "***************************************************************\n" );

    }
    if (file_remove)
        remove ( filename );

    //make file with list of files
    char filelistname[1024]= {0};
    sprintf ( filelistname,"%s%s.filelist", tmp, fnext );
    FILE *flout=fopen ( filelistname,"wt" );
    for ( int page=0; page<count; page++ ) {
        fprintf ( flout,"%s%s%05d.gpraw\n", tmp, fnext, page );
    }
    fclose ( flout );

    return 0;
}
//Test file exist
bool fexist ( char *fn ) {
    FILE *fe=fopen ( fn,"r" );
    bool rz = fe!=NULL;
    if ( fe ) fclose ( fe );
    return rz;
}

void do_help() {
    printf ("img2gpraw (%s)\n",__DATE__);

    printf (
        "Utilite to convert any image to gpraw format.\n"
        "Usage:\n"
        "img2gpraw [--help | -h]\n"
        "img2gpraw {--image | -i}<filename> [--log | -l] [--remove | -r]\n"
        "\nOptions:\n"
        "--help,   -h\t- help\n"
        "--image,  -i\t- input filename\n"
        "--log,    -l\t- enable log\n"
        "--remove, -r\t- remove source file\n"
        "\nCopyright(c) 2010-2011, D.Gar'kaev aka Dickobraz.\n"
    );
}

void parse_opt ( int argc, char **argv ) {
    int opt;
    int option_index = -1;
    while ( ( opt = getopt_long ( argc, argv, short_options,
                                  long_options, &option_index ) ) != -1 ) {
        switch ( opt ) {
        case 'h': {
            do_help();
            exit ( 0 );
        };
        case 'l': {
            log_enable=true;
            break;
        }
        case 'i': { //image
            if ( optarg )
                filename = strdup ( optarg );
            break;
        }
        case 'r': {
            file_remove=true;
            break;
        }

        case '?':
        default: {
            fprintf ( stderr, "found unknown option\n" );
            exit ( 1 );
        };
        };
        option_index = -1;
    };
}

void get_imginfo(char * fn, imginfo &ii)
{
    char cmd[1024]= {0};
    sprintf(cmd, "%sconvert.exe -format \"%s\" %s info: 2>nul",
            own_path,
            "%n\\n%[width]\\n%[height]\\n%[colorspace]\\n%z\\n%[xresolution]\\n%[yresolution]\\n",
            fn);

    FILE *fin=popen(cmd,"r");
    if (fin==NULL)
    {
        LOG ( "Command '%s' execute error.\n", cmd);
        exit(1);
    }

    LOG ( "Command:\n%s\n", cmd);

    char buf[1024]= {0};
    int id=0;
    /*
    0- количество страниц
    1- width
    2- height
    3- colorspace
    4- bit per plane
    5- разрешение по x
    6- разрешение по y
    */
    while (fgets(buf,sizeof(buf),fin))
    {
        switch(id)
        {
        case 0://page count
            ii.pcount=atoi(buf);
            if (ii.pcount==0) {
                LOG("Parsing error 'page_count'\n");
                exit(1);
            }
            break;
        case 1://width
            ii.width=atoi(buf);
            if(ii.width==0)  {
                LOG("Parsing error 'width'\n");
                exit(1);
            }
            break;
        case 2://height
            ii.height=atoi(buf);
            if(ii.height==0)  {
                LOG("Parsing error 'height'\n");
                exit(1);
            }
            break;
        case 3://colorspace
            strrtrim(buf,"\n");
            if (buf[0]==0)
            {
                LOG("Parsing error 'colorspace'\n");
                exit(1);
            }

            for (int i = 0; buf[i]; i++)
                buf[i] = toupper(buf[i]);

            if (strcmp ( buf,"GRAY" )==0)
                strcpy(ii.color_space,"GRAYSCALE");
            else
                strcpy(ii.color_space,buf);
            set_channels_count(ii);
            set_channels_name(ii);
            break;
        case 4://bits per plane
            ii.bitspp=atoi(buf);
            if(ii.bitspp==0)  {
                LOG("Parsing error 'bits per plane'\n");
                exit(1);
            }
            break;
        case 5://xres
            ii.xres=atoi(buf);
            if(ii.xres==0)  {
                LOG("Parsing error 'xres'\n");
                exit(1);
            }
            break;
        case 6://yres
            ii.yres=atoi(buf);
            if(ii.yres==0)  {
                LOG("Parsing error 'yres'\n");
                exit(1);
            }
            break;
        }
        if(id==6) break;
        id++;
    }

    pclose(fin);
}

void strtrim(char *str, char *s, trim_type t) {
    int len = strlen(str), slen = strlen(s);
    int beg = 0;

    if (t & TRIM_RIGHT) {
        char b = 1;
        int i;
        while (b) {
            b = 0;

            for (i = 0; i < slen; i++)
                if (str[len - 1] == s[i]) {
                    str[--len] = 0;
                    b = 1;
                    break;
                }
        }
    }
    if (t & TRIM_LEFT) {
        char b = 1;
        int i;
        while (b) {
            b = 0;

            for (i = 0; i < slen; i++)
                if (str[beg] == s[i]) {
                    str[beg++] = 0;
                    b = 1;
                    break;
                }
        }
    }
    if (beg)
        memmove(str, str + beg, len - beg + 1);
}


void set_channels_count(imginfo &ii)
{
    if (strcmp ( ii.color_space,"GRAYSCALE" ) ==0) ii.channels=1;
    if (strcmp ( ii.color_space,"RGB" ) ==0) ii.channels=3;
    if (strcmp ( ii.color_space,"CMY" ) ==0) ii.channels=3;
    if (strcmp ( ii.color_space,"CMYK" ) ==0) ii.channels=4;
}

void set_channels_name(imginfo &ii)
{
    if (strcmp ( ii.color_space,"GRAYSCALE" ) ==0)
    {
        strcpy(ii.channel_name[0],"Gray");
    }
    if (strcmp ( ii.color_space,"RGB" ) ==0)
    {
        strcpy(ii.channel_name[0],"Red");
        strcpy(ii.channel_name[1],"Green");
        strcpy(ii.channel_name[2],"Blue");
    }
    if (strcmp ( ii.color_space,"CMY" ) ==0)
    {
        strcpy(ii.channel_name[0],"Cyan");
        strcpy(ii.channel_name[1],"Magenta");
        strcpy(ii.channel_name[2],"Yellow");
    }
    if (strcmp ( ii.color_space,"CMYK" ) ==0)
    {
        strcpy(ii.channel_name[0],"Cyan");
        strcpy(ii.channel_name[1],"Magenta");
        strcpy(ii.channel_name[2],"Yellow");
        strcpy(ii.channel_name[3],"Black");
    }

}


size_t get_fsize( const char * szFileName )
{
    struct stat fileStat;
    int err = stat( szFileName, &fileStat );
    if (0 != err) return 0;
    return fileStat.st_size;
}
