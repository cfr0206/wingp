//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "globaldef.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TCriticalSection * csLock = new TCriticalSection();
TListBox * log=NULL;

void LogM(AnsiString s)
{
AnsiString ss;
ss="[M]{"+DateTimeToStr(Now())+"} "+s;
log->ItemIndex=log->Items->Add(ss);
}

void LogE(AnsiString s)
{
AnsiString ss;
ss="[E]{"+DateTimeToStr(Now())+"} "+s;
log->ItemIndex=log->Items->Add(ss);
}
