//---------------------------------------------------------------------------


#ifndef frmJobFrameH
#define frmJobFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmJobs : public TFrame
{
__published:	// IDE-managed Components
        TPanel *Panel1;
        TLabel *Label1;
private:	// User declarations
public:		// User declarations
        __fastcall TfrmJobs(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmJobs *frmJobs;
//---------------------------------------------------------------------------
#endif
