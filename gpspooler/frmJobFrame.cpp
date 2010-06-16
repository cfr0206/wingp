//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "frmJobFrame.h"
#include "globaldef.h"
#include <ctype.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmJobs *frmJobs;

//---------------------------------------------------------------------------
__fastcall TfrmJobs::TfrmJobs ( TComponent* Owner, AnsiString fn )
    : TFrame ( Owner )
    {
    count++;
    Name += AnsiString ( count );
    Top = count * Height;
    Align = alTop;

    param_list = new TStringList();
    param_list->LoadFromFile ( fn );
    lbJobName->Caption = "file: " +  Utf8ToAnsi ( param_list->Values["name"] + "   " );
    udCopies->Position = param_list->Values["copies"].ToIntDef ( 1 );
    lbPrinterName->Caption = "printer: " + param_list->Values["printer"];
    file = param_list->Values["file"];
    printer = param_list->Values["printer"];
    copies = param_list->Values["copies"].ToIntDef ( 1 );
    name = param_list->Values["name"];
    job = fn;

    LogM("Job parametr:");
    LogM("file: " +  Utf8ToAnsi ( param_list->Values["name"] + "   " ));
    LogM("printer: " + param_list->Values["printer"]);
    LogM("copies: "+param_list->Values["copies"]);

    lock_file();
    
    }
__fastcall TfrmJobs::~TfrmJobs()
    {
    unlock_file();
    delete param_list;
    }
//---------------------------------------------------------------------------
void TfrmJobs::lock_file()
    {
    fin = FileOpen ( file, fmOpenRead | fmShareExclusive );
    if (fin==-1)
      LogE("Lock file error: "+file);
      else
        LogM("Lock file successfully: "+file);

    fin_job = FileOpen ( job, fmOpenRead | fmShareExclusive );
    if (fin_job==-1)
      LogE("Lock file error: "+job);
      else
        LogM("Lock file successfully: "+job);

    }

void TfrmJobs::unlock_file()
    {
    if ( fin != -1 )
        {
        FileClose ( fin );
        fin = -1;
        LogM("Unlock file "+file);
        }
    if ( fin_job != -1 )
        {
        FileClose ( fin_job );
        fin_job = -1;
        LogM("Unlock file "+job);
        }
    }

void __fastcall TfrmJobs::btnSendToPrinterClick ( TObject *Sender )
    {
    int ucopies = get_user_copies();
    for ( int i = 0; i < ucopies; i++ )
        send_to_printer();
    if ( chbDelAfterPrinting->Checked )
        {
        unlock_file();
        DeleteFile ( file + ".job" );
        DeleteFile ( file );
        Visible=false;
        }
    }
//---------------------------------------------------------------------------
bool TfrmJobs::send_to_printer()
    {
    HANDLE     hPrinter;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob;
    if ( ! OpenPrinter ( ( LPTSTR ) printer.c_str(), &hPrinter, NULL ) )
        {
         return false;
        }
     // Fill in the structure with info about this "document."
    DocInfo.pDocName = ( LPTSTR ) Utf8ToAnsi ( name + "   " ).data(); //HACK:Utf8ToAnsi cut string
    DocInfo.pOutputFile = NULL;
    DocInfo.pDatatype = "RAW";
    // Inform the spooler the document is beginning.
    if ( ( dwJob = StartDocPrinter ( hPrinter, 1, ( LPSTR ) &DocInfo ) ) == 0 )
        {
        ClosePrinter ( hPrinter );
         return FALSE;
        }
     // Start a page.
    if ( ! StartPagePrinter ( hPrinter ) )
        {
        EndDocPrinter ( hPrinter );
        ClosePrinter ( hPrinter );
         return FALSE;
        }
    if ( fin == -1 )
        {
        EndDocPrinter ( hPrinter );
        ClosePrinter ( hPrinter );
         return FALSE;
        }
    FileSeek(fin, 0, 0);

    int buf_len = 65536;
    char *buf = new char[buf_len];
    int read_count;
    unsigned long bytes_written;
    while ( true )
        {
        read_count = FileRead ( fin, buf, buf_len );
        if ( read_count == 0 )
            break;
        WritePrinter ( hPrinter, ( LPVOID ) buf, read_count, &bytes_written );
        }
    // End the page.
    EndPagePrinter ( hPrinter );
    // Inform the spooler that the document is ending.
    EndDocPrinter ( hPrinter );
    ClosePrinter ( hPrinter );
    delete buf;
    return true;
    }

void __fastcall TfrmJobs::edCopiesKeyPress ( TObject *Sender, char &Key )
    {
    if ( ( unsigned char ) Key > '9' )
        Key = 0;
    }
//---------------------------------------------------------------------------
int TfrmJobs::get_user_copies()
    {
    return udCopies->Position;
    }
void __fastcall TfrmJobs::BitBtn1Click ( TObject *Sender )
    {
    csLock->Enter();
try
{
    unlock_file();
    if (DeleteFile ( file + ".job" ))
        LogM("File "+file+".job deleted successfully.");
       else
       LogE("File "+file+".job don't deleted.");

    if (DeleteFile ( file ))
        LogM("File "+file+" deleted successfully.");
       else
       LogE("File "+file+" don't deleted.");

    delete this;
    }
    __finally
    {
    csLock->Leave();

    }
    }
//---------------------------------------------------------------------------

AnsiString TfrmJobs::get_job_name()
    {
    return job;
    }
