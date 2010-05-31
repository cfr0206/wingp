//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmMain.h"
#include "frmJobFrame.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"
TfrmGPSpooler *frmGPSpooler;
//---------------------------------------------------------------------------
__fastcall TfrmGPSpooler::TfrmGPSpooler(TComponent* Owner)
    : TForm(Owner),
      job_list(NULL)
{
    PageControl1->ActivePage=tshJobs;
    work_folder=ExtractFilePath(Application->ExeName);
    jobs_folder=work_folder+"jobs\\";

    check_jobs_folder();
    get_job_list();
    fill_job_list();

    printers = new TStringList();
    get_printers(printers);

}

__fastcall TfrmGPSpooler::~TfrmGPSpooler()
{
    delete printers;
}
//---------------------------------------------------------------------------
void TfrmGPSpooler::get_job_list()
{
    if (frmJobs)//delete job list
    {
        delete job_list;
        job_list=NULL;
    }
    job_list = new TFileListBox(this);
    job_list->Parent=this;
    job_list->Visible=false;
    job_list->Directory=jobs_folder;
    job_list->Mask="*.job";
    job_list->Update();
    for (int i=0; i<job_list->Items->Count; i++)
    {
        AnsiString fn=job_list->Items->operator [](i);
        TStringList *sl=new TStringList();
        sl->LoadFromFile(fn);
        job_list->Items->Objects[i]=sl;
    }
}

void TfrmGPSpooler::get_printers(TStrings * s)
{
#define MAX_PRINTERS 256

    s->Clear();

    DWORD              dwBytesNeeded=0,dwReturned=0;
    PRINTER_INFO_4 *   pinfo4 = NULL;
    int                previous[MAX_PRINTERS], i =0;

    for (i=0; i<MAX_PRINTERS; i++) {
        previous[i]=0;
    }
    while ( !EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL,
                           4, (LPBYTE) pinfo4, dwBytesNeeded, &dwBytesNeeded,
                           &dwReturned) )
    {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            if (pinfo4) LocalFree (pinfo4);
            pinfo4 = (PRINTER_INFO_4 *) LocalAlloc (LPTR, dwBytesNeeded);
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
    for (i = 0; i < (int)dwReturned; i++)
    {
        if (OpenPrinter(pinfo4[i].pPrinterName, &hPrinter, NULL))
        {
            GetPrinter(hPrinter,2,(LPBYTE)&pi2,sizeof(pi2),&cbNeeded);
            pi2=(PRINTER_INFO_2 *)LocalAlloc(LPTR,cbNeeded);
            if (GetPrinter(hPrinter,2,(LPBYTE)pi2,cbNeeded,&cbNeeded))
            {
                s->Add(pi2->pPrinterName);
            }
            if (pi2) LocalFree(pi2);

            ClosePrinter(hPrinter);
        }
    }

}

void __fastcall TfrmGPSpooler::btnSetEnvClick(TObject *Sender)
{
    delete_key_env();
    make_key_env();
}
//---------------------------------------------------------------------------

void __fastcall TfrmGPSpooler::btnDelEnvClick(TObject *Sender)
{
    delete_key_env();
}
//---------------------------------------------------------------------------
void TfrmGPSpooler::delete_key_env()
{
    TRegistry * reg = new TRegistry(KEY_ALL_ACCESS);
    try
    {

        reg->RootKey = HKEY_LOCAL_MACHINE;
        if (reg->OpenKey("\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment\\", false));
        {
            reg->DeleteValue("GPSPOOLERJOBS");
            reg->CloseKey();
        }

        reg->RootKey = HKEY_CURRENT_USER;
        if (reg->OpenKey("\\Environment\\", false))
        {
            reg->DeleteValue("GPSPOOLERJOBS");
            reg->CloseKey();
        }
    }
    __finally
    {
        delete reg;
    }
}

void TfrmGPSpooler::make_key_env()
{
    TRegistry * reg = new TRegistry(KEY_ALL_ACCESS);
    try
    {
        if (rbSystem->Checked)
        {
            reg->RootKey = HKEY_LOCAL_MACHINE;
            if (reg->OpenKey("\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", true));
            {
                reg->WriteString("GPSPOOLERJOBS",jobs_folder);
                reg->CloseKey();
            }
        }
        else
        {
            reg->RootKey = HKEY_CURRENT_USER;
            if (reg->OpenKey("\\Environment", true))
            {
                reg->WriteString("GPSPOOLERJOBS",jobs_folder);
                reg->CloseKey();
            }
        }
    }
    __finally
    {
        delete reg;
    }
}

void TfrmGPSpooler::check_jobs_folder()
{
    if (!DirectoryExists(jobs_folder))
        CreateDir(jobs_folder);
}

void __fastcall TfrmGPSpooler::btnUpdateJobListClick(TObject *Sender)
{
    fill_job_list();
}
//---------------------------------------------------------------------------
void TfrmGPSpooler::fill_job_list()
{
//delete all JobFrames
    TControl *tmp;
    for(int i=pnlJobs->ControlCount-1; i>=0; i--)
    {
        tmp = pnlJobs->Controls[i];
        if (dynamic_cast<TfrmJobs *>(tmp) != NULL)
        {
            delete (TfrmJobs *)tmp;
        }
    }


    for(int i=0; i<job_list->Items->Count; i++)
    {
        frmJobs = new TfrmJobs(pnlJobs, job_list->Items->operator [](i));
        frmJobs->Parent=pnlJobs;
    }

}

