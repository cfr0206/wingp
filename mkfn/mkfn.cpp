#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>


int main(int argc, char *argv[])
{
char fn[1024]={0};
char *argv0=argv[0];


struct _timeb timebuffer;
_ftime( &timebuffer );
fprintf(stdout, "RME%ld%03u",time(NULL),timebuffer.millitm);

return 0;
}
