//http://sources.ru/delphi/system/jobs_information_from_printer_spooler.shtml
//http://asm.netcode.ru/cpp/?lang=&katID=6&skatID=64&artID=2714
//!http://www.tdoc.ru/c/cpp-sources/printer/kak-uznat-sostoyanie-printera-a-tak-zhe-zadanij-na-printere.html
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmMain.h"
#include "globaldef.h"
#include "uLogs.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"

TfrmGPSpooler *frmGPSpooler;
//---------------------------------------------------------------------------

__fastcall TfrmGPSpooler::TfrmGPSpooler ( TComponent* Owner )
    : TForm ( Owner ),
      job_list ( NULL ),
      ForcedClosure(false)
{
    PageControl1->ActivePage = tshJobs;
    pnlJobs->DoubleBuffered = true;

    ReadConfigParam();

    exe_name = Application->ExeName;
    work_folder = ExtractFilePath ( exe_name );
    jobs_folder = work_folder + "jobs\\";
    xml_folder = work_folder + "xml\\";
    temp_folder = work_folder + "temp\\";
    exe_name_gprint = work_folder+"gprint.exe";

    mmEnv->Lines->Add("Set Environment Variables:");
    mmEnv->Lines->Add("--------------------------------------------------------------");
    mmEnv->Lines->Add("GPSPOOLER\t= " + exe_name);
    mmEnv->Lines->Add("GPSPOOLERJOBS\t= " + jobs_folder);
    mmEnv->Lines->Add("STP_DATA_PATH\t= " + xml_folder);
    mmEnv->Lines->Add("GPRINT_TEMP\t= " + temp_folder);
    mmEnv->Lines->Add("GPRINT\t\t= " + exe_name_gprint);
    mmEnv->Lines->Add("GPRINT_OPT\t= " + AnsiString(getenv( "GPRINT_OPT" )));
    AnsiString as = AnsiString(getenv( "GPRINT_OPT" ));
    Edit1->Text = AnsiString(getenv( "GPRINT_OPT" ));
    mmEnv->Lines->Add("--------------------------------------------------------------");
    mmEnv->Lines->Add("After installation, restart the applications using gutenprint.");

    LogEnv();

    job_list = new TFileListBox ( this );
    job_list->Parent = this;
    job_list->Visible = false;

    CheckJobFolder();

    printers = new TStringList();
    GetPrinterList ( printers );

    job_list->Clear();
    job_list->Directory = jobs_folder;
    job_list->Mask = "*.job";

    UpdateJobList();
}
//---------------------------------------------------------------------------

