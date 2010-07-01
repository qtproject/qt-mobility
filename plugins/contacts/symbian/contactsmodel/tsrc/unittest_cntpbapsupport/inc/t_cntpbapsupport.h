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
*
*/

		
#ifndef __T_CNTPBAPSUPPORT_H__
#define __T_CNTPBAPSUPPORT_H__
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntconvertercallback.h"
#endif


//
// Class CCntPBAPSupport Definition
//

class CCntPBAPSupport : public CBase, public MConverterCallBack
	{
public:
	static CCntPBAPSupport* NewL();
	~CCntPBAPSupport();	
	//Test Functions
	void TestExportOfAllSupportedFieldsL();
	void TestMandatoryAndTELPropertiesL();
	void TestIntraContactPropertiesL();
	void TestFilterL();
	void TestTypePrefixingL();
	void TestVCard21SupportL();
	void SetIntraExport(TBool aState);
private:
	CCntPBAPSupport();
	void ConstructL();
	//Support Functions
	void ExportContactsL(TInt64 aFilter, TVCardVersion aVer, TBool aExportTel);
	void ImportContactsL(const TDesC8& aFileName);
	void AddIntraContactPropertiesL(const TContactItemId& aContactId, CArrayPtr<CParserProperty>* aPropertyList);
	void CompareFilesL(RReadStream& aCompareStream);
	TBool CompareLines(const TDesC8& aLine1, const TDesC8& aLine2);
	
	void CompareExportsL(const TDesC8& aExpected);
	// Utils
	void ReportFileErrorL(TInt aErr, const TDesC& aFileName);
private:
	RFs iFsSession;
	CContactDatabase* iDb;
	CContactIdArray* iIdArray;
	TBool iSessionConnected;
	TBool iAddIntraProperties;
	};

/**
Eumeration containing all the supported vCard properties required for PBAP export,
used to interpret bit-filter provided by PBAP clients.
@internalComponent
*/	
enum TestTPBAPProperties
	{
	EPropertyVER	       = 0x01,      
	EPropertyFN			   = 0x02,      
	EPropertyN			   = 0x04,      
	EPropertyPHOTO		   = 0x08,      
	EPropertyBDAY		   = 0x10,      
	EPropertyADR		   = 0x20,      
	EPropertyLABEL		   = 0x40,      
	EPropertyTEL		   = 0x80,      
	EPropertyEMAIL		   = 0x100,     
	EPropertyMAILER		   = 0x200,     
	EPropertyTZ			   = 0x400,     
	EPropertyGEO		   = 0x800,     
	EPropertyTITLE		   = 0x1000,    
	EPropertyROLE		   = 0x2000,    
	EPropertyLOGO		   = 0x4000,    
	EPropertyAGENT		   = 0x8000,    
	EPropertyORG		   = 0x10000,   
	EPropertyNOTE		   = 0x20000,   
	EPropertyREV		   = 0x40000,   
	EPropertySOUND		   = 0x80000,   
	EPropertyURL		   = 0x100000,  
	EPropertyUID		   = 0x200000,  
	EPropertyKEY		   = 0x400000,  
	EPropertyNICKNAME	   = 0x800000,  
	EPropertyCATEGORIES	   = 0x1000000, 
	EPropertyPRODID		   = 0x2000000, 
	EPropertyCLASS		   = 0x4000000, 
	EPropertySORTSTRING    = 0x8000000, 
	};

#endif
