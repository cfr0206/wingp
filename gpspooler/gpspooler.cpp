//---------------------------------------------------------------------------

#include <vcl.h>
#include <ScktComp.hpp>
#pragma hdrstop
#include <windows.h>
//---------------------------------------------------------------------------
USEFORM("frmMain.cpp", frmGPSpooler);
USEFORM("frmJobFrame.cpp", frmJobs); /* TFrame: File Type */
//---------------------------------------------------------------------------
WINAPI WinMain ( HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
    {
    HANDLE hMutex = CreateMutex(NULL, FALSE, "GPSPOOLER");
    if (WaitForSingleObject(hMutex,0)== WAIT_OBJECT_0)
    {
    try
        {
        Application->Initialize();
        Application->CreateForm(__classid(TfrmGPSpooler), &frmGPSpooler);
        Application->Run();
        }
    catch ( Exception &exception )
        {
        Application->ShowException ( &exception );
        }
    catch ( ... )
        {
        try
            {
            throw Exception ( "" );
            }
        catch ( Exception &exception )
            {
            Application->ShowException ( &exception );
            }
        }
    ReleaseMutex(hMutex);    
    }
    else //send job name
    {
        TClientSocket *IPCClient = new TClientSocket(NULL);
        IPCClient->Port=37051;
        IPCClient->Address="127.0.0.1";
        IPCClient->ClientType = ctBlocking;
        IPCClient->Active=true;
        IPCClient->Socket->SendText(lpCmdLine);
        IPCClient->Close();
    }
    CloseHandle(hMutex);
    return 0;
    }
//---------------------------------------------------------------------------
