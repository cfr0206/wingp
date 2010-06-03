//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmMain.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"
TfrmGPSpooler *frmGPSpooler;
//---------------------------------------------------------------------------
__fastcall TfrmGPSpooler::TfrmGPSpooler ( TComponent* Owner )
    : TForm ( Owner ),
      job_list ( NULL )
    {
    PageControl1->ActivePage = tshJobs;
    pnlJobs->DoubleBuffered = true;
    work_folder = ExtractFilePath ( Application->ExeName );
    jobs_folder = work_folder + "jobs\\";
    xml_folder = work_folder + "xml\\";
    temp_folder = work_folder + "temp\\";
    gprint_folder = work_folder;

    mmEnv->Lines->Add("GPSPOOLERJOBS\t= " + jobs_folder);
    mmEnv->Lines->Add("STP_DATA_PATH\t= " + xml_folder);
    mmEnv->Lines->Add("GPRINT_TEMP\t= " + temp_folder);
    mmEnv->Lines->Add("GPRINT_BASE\t= " + gprint_folder);
    mmEnv->Lines->Add("--------------------------------------------------------------");
    mmEnv->Lines->Add("After installation, restart the applications using gutenprint.");

    job_list = new TFileListBox ( this );
    job_list->Parent = this;
    job_list->Visible = false;

    check_jobs_folder();
    //get_job_list();
    fill_job_list();
    printers = new TStringList();
    get_printers ( printers );

    hFolderWatch = FindFirstChangeNotification((LPCTSTR) jobs_folder.c_str(),
                                                FALSE,
                                                FILE_NOTIFY_CHANGE_FILE_NAME);
    }

__fastcall TfrmGPSpooler::~TfrmGPSpooler()
    {
    FindCloseChangeNotification(hFolderWatch);
    delete printers;
    }
//---------------------------------------------------------------------------
void TfrmGPSpooler::get_job_list()
    {
    job_list->Clear();
    job_list->Directory = jobs_folder;
    job_list->Mask = "*.job";
    job_list->Update();
    }

void TfrmGPSpooler::get_printers ( TStrings * s )
    {
#define MAX_PRINTERS 256
    s->Clear();
    DWORD              dwBytesNeeded = 0, dwReturned = 0;
    PRINTER_INFO_4 *   pinfo4 = NULL;
    int                previous[MAX_PRINTERS], i = 0;
    for ( i = 0; i < MAX_PRINTERS; i++ )
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
            //errlog("EnumPrinters failed: %d\n",(int)GetLastError());
            return;
            }
        }
    HANDLE hPrinter;
    PRINTER_INFO_2 *pi2;
    DWORD cbNeeded;
    for ( i = 0; i < ( int ) dwReturned; i++ )
        {
        if ( OpenPrinter ( pinfo4[i].pPrinterName, &hPrinter, NULL ) )
            {
            GetPrinter ( hPrinter, 2, ( LPBYTE ) &pi2, sizeof ( pi2 ), &cbNeeded );
            pi2 = ( PRINTER_INFO_2 * ) LocalAlloc ( LPTR, cbNeeded );
            if ( GetPrinter ( hPrinter, 2, ( LPBYTE ) pi2, cbNeeded, &cbNeeded ) )
                {
                s->Add ( pi2->pPrinterName );
                }
            if ( pi2 )
                LocalFree ( pi2 );
            ClosePrinter ( hPrinter );
            }
        }
    }

void __fastcall TfrmGPSpooler::btnSetEnvClick ( TObject *Sender )
    {
    delete_key_env();
    make_key_env();
    }
//---------------------------------------------------------------------------

void __fastcall TfrmGPSpooler::btnDelEnvClick ( TObject *Sender )
    {
    delete_key_env();
    }
