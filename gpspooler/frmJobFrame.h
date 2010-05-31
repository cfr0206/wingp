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
        void __fastcall btnSendToPrinterClick(TObject *Sender);
        void __fastcall edCopiesKeyPress(TObject *Sender, char &Key);
private:	// User declarations
        TStringList *param_list;
        bool send_to_printer();
        AnsiString name, file, printer;
        int copies;
        int get_user_copies();
public:		// User declarations
        static int count;
        __fastcall TfrmJobs(TComponent* Owner, AnsiString fn);
        __fastcall ~TfrmJobs();
};
int TfrmJobs::count=0;
//---------------------------------------------------------------------------
extern PACKAGE TfrmJobs *frmJobs;
//---------------------------------------------------------------------------
#endif
