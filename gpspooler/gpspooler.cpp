//---------------------------------------------------------------------------

#include <vcl.h>
#include <ScktComp.hpp>
#pragma hdrstop
#include <windows.h>
#include "uLogs.hpp"
#include "globaldef.h"

using namespace Ulogs;
//---------------------------------------------------------------------------
USEFORM("frmMain.cpp", frmGPSpooler);
USEFORM("frmJobFrame.cpp", frmJobs); /* TFrame: File Type */
//---------------------------------------------------------------------------

void InitLogingSystem()
{
    AnsiString log_folder = ExtractFilePath ( Application->ExeName )+"log\\";
    if ( !DirectoryExists ( log_folder ) )
        CreateDir ( log_folder );

    AnsiString logfilename;
    TDateTime dt = Now();
    AnsiString result;
    DateTimeToString(result, "yyyymmddhhnnsszzz", dt);
    logfilename=log_folder+result+".log";
    TFileLog * FileLog = new TFileLog(TChannels()<<FL,logfilename);
    SetLog(FileLog);
}

void SendNewJobName(AnsiString jobname)
{
    TClientSocket *IPCClient = new TClientSocket(NULL);
    IPCClient->Port=37051;
    IPCClient->Address="127.0.0.1";
    IPCClient->ClientType = ctBlocking;
    IPCClient->Active=true;
    IPCClient->Socket->SendText(jobname);
    IPCClient->Close();
}

void ReadConfig()
{
    AnsiString config_filename = ExtractFilePath ( Application->ExeName )+"gpspooler.cfg";
    Config = new TStringList();
    try
    {
        Config->LoadFromFile(config_filename);
    }
    catch(EFOpenError *oe)
    {
        ToLog(FL,oe->Message);
    }
}

void SaveConfig()
{
    AnsiString config_filename = ExtractFilePath ( Application->ExeName )+"gpspooler.cfg";
    Config->SaveToFile(config_filename);
}

WINAPI WinMain ( HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
    HANDLE hMutex = CreateMutex(NULL, FALSE, "GPSPOOLER");
    if (WaitForSingleObject(hMutex,0)== WAIT_OBJECT_0)
    {
        InitLogingSystem();
        ReadConfig();
        try
        {
            Application->Initialize();
            Application->CreateForm(__classid(TfrmGPSpooler), &frmGPSpooler);
            Application->Run();
            SaveConfig();
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
        SendNewJobName(lpCmdLine);
    }
    CloseHandle(hMutex);
    return 0;
}
//---------------------------------------------------------------------------
