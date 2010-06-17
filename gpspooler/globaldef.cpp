//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "globaldef.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

TCriticalSection * csLock = new TCriticalSection();

TStringList *Config;
