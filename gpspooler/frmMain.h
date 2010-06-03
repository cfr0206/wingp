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
#include "frmJobFrame.h"
//---------------------------------------------------------------------------
class TfrmGPSpooler : public TForm
{
__published:	// IDE-managed Components
    TTrayIcon *TrayIcon1;
    TPageControl *PageControl1;
        TTabSheet *tshJobs;
    TTabSheet *TabSheet2;
    TGroupBox *GroupBox1;
        TPanel *Panel1;
        TScrollBox *pnlJobs;
        TPanel *Panel3;
        TButton *btnSetEnv;
        TButton *btnDelEnv;
        TRadioButton *rbSystem;
        TRadioButton *rbCurrentUser;
        TMemo *mmEnv;
        TTimer *Timer1;
    void __fastcall btnSetEnvClick(TObject *Sender);
    void __fastcall btnDelEnvClick(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
protected:	// User declarations
    AnsiString work_folder;
    AnsiString jobs_folder;
    AnsiString xml_folder;
    AnsiString temp_folder;
    AnsiString gprint_folder;    
    TStringList *printers;
    TFileListBox *job_list;

    HANDLE hFolderWatch;

    void check_jobs_folder();
    void get_printers(TStrings *);
    void delete_key_env();
    void make_key_env();
    void get_job_list();
    void fill_job_list();
    void delete_hiden_job();
    TfrmJobs * get_job(AnsiString jn);
    void update_env();
public:		// User declarations
    __fastcall TfrmGPSpooler(TComponent* Owner);
    __fastcall ~TfrmGPSpooler();    
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmGPSpooler *frmGPSpooler;
//---------------------------------------------------------------------------
#endif
