#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>


int main(int argc, char *argv[])
{
struct _timeb timebuffer;
_ftime( &timebuffer );
fprintf(stdout, "%ld%03u",time(NULL),timebuffer.millitm);
return 0;
}
