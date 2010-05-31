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
#include <FileCtrl.hpp>

//---------------------------------------------------------------------------
class TfrmGPSpooler : public TForm
{
__published:	// IDE-managed Components
    TTrayIcon *TrayIcon1;
    TPageControl *PageControl1;
        TTabSheet *tshJobs;
    TTabSheet *TabSheet2;
    TGroupBox *GroupBox1;
    TButton *btnSetEnv;
    TRadioButton *rbCurrentUser;
    TRadioButton *rbSystem;
    TButton *btnDelEnv;
        TPanel *Panel1;
        TButton *btnUpdateJobList;
        TScrollBox *pnlJobs;
    void __fastcall btnSetEnvClick(TObject *Sender);
    void __fastcall btnDelEnvClick(TObject *Sender);
        void __fastcall btnUpdateJobListClick(TObject *Sender);
protected:	// User declarations
    AnsiString work_folder;
    AnsiString jobs_folder;
    TStringList *printers;
    TFileListBox *job_list;

    void check_jobs_folder();
    void get_printers(TStrings *);
    void delete_key_env();
    void make_key_env();
    void get_job_list();
    void fill_job_list();

public:		// User declarations
    __fastcall TfrmGPSpooler(TComponent* Owner);
    __fastcall ~TfrmGPSpooler();    
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmGPSpooler *frmGPSpooler;
//---------------------------------------------------------------------------
#endif
