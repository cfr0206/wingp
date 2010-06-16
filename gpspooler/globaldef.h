//---------------------------------------------------------------------------

#ifndef globaldefH
#define globaldefH

#include <SyncObjs.hpp>
//---------------------------------------------------------------------------
extern TCriticalSection * csLock;
extern TListBox * log;

void LogM(AnsiString s);
void LogE(AnsiString s);


#endif
