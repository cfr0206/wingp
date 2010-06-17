// Borland C++ Builder
// Copyright (c) 1995, 2002 by Borland Software Corporation
// All rights reserved

// (DO NOT EDIT: machine generated header) 'uLogs.pas' rev: 6.00

#ifndef uLogsHPP
#define uLogsHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Forms.hpp>	// Pascal unit
#include <ComCtrls.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Ulogs
{
//-- type declarations -------------------------------------------------------
typedef Set<Byte, 0, 255>  TChannels;

class DELPHICLASS TLog;
class PASCALIMPLEMENTATION TLog : public System::TObject 
{
	typedef System::TObject inherited;
	
protected:
	TChannels FChannels;
	virtual void __fastcall InternalToLog(Byte Channel, AnsiString Mes) = 0 ;
	
public:
	void __fastcall SetChannel(Byte Channel, bool Status);
	void __fastcall toLog(Byte Channel, AnsiString Mes);
	__fastcall TLog(const TChannels &AChannels);
	__property TChannels Channels = {read=FChannels, write=FChannels};
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TLog(void) { }
	#pragma option pop
	
};


class DELPHICLASS TStringsLog;
class PASCALIMPLEMENTATION TStringsLog : public TLog 
{
	typedef TLog inherited;
	
public:
	Classes::TStrings* FStrings;
	virtual void __fastcall InternalToLog(Byte Channel, AnsiString Mes);
	__fastcall TStringsLog(const TChannels &AChannels, Classes::TStrings* AStrings);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TStringsLog(void) { }
	#pragma option pop
	
};


class DELPHICLASS TFileLog;
class PASCALIMPLEMENTATION TFileLog : public TLog 
{
	typedef TLog inherited;
	
public:
	TextFile FFile;
	virtual void __fastcall InternalToLog(Byte Channel, AnsiString Mes);
	__fastcall TFileLog(const TChannels &AChannels, AnsiString FileName);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TFileLog(void) { }
	#pragma option pop
	
};


class DELPHICLASS TCaptionLog;
class PASCALIMPLEMENTATION TCaptionLog : public TLog 
{
	typedef TLog inherited;
	
public:
	Controls::TControl* FControl;
	virtual void __fastcall InternalToLog(Byte Channel, AnsiString Mes);
	__fastcall TCaptionLog(const TChannels &AChannels, Controls::TControl* AControl);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TCaptionLog(void) { }
	#pragma option pop
	
};


class DELPHICLASS TStatusBarLog;
class PASCALIMPLEMENTATION TStatusBarLog : public TLog 
{
	typedef TLog inherited;
	
public:
	Comctrls::TStatusPanel* FStatusPanel;
	virtual void __fastcall InternalToLog(Byte Channel, AnsiString Mes);
	__fastcall TStatusBarLog(const TChannels &AChannels, Comctrls::TStatusPanel* AStatusPanel);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TStatusBarLog(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMessageBoxLog;
class PASCALIMPLEMENTATION TMessageBoxLog : public TLog 
{
	typedef TLog inherited;
	
public:
	AnsiString fCaption;
	int fFlags;
	virtual void __fastcall InternalToLog(Byte Channel, AnsiString Mes);
	__fastcall TMessageBoxLog(const TChannels &AChannels, AnsiString Caption, int Flags);
public:
	#pragma option push -w-inl
	/* TObject.Destroy */ inline __fastcall virtual ~TMessageBoxLog(void) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall ToLog(Byte Channel, AnsiString Mes);
extern PACKAGE Word __fastcall SetLog(TLog* ALog);
extern PACKAGE int __fastcall SetNamedLog(TLog* ALog, AnsiString Name);
extern PACKAGE int __fastcall ReSetNamedLog(AnsiString Name);

}	/* namespace Ulogs */
using namespace Ulogs;
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// uLogs
