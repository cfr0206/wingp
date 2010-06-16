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
#include <ScktComp.hpp>

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
        TServerSocket *IPCServer;
        TSplitter *Splitter1;
        TListBox *lbLog;
    void __fastcall btnSetEnvClick(TObject *Sender);
    void __fastcall btnDelEnvClick(TObject *Sender);

        void __fastcall IPCServerClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
protected:	// User declarations
    AnsiString exe_name;
    AnsiString work_folder;
    AnsiString jobs_folder;
    AnsiString xml_folder;
    AnsiString temp_folder;
    AnsiString exe_name_gprint;    
    TStringList *printers;
    TFileListBox *job_list;

    //TWatchFolder *WatchFolder;

    void CheckJobFolder();
    void GetPrinterList(TStrings *);
    void DeleteKeyEnv();
    void MakeKeyEnv();
    void ReadJobFolder();
    TfrmJobs * GetJobFrame(AnsiString jn);
    void MsgUpdateEnv();
    void AddJob(AnsiString job);
public:		// User declarations
    __fastcall TfrmGPSpooler(TComponent* Owner);
    __fastcall ~TfrmGPSpooler();
    void UpdateJobList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmGPSpooler *frmGPSpooler;
//---------------------------------------------------------------------------
#endif
