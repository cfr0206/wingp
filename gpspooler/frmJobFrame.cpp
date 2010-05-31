//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmJobFrame.h"
#include <ctype.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmJobs *frmJobs;

//---------------------------------------------------------------------------
__fastcall TfrmJobs::TfrmJobs(TComponent* Owner, AnsiString fn)
    : TFrame(Owner)
{
    count++;
    Name+=AnsiString(count);
    Top=count*Height;
    Align=alTop;

    param_list = new TStringList();
    param_list->LoadFromFile(fn);
    lbJobName->Caption="file: "+param_list->Values["name"];
    //lbCopies->Caption="copies: "+param_list->Values["copies"];
    udCopies->Position=param_list->Values["copies"].ToIntDef(1);
    lbPrinterName->Caption="printer: "+param_list->Values["printer"];

    file=param_list->Values["file"];
    printer=param_list->Values["printer"];
    copies=param_list->Values["copies"].ToIntDef(1);
    name=param_list->Values["name"];
}
__fastcall TfrmJobs::~TfrmJobs()
{
    delete param_list;
}
//---------------------------------------------------------------------------

void __fastcall TfrmJobs::btnSendToPrinterClick(TObject *Sender)
{
int ucopies=get_user_copies();
for(int i=0;i<ucopies;i++);
            send_to_printer();
if (chbDelAfterPrinting->Checked)
        {
        DeleteFile(file);
        DeleteFile(file+".job");
        delete this;
        }
}
//---------------------------------------------------------------------------
bool TfrmJobs::send_to_printer()
{
    HANDLE     hPrinter;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob;

    // Need a handle to the printer.
    if( ! OpenPrinter( (LPTSTR)printer.c_str(), &hPrinter, NULL ) )
    {
//            DBG("ERROR: OpenPrinter");
        return false;
    }
//        DBG("OK: OpenPrinter");

    // Fill in the structure with info about this "document."
    DocInfo.pDocName = (LPTSTR)name.c_str();
    DocInfo.pOutputFile = NULL;
    DocInfo.pDatatype = "RAW";
    // Inform the spooler the document is beginning.
    if( (dwJob = StartDocPrinter( hPrinter, 1, (LPSTR)&DocInfo )) == 0 )
    {
        ClosePrinter( hPrinter );
        //DBG("ERROR: StartDocPrinter");
        return FALSE;
    }
    //DBG("OK: StartDocPrinter");
    // Start a page.
    if( ! StartPagePrinter( hPrinter ) )
    {
        EndDocPrinter( hPrinter );
        ClosePrinter( hPrinter );
        //DBG("ERROR: StartPagePrinter");
        return FALSE;
    }
    int fin=FileOpen(file,fmOpenRead);
    if(fin==-1)
    {
        EndDocPrinter( hPrinter );
        ClosePrinter( hPrinter );
        //DBG("ERROR: StartPagePrinter");
        return FALSE;
    }

    int buf_len=65536;
    char *buf =new char[buf_len];
    int read_count;
    unsigned long bytes_written;
    while(true)
    {
        read_count=FileRead(fin, buf, buf_len);
        if (read_count==0) break;
        WritePrinter( hPrinter, (LPVOID)buf, read_count, &bytes_written );
    }

    // End the page.
    if( ! EndPagePrinter( hPrinter ) )
    {
        EndDocPrinter( hPrinter );
        ClosePrinter( hPrinter );
        return FALSE;
    }
    // Inform the spooler that the document is ending.
    if( ! EndDocPrinter( hPrinter ) )
    {
        ClosePrinter( hPrinter );
        return FALSE;
    }
    // Tidy up the printer handle.
    ClosePrinter( hPrinter );

    FileClose(fin);
    delete buf;
    return true;
}

void __fastcall TfrmJobs::edCopiesKeyPress(TObject *Sender, char &Key)
{
if ((unsigned char)Key>'9') Key=0;
}
//---------------------------------------------------------------------------
int TfrmJobs::get_user_copies()
{
 return udCopies->Position;
}
