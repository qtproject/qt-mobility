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


#ifndef __PBAPCONVERTER_H__
#define __PBAPCONVERTER_H__

// System includes
#include <cntvcard.h>

/**
Contact Converter class, provides functionality for exporting contacts as PBAP compliant vCard2.1 or vCard3.0.
@internalComponent
*/
class CPBAPContactVCardConverter: public CContactEcomConverter
{
public: // from CContactConverter
	CPBAPContactVCardConverter(TInt64 aFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel);
	static CPBAPContactVCardConverter* NewL(TAny* aPluginParam);
	CArrayPtr<CContactItem>* ImportL(CContactDatabase& aDb, RReadStream& aReadStream, TBool& aImportSuccessful, TInt aOptions, TBool aImportSingle);
	void ExportL(CContactDatabase& aDb, const CContactIdArray& aSelectedContactIds, RWriteStream& aWriteStream, TInt aOptions, const Versit::TVersitCharSet aCharSet, TBool aExportPrivateFields, TInt aCommitNumber);
public:
	TInt64 GetFilter()const;
	MConverterCallBack* GetCallback()const;
	TVCardVersion GetVersion()const;
	TBool IsExportTel()const;
private:
	TInt64 PrepareFilterAndOption(TInt& aOption);
private:
	TInt64 iFilter;
	MConverterCallBack* iCallback;
	TVCardVersion iVersion;
	TBool iExportTel;
};

/**
Eumeration containing all the supported vCard properties required for PBAP export,
used to interpret bit-filter provided by PBAP clients.
@internalComponent
*/
enum TPBAPProperties
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
	EAllProperties		   = 0xFFFFFFF, 		
	};

#endif