//---------------------------------------------------------------------------
void TfrmGPSpooler::delete_key_env()
    {
    TRegistry * reg = new TRegistry ( KEY_ALL_ACCESS );
    try
        {
        reg->RootKey = HKEY_LOCAL_MACHINE;
        if ( reg->OpenKey ( "\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment\\", false ) )
            ;
            {
            reg->DeleteValue ( "GPSPOOLERJOBS" );
            reg->DeleteValue ( "STP_DATA_PATH" );
            reg->DeleteValue ( "GPRINT_BASE" );
            reg->DeleteValue ( "GPRINT_TEMP" );
            reg->CloseKey();
            }
        reg->RootKey = HKEY_CURRENT_USER;
        if ( reg->OpenKey ( "\\Environment\\", false ) )
            {
            reg->DeleteValue ( "GPSPOOLERJOBS" );
            reg->DeleteValue ( "STP_DATA_PATH" );
            reg->DeleteValue ( "GPRINT_BASE" );
            reg->DeleteValue ( "GPRINT_TEMP" );
            reg->CloseKey();
            }
        }
    __finally
        {
        delete reg;
        }
    update_env();
    }

void TfrmGPSpooler::make_key_env()
    {
    TRegistry * reg = new TRegistry ( KEY_ALL_ACCESS );
    try
        {
        if ( rbSystem->Checked )
            {
            reg->RootKey = HKEY_LOCAL_MACHINE;
            if ( reg->OpenKey ( "\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", true ) )
                ;
                {
                reg->WriteString ( "GPSPOOLERJOBS", jobs_folder );
                reg->WriteString ( "STP_DATA_PATH", xml_folder );
                reg->WriteString ( "GPRINT_BASE", gprint_folder );
                reg->WriteString ( "GPRINT_TEMP", temp_folder );
                reg->CloseKey();
                }
            }
        else
            {
            reg->RootKey = HKEY_CURRENT_USER;
            if ( reg->OpenKey ( "\\Environment", true ) )
                {
                reg->WriteString ( "GPSPOOLERJOBS", jobs_folder );
                reg->WriteString ( "STP_DATA_PATH", xml_folder );
                reg->WriteString ( "GPRINT_BASE", gprint_folder );
                reg->WriteString ( "GPRINT_TEMP", temp_folder );
                reg->CloseKey();
                }
            }
        }
    __finally
        {
        delete reg;
        }
    update_env();
    }
void TfrmGPSpooler::update_env()
    {
    unsigned long dwReturnValue;
    SendMessageTimeout ( HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                         ( LPARAM ) "Environment", SMTO_ABORTIFHUNG,
                         5000, &dwReturnValue );
    }
void TfrmGPSpooler::check_jobs_folder()
    {
    if ( !DirectoryExists ( jobs_folder ) )
        CreateDir ( jobs_folder );
    if ( !DirectoryExists ( temp_folder ) )
        CreateDir ( temp_folder );
    }

void TfrmGPSpooler::fill_job_list()
    {
    //pnlJobs->DisableAutoRange();
    get_job_list();
    for ( int i = 0; i < job_list->Items->Count; i++ )
        {
        if ( get_job ( job_list->Items->operator [] ( i ) ) == NULL )
            frmJobs = new TfrmJobs ( pnlJobs, job_list->Items->operator [] ( i ) );
        frmJobs->Parent = pnlJobs;
        }
    //pnlJobs->EnableAutoRange();
    }
void TfrmGPSpooler::delete_hiden_job()
{
    TControl *tmp;
    TfrmJobs * job;
    for ( int i = pnlJobs->ControlCount - 1; i >= 0; i-- )
        {
        tmp = pnlJobs->Controls[i];
        job = dynamic_cast<TfrmJobs *> ( tmp );
        if ( job != NULL )
            {
            if ( job->Visible == false )
                delete job;
            }
        }
}

//http://sources.ru/delphi/system/jobs_information_from_printer_spooler.shtml
//http://asm.netcode.ru/cpp/?lang=&katID=6&skatID=64&artID=2714
//!http://www.tdoc.ru/c/cpp-sources/printer/kak-uznat-sostoyanie-printera-a-tak-zhe-zadanij-na-printere.html
TfrmJobs * TfrmGPSpooler::get_job ( AnsiString jn )
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
void __fastcall TfrmGPSpooler::Timer1Timer ( TObject *Sender )
    {
    fill_job_list();
    delete_hiden_job();
    }
//---------------------------------------------------------------------------


