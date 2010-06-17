//---------------------------------------------------------------------------


#ifndef frmJobFrameH
#define frmJobFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmJobs : public TFrame
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TPanel *Panel2;
    TPanel *Panel3;
    TLabel *lbJobName;
    TLabel *lbPrinterName;
    TBitBtn *btnSendToPrinter;
    TBitBtn *BitBtn2;
    TBitBtn *BitBtn1;
    TLabeledEdit *edCopies;
    TUpDown *udCopies;
    TCheckBox *chbDelAfterPrinting;
    TProgressBar *pbPrinting;
    void __fastcall btnSendToPrinterClick(TObject *Sender);
    void __fastcall edCopiesKeyPress(TObject *Sender, char &Key);
    void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
    static int count;
    TStringList *param_list;
    bool send_to_printer();
    AnsiString name, file, printer, job;
    int copies;
    int get_user_copies();
    int fin,fin_job;
    void lock_file();
    void unlock_file();
public:		// User declarations
    AnsiString get_job_name();
    __fastcall TfrmJobs(TComponent* Owner, AnsiString fn);
    __fastcall ~TfrmJobs();

};
int TfrmJobs::count=0;
//---------------------------------------------------------------------------
extern PACKAGE TfrmJobs *frmJobs;
//---------------------------------------------------------------------------
#endif
