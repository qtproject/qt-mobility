/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Exports the Contact items from the database into a vCard
*
*/



 
#ifndef __TESTCONTACTSPBAPEXPORT_H__
#define __TESTCONTACTSPBAPEXPORT_H__

// System includes
#include <cntfilt.h>
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>
#include <cntdb.h>
#include <vcard.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <testexecutestepbase.h>

// User includes
#include "cntvcardutils.h"
#include "contactspbapexportutilityclass.h"

class CTestContactsPBAPExport : public CTestStep
	{
public:
	CTestContactsPBAPExport();
	~CTestContactsPBAPExport();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void ExportContactsL();
	void GetInputFromIni();
	void SetFilterL();
		
private:
	CActiveScheduler* iScheduler;
	CContactsPBAPExportUtilityClass* iExportObj;
	TPtrC 	iStandard;
	TPtrC 	iExportTo;
	TPtrC 	iFilter;
	TBool 	iSetOOM;
	TBool 	iDamageDb;
	TBool 	iInvalidFileSystem;
	TBool   iFilterBitsFutureUse;
	TBool 	iSetFilterOnlyFutureUse;
	TInt64  iContactFilter;
	
	};

// Constants
const TInt64 KFilterBitsForFutureUse 		= 0x7FFFFFFFFF;// Set all the bits including those reserved for future use
const TInt64 KFilterBitsOnlyForFutureUse 	= 0x7FFE000000;// Set only those bits, reserved for future use
	
enum TTokenIdentifier
	{
	ETokenIdVersion	=	0x01,
	ETokenIdFn		=	0x02,
	ETokenIdN		=	0x04,
	ETokenIdPhoto	=	0x08,
	ETokenIdBday	=	0x10,
	ETokenIdAdr		=	0x20,
	ETokenIdLabel	=	0x40,
	ETokenIdTel		=	0x80,
	ETokenIdEmail	=	0x100,
	ETokenIdMailer	=	0x200,
	ETokenIdTz		=	0x400,
	ETokenIdGeo		=	0x800,
	ETokenIdTitle	=	0x1000,
	ETokenIdRole	=	0x2000,
	ETokenIdLogo	=	0x4000,
	ETokenIdAgent	=	0x8000,
	ETokenIdOrg		=	0x10000,
	ETokenIdNote	=	0x20000,
	ETokenIdRev		=	0x40000,
	ETokenIdSound	=	0x80000,
	ETokenIdUrl		=	0x100000,
	ETokenIdUid		=	0x200000,
	ETokenIdKey		=	0x400000	 
	};

_LIT(KTestContactsPBAPExport,   "TestContactsPBAPExport");
_LIT(KStandard,					"Standard");
_LIT(KNumberToExport,  			"NumberToExport");
_LIT(KExportTo,					"ExportTo");
_LIT(KFilter,					"Filter");
_LIT(KVersion,					"VERSION");
_LIT(KNameVal,					"N");
_LIT(KFirstName,				"FN");
_LIT(KTel,						"TEL");
_LIT(KPhoto,					"PHOTO");
_LIT(KBday,						"BDAY");
_LIT(KAdr,						"ADR");
_LIT(KLabel,					"LABEL");
_LIT(KEmail,					"EMAIL");
_LIT(KMailer,					"MAILER");
_LIT(KTitle,					"TITLE");
_LIT(KRole,						"ROLE");
_LIT(KLogo,						"LOGO");
_LIT(KOrg,						"ORG");
_LIT(KNote,						"NOTE");
_LIT(KRev,						"REV");
_LIT(KSound,					"SOUND");
_LIT(KUrl,						"URL");
_LIT(KUid,						"UID");
_LIT(KKey,						"KEY");
_LIT(KTZ,						"TZ");
_LIT(KGeo,						"GEO");
_LIT(KAgent,					"AGENT");
_LIT(KSetOOM,					"setOOM");
_LIT(KDamageDb,					"Damage");
_LIT(KInvalidFileSystem,		"InvalidFileSystem");
_LIT(KFilterBitsFutureUse,		"SetFilterBitsFutureUse");
_LIT(KSetFilterOnlyFutureUse,	"SetFilterOnlyFutureUse");

#endif
