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
__fastcall TfrmGPSpooler::TfrmGPSpooler(TComponent* Owner)
        : TForm(Owner)
{
PageControl1->ActivePage=tshJobs;
    work_folder=ExtractFilePath(Application->ExeName);
    jobs_folder=work_folder+"jobs\\";

    check_jobs_folder();
    get_printers(lbPrinterList->Items);
}
//---------------------------------------------------------------------------
void __fastcall TfrmGPSpooler::btnRefreshClick(TObject *Sender)
{
    lbPrinterList->Clear();
    get_printers(lbPrinterList->Items);
}
//---------------------------------------------------------------------------
void TfrmGPSpooler::get_printers(TStrings * s)
{
#define MAX_PRINTERS 256
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

