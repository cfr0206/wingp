//---------------------------------------------------------------------------

#ifndef frmMainH
#define frmMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "trayicon.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Registry.hpp>

//---------------------------------------------------------------------------
class TfrmGPSpooler : public TForm
{
__published:	// IDE-managed Components
    TTrayIcon *TrayIcon1;
    TPageControl *PageControl1;
        TTabSheet *tshJobs;
    TTabSheet *TabSheet2;
    TGroupBox *Printers;
    TListBox *lbPrinterList;
    TPanel *Panel1;
    TButton *btnRefresh;
    TGroupBox *GroupBox1;
    TButton *btnSetEnv;
    TRadioButton *rbCurrentUser;
    TRadioButton *rbSystem;
    TButton *btnDelEnv;
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall btnSetEnvClick(TObject *Sender);
    void __fastcall btnDelEnvClick(TObject *Sender);
protected:	// User declarations
    AnsiString work_folder;
    AnsiString jobs_folder;

    void check_jobs_folder();
    void get_printers(TStrings *);
    void delete_key_env();
    void make_key_env();
public:		// User declarations
    __fastcall TfrmGPSpooler(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmGPSpooler *frmGPSpooler;
//---------------------------------------------------------------------------
#endif