__fastcall TfrmGPSpooler::~TfrmGPSpooler()
{
    delete printers;
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::ReadJobFolder()
{
    job_list->Update();
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::GetPrinterList ( TStrings * s )
{
#define MAX_PRINTERS 256
    s->Clear();
    DWORD              dwBytesNeeded = 0, dwReturned = 0;
    PRINTER_INFO_4 *   pinfo4 = NULL;
    int                previous[MAX_PRINTERS];
    for ( int i = 0; i < MAX_PRINTERS; i++ )
    {
        previous[i] = 0;
    }
    while ( !EnumPrinters ( PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL,
                            4, ( LPBYTE ) pinfo4, dwBytesNeeded, &dwBytesNeeded,
                            &dwReturned ) )
    {
        if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
        {
            if ( pinfo4 )
                LocalFree ( pinfo4 );
            pinfo4 = ( PRINTER_INFO_4 * ) LocalAlloc ( LPTR, dwBytesNeeded );
        }
        else
        {
            ToLog(FL, AnsiString().printf("EnumPrinters failed: %d\n",(int)GetLastError()));
            return;
        }
    }
    HANDLE hPrinter;
    PRINTER_INFO_2 *pi2;
    DWORD cbNeeded;

    ToLog(FL,"Found Printers:");
    for ( int i = 0; i < ( int ) dwReturned; i++ )
    {
        if ( OpenPrinter ( pinfo4[i].pPrinterName, &hPrinter, NULL ) )
        {
            GetPrinter ( hPrinter, 2, ( LPBYTE ) &pi2, sizeof ( pi2 ), &cbNeeded );
            pi2 = ( PRINTER_INFO_2 * ) LocalAlloc ( LPTR, cbNeeded );
            if ( GetPrinter ( hPrinter, 2, ( LPBYTE ) pi2, cbNeeded, &cbNeeded ) )
            {
                s->Add ( pi2->pPrinterName );
                ToLog(FL,"\t* "+AnsiString(pi2->pPrinterName));
            }
            if ( pi2 )
                LocalFree ( pi2 );
            ClosePrinter ( hPrinter );
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmGPSpooler::btnSetEnvClick ( TObject *Sender )
{
    DeleteKeyEnv();
    MakeKeyEnv();
}
//---------------------------------------------------------------------------

void __fastcall TfrmGPSpooler::btnDelEnvClick ( TObject *Sender )
{
    DeleteKeyEnv();
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::DeleteKeyEnv()
{
    TRegistry * reg = new TRegistry ( KEY_ALL_ACCESS );
    try
    {
        reg->RootKey = HKEY_LOCAL_MACHINE;
        if ( reg->OpenKey ( "\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment\\", false ) )
            ;
        {
            reg->DeleteValue ( "GPSPOOLER" );
            reg->DeleteValue ( "GPSPOOLERJOBS" );
            reg->DeleteValue ( "STP_DATA_PATH" );
            reg->DeleteValue ( "GPRINT" );
            reg->DeleteValue ( "GPRINT_OPT" );
            reg->DeleteValue ( "GPRINT_TEMP" );
            reg->CloseKey();
        }
        reg->RootKey = HKEY_CURRENT_USER;
        if ( reg->OpenKey ( "\\Environment\\", false ) )
        {
            reg->DeleteValue ( "GPSPOOLER" );
            reg->DeleteValue ( "GPSPOOLERJOBS" );
            reg->DeleteValue ( "STP_DATA_PATH" );
            reg->DeleteValue ( "GPRINT" );
            reg->DeleteValue ( "GPRINT_OPT" );
            reg->DeleteValue ( "GPRINT_TEMP" );
            reg->CloseKey();
        }
    }
    __finally
    {
        delete reg;
    }
    MsgUpdateEnv();
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::MakeKeyEnv()
{
    TRegistry * reg = new TRegistry ( KEY_ALL_ACCESS );
    try
    {
        if ( rbSystem->Checked )
        {
            reg->RootKey = HKEY_LOCAL_MACHINE;
            if ( reg->OpenKey ( "\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", true ) )
            {
                reg->WriteString ( "GPSPOOLER", exe_name );
                reg->WriteString ( "GPSPOOLERJOBS", jobs_folder );
                reg->WriteString ( "STP_DATA_PATH", xml_folder );
                reg->WriteString ( "GPRINT", exe_name_gprint );
                reg->WriteString ( "GPRINT_OPT", Edit1->Text );
                reg->WriteString ( "GPRINT_TEMP", temp_folder );
                reg->CloseKey();
            }
        }
        else
        {
            reg->RootKey = HKEY_CURRENT_USER;
            if ( reg->OpenKey ( "\\Environment", true ) )
            {
                reg->WriteString ( "GPSPOOLER", exe_name );
                reg->WriteString ( "GPSPOOLERJOBS", jobs_folder );
                reg->WriteString ( "STP_DATA_PATH", xml_folder );
                reg->WriteString ( "GPRINT", exe_name_gprint );
                reg->WriteString ( "GPRINT_OPT", Edit1->Text  );
                reg->WriteString ( "GPRINT_TEMP", temp_folder );
                reg->CloseKey();
            }
        }
    }
    __finally
    {
        delete reg;
    }
    MsgUpdateEnv();
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::MsgUpdateEnv()
{
    unsigned long dwReturnValue;
    SendMessageTimeout ( HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                         ( LPARAM ) "Environment", SMTO_ABORTIFHUNG,
                         5000, &dwReturnValue );
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::CheckJobFolder()
{
    if ( !DirectoryExists ( jobs_folder ) )
    {
        CreateDir ( jobs_folder );
        ToLog(FL,"CreateDir("+jobs_folder+")");
    }
    else
        ToLog(FL,"DirectoryExists: "+jobs_folder);

    if ( !DirectoryExists ( temp_folder ) )
    {
        CreateDir ( temp_folder );
        ToLog(FL,"CreateDir("+temp_folder +")");
    }
    else
        ToLog(FL,"DirectoryExists: "+temp_folder);
}
//---------------------------------------------------------------------------

TfrmJobs * TfrmGPSpooler::GetJobFrame ( AnsiString jn )
{
    TControl *tmp;
    TfrmJobs * job;
    for ( int i = pnlJobs->ControlCount - 1; i >= 0; i-- )
    {
        tmp = pnlJobs->Controls[i];
        job = dynamic_cast<TfrmJobs *> ( tmp );
        if ( job != NULL )
        {
            if ( job->get_job_name() == jn )
                return job;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::UpdateJobList()
{
    ReadJobFolder();
    for ( int i = 0; i < job_list->Items->Count; i++ )
    {
        if ( GetJobFrame ( job_list->Items->operator [] ( i ) ) == NULL )
        {
            AddJob(job_list->Items->operator [] ( i ) );
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmGPSpooler::IPCServerClientRead(TObject *Sender,
        TCustomWinSocket *Socket)
{
    AnsiString job=Socket->ReceiveText();
    AddJob(job);
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::AddJob(AnsiString job)
{
    csLock->Enter();
    ToLog(FL,"Add Job: "+job);
    try
    {
        frmJobs = new TfrmJobs ( pnlJobs, job );
        frmJobs->Parent = pnlJobs;
    }
    __finally
    {
        csLock->Leave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmGPSpooler::cbMinOnExitClick(TObject *Sender)
{
    Config->Values[MINTOEXIT]=cbMinOnExit->Checked?"true":"false";
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::ReadConfigParam()
{
    cbMinOnExit->Checked = StrToBoolDef(Config->Values[MINTOEXIT],false);
}
void __fastcall TfrmGPSpooler::FormCloseQuery(TObject *Sender,
        bool &CanClose)
{
    if (ForcedClosure)
    {
        CanClose = true;
        return;
    }
    if (cbMinOnExit->Checked)
    {
        CanClose = false;
        TrayIcon->Minimize();
    }
    else
        CanClose = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmGPSpooler::cmdExitClick(TObject *Sender)
{
    ForcedClosure=true;
    Close();
}
//---------------------------------------------------------------------------

void TfrmGPSpooler::LogEnv()
{
ToLog(FL, "");
ToLog(FL, "Environment variables:");
ToLog(FL, "GPSPOOLER     = " + AnsiString(getenv( "GPSPOOLER" )));
ToLog(FL, "GPSPOOLERJOBS = " + AnsiString(getenv( "GPSPOOLERJOBS" )));
ToLog(FL, "STP_DATA_PATH = " + AnsiString(getenv( "STP_DATA_PATH" )));
ToLog(FL, "GPRINT        = " + AnsiString(getenv( "GPRINT" )));
ToLog(FL, "GPRINT_OPT    = " + AnsiString(getenv( "GPRINT_OPT" )));
ToLog(FL, "GPRINT_TEMP   = " + AnsiString(getenv( "GPRINT_TEMP" )));
ToLog(FL, "");
}
