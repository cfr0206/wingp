#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "gpraw.h"
#include "FreeImage.h"

//*****************************************************************************
#define LOGINIT()                                \
{                                                \
FILE *_flog_ = fopen ( "rm2gpraw.log","w" );     \
if ( _flog_ )                                    \
        fclose ( _flog_ );                       \
}

#define LOG(...)                                    \
if (log_enable)                                     \
{                                                   \
    FILE *_flog_ = fopen ( "rm2gpraw.log","a+t" );  \
    if ( _flog_ ) {                                 \
        fprintf ( _flog_, __VA_ARGS__ );            \
        fflush(_flog_);                             \
        fclose ( _flog_ );                          \
    }                                               \
}


//********************** Command line options **********************************
const char* short_options = "i:hl";

const struct option long_options[] = {
    {"image",  required_argument, NULL, 'i'},
    {"help" ,  no_argument,       NULL, 'h'},
    {"log"  ,  no_argument,       NULL, 'l'},
    {NULL   ,            0,       NULL, 0}
};
//**************************** Command ****************************************
bool log_enable=false;
char *filename=NULL;
//********************** Functions declare ************************************
FIMULTIBITMAP* GenericLoader ( const char* lpszPathName, int flag );
void FreeImageErrorHandler ( FREE_IMAGE_FORMAT fif, const char *message );
bool fexist ( char *fn );
void do_help();
void parse_opt ( int argc, char **argv );
//*****************************************************************************

int main ( int argc, char *argv[] ) {
    if ( argc==1 ) {
        do_help();
        return 0;
    }
    LOGINIT();
    parse_opt ( argc, argv );

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

    FreeImage_SetOutputMessage ( FreeImageErrorHandler );

    FIMULTIBITMAP* mdib=GenericLoader ( filename,0 );
    if ( mdib==NULL )
        return 1;

    int count=FreeImage_GetPageCount ( mdib );
    LOG ( "Page count: %d\n",count );

    FIBITMAP* dibpage;

    char fnout[1024]= {0};
    char fnext[1024]= {0};

    for ( int page=0; page<count; page++ ) {
        dibpage = FreeImage_LockPage ( mdib, page );
        if ( dibpage ) {
            FIBITMAP* dib = FreeImage_ConvertTo24Bits ( dibpage );

            if ( dib ) {
                int w, h, bpp, dpix, dpiy;
                w=FreeImage_GetWidth ( dib );
                h=FreeImage_GetHeight ( dib );
                bpp=FreeImage_GetBPP ( dib );
                dpix= ( int ) ( ( FreeImage_GetDotsPerMeterX ( dib ) *1.0 ) /1000.0*25.4 );
                dpiy= ( int ) ( ( FreeImage_GetDotsPerMeterY ( dib ) *1.0 ) /1000.0*25.4 );
                //FREE_IMAGE_COLOR_TYPE ict =  FreeImage_GetColorType ( dib );

                LOG ( "Page number: %d\n",page )
                LOG ( "Input image parametrs:\n" );
                LOG ( "\twidth: %d\n",w );
                LOG ( "\theight: %d\n",h );
                LOG ( "\tbpp: %d\n",bpp );
                LOG ( "\tdpi_x: %d\n",dpix );
                LOG ( "\tdpi_y: %d\n",dpiy );

                gpraw_t gpraw;
                gpraw.w=w;
                gpraw.h=h;
                gpraw.planes=3;
                gpraw.depth=8;
                gpraw.xdpi=dpix;
                gpraw.ydpi=dpiy;
                strcpy ( gpraw.color_space,"RGB" );
                strcpy ( gpraw.channel_name[0],"Red" );
                strcpy ( gpraw.channel_name[1],"Green" );
                strcpy ( gpraw.channel_name[2],"Blue" );
                strcpy ( gpraw.document_name,filename );

                char *ld = strrchr ( filename,'\\' );
                if ( ld==NULL )
                    strcpy ( fnext,filename );
                else
                    strcpy ( fnext,ld+1 );

                sprintf ( fnout,"%s%s%05d.gpraw", tmp, fnext, page );
                LOG ( "Output filename: %s\n", fnout );

                FILE *fout = fopen ( fnout,"wb" );

                fwrite ( &gpraw,sizeof ( gpraw ),1,fout );

                BYTE *data;
                for ( int scanline=0; scanline<h; scanline++ ) {
                    data = FreeImage_GetScanLine ( dib, scanline );
                    fwrite ( data,w*3,1,fout );
                }

                fclose ( fout );

                FreeImage_Unload ( dib );
            }
            FreeImage_UnlockPage ( mdib, dibpage, FALSE );
            LOG ( "***************************************************************\n" );
        }
    }
    FreeImage_CloseMultiBitmap ( mdib );
    remove ( filename );

//make file with list of files
    char filelistname[1024]= {0};
    sprintf ( filelistname,"%s%s.fl", tmp, fnext );
    FILE *flout=fopen ( filelistname,"wt" );
    for ( int page=0; page<count; page++ ) {
        fprintf ( flout,"%s%s%05d.gpraw\n", tmp, fnext, page );
    }
    fclose ( flout );

    return 0;
}

FIMULTIBITMAP* GenericLoader ( const char* lpszPathName, int flag ) {
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

    // check the file signature and deduce its format
    // (the second argument is currently not used by FreeImage)
    fif = FreeImage_GetFileType ( lpszPathName, 0 );
    if ( fif == FIF_UNKNOWN ) {
        // no signature ?
        // try to guess the file format from the file extension
        fif = FreeImage_GetFIFFromFilename ( lpszPathName );
    }
    // check that the plugin has reading capabilities ...
    if ( ( fif != FIF_UNKNOWN ) && FreeImage_FIFSupportsReading ( fif ) ) {
        // ok, let's load the file
        //FIBITMAP *dib = FreeImage_Load(fif, lpszPathName, flag);
        BOOL bMemoryCache = TRUE;
        FIMULTIBITMAP *dib = FreeImage_OpenMultiBitmap ( fif, lpszPathName, FALSE, TRUE );
        // unless a bad file format, we are done !
        return dib;
    }
    return NULL;
}


void FreeImageErrorHandler ( FREE_IMAGE_FORMAT fif, const char *message ) {
    LOG ( "\n*** " );
    if ( fif != FIF_UNKNOWN ) {
        LOG ( "%s Format:\n", FreeImage_GetFormatFromFIF ( fif ) );
    }
    LOG ( "*** %s\n", message );
    LOG ( "***\n" );
}

//Test file exist
bool fexist ( char *fn ) {
    FILE *fe=fopen ( fn,"r" );
    bool rz = fe!=NULL;
    if ( fe ) fclose ( fe );
    return rz;
}

void do_help() {
    printf ( "Copyright(c) D.Gar'kaev aka Dickobraz.\n" );
    printf ( "Utilite to convert RedMonEE multipage tiff to gpraw format.\n" );
    printf ( "Usage: rm2gpraw <filename>\n" );
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
        case '?':
        default: {
            fprintf ( stdout, "found unknown option\n" );
            exit ( 1 );
        };
        };
        option_index = -1;
    };
}










