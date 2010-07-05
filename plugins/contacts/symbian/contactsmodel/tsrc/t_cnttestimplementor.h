/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This is the base class/interface for a set of contact model tests.
* It is based on the bridge pattern.
*
*/


#ifndef __T_CNTTESTIMPLEMENTOR_H__
#define __T_CNTTESTIMPLEMENTOR_H__

#include "persistencelayer.h"
#include <cntdb.h>
#include "t_cnttestimpldefs.h"

#include <cntitem.h>


class CCntTestImplementor : public CBase
	{
public:


	virtual ~CCntTestImplementor() {};	

	void PrintCntForViewL(const TDesC& aFileName, const CContactItemViewDef& aView);
	void DumpContactL (const CContactItem& aItem, const TDesC& aLabel);

	virtual void OpenDatabaseL(const TDesC& aFilename) = 0; 
	virtual void CreateDatabaseL(const TDesC& aFilename, TBool aOwerwrite = EFalse) = 0; 
	virtual void CloseDatabase() = 0; 


	void ConstructL();

	// Virtual methods
	virtual CContactItem*  ReadMinimalLC (TContactItemId id)       = 0;
	virtual CContactItem*  OpenLC  		 (TContactItemId id, const CContactItemViewDef& aViewDef) = 0;
	virtual void GroupAddAndRemove 		 (CContactGroup& aGrp, TContactItemId addId, TContactItemId remId) = 0;
	virtual TContactItemId CreateL 		 (CContactItem& cntItem)   = 0;
	virtual void UpdateL           		 (CContactItem& cntItem)   = 0;
	virtual void DeleteL           		 (TContactItemId id)       = 0;
	virtual CContactGroup* CreateGroupLC (const TDesC& aGroupLabel)= 0;
	virtual CContactItem* CreateContactCardTemplateLC (const TDesC& aGroupLabel)= 0;
	virtual const TDesC& TesterType		 () 					   = 0;
	virtual const CContactTemplate&  GetSysTemplate () 			   = 0;
	virtual void CompactL() = 0;
	virtual TBool CompressRequired() = 0;
	virtual TBool IsDamaged() const = 0;
	virtual TInt FileSize() const = 0;
	virtual TInt WastedSpaceInBytes() const = 0;
	virtual TBool ContactDatabaseExistsL(const TDesC& aFileName) = 0;
	virtual TBool DefaultContactDatabaseExistsL() = 0;	
	virtual void DeleteDefaultFileL() = 0;
	virtual void GetDefaultNameL(TDes &aDes) = 0;
	virtual TBool DatabaseDrive(TDriveUnit& aDriveUnit) = 0;
	virtual CDesCArray* ListDatabasesL(TDriveUnit& aDriveUnit) = 0;
	virtual void SetOwnCard(TContactItemId id) = 0;
	
	//Reads a contacts from the database and merges it with 
	//the template, so the contact contains all the fields, including empty fields
	virtual CContactItem* ReadLC(TContactItemId id, const CContactItemViewDef& aViewDef) = 0;
	

protected:

	TInt iDumpCounter;

	};
	




// Contact Test Implementation
// Tests for the original contacts model.
class CCntTestImpl  : public CCntTestImplementor  // Derived from CBase - Zero Initialisation
	{
public:

	static CCntTestImpl* NewLC();
	virtual ~CCntTestImpl();

	void OpenDatabaseL(const TDesC& aFilename); 
	void CreateDatabaseL(const TDesC& aFilename, TBool aOwerwrite = EFalse); 
	void CloseDatabase(); 

	//Overloaded Virtual methods
	CContactItem* ReadMinimalLC(TContactItemId id);
	CContactItem* OpenLC(TContactItemId id, const CContactItemViewDef& aViewDef);
	void GroupAddAndRemove(CContactGroup& aGrp, TContactItemId addId, TContactItemId remId);
	TContactItemId CreateL(CContactItem& cntItem);
	void UpdateL(CContactItem& cntItem);
	void DeleteL(TContactItemId id);
	CContactGroup* CreateGroupLC(const TDesC& aGroupLabel);	
	CContactItem* CreateContactCardTemplateLC (const TDesC& aGroupLabel);
	const TDesC& TesterType() { return nsCntTestImpl::KCnt;};

	CContactItem* ReadLC(TContactItemId id, const CContactItemViewDef& aViewDef);
	
	const CContactTemplate&  GetSysTemplate ();
	void CompactL();
	TBool CompressRequired();
	TBool IsDamaged() const;
	TBool DefaultContactDatabaseExistsL();
	TBool ContactDatabaseExistsL(const TDesC& aFileName);
	TInt FileSize() const;
	TInt WastedSpaceInBytes() const;
	void DeleteDefaultFileL();
	void GetDefaultNameL(TDes &aDes);
	TBool DatabaseDrive(TDriveUnit& aDriveUnit);
	CDesCArray* ListDatabasesL(TDriveUnit& aDriveUnit);
	void SetOwnCard(TContactItemId id);

private:
	CContactDatabase* iDatabase;
	CContactTemplate* iGoldenTemplate;
	};







// Persistence layer Test Implementation
// Tests the Persistence layer contacts model.
class CPlTestImpl  : public CCntTestImplementor // Derived from CBase - Zero Initialisation
	{
public:

	static CPlTestImpl* NewLC();
	~CPlTestImpl();

	void OpenDatabaseL(const TDesC& aFilename); 
	void CreateDatabaseL(const TDesC& aFilename, TBool aOwerwrite = EFalse); 
	void CloseDatabase(); 

	// Virtual Methods
	CContactItem* ReadMinimalLC(TContactItemId id);
	CContactItem* OpenLC(TContactItemId id, const CContactItemViewDef& aViewDef);
	void GroupAddAndRemove(CContactGroup& aGrp, TContactItemId addId, TContactItemId remId);
	TContactItemId CreateL(CContactItem& cntItem);
	void UpdateL(CContactItem& cntItem);
	void DeleteL(TContactItemId id);
	CContactGroup* CreateGroupLC(const TDesC& aGroupLabel);	
	CContactItem* CreateContactCardTemplateLC (const TDesC& aGroupLabel);	
	const TDesC& TesterType() {return nsCntTestImpl::KPl;};
	CContactItem* ReadLC(TContactItemId id, const CContactItemViewDef& aViewDef) ;

	const CContactTemplate&  GetSysTemplate ();
	void CompactL();
	TBool CompressRequired();
	TBool IsDamaged() const;
	TBool DefaultContactDatabaseExistsL();
	TBool ContactDatabaseExistsL(const TDesC& aFileName);
	TInt FileSize() const;
	TInt WastedSpaceInBytes() const;
	void DeleteDefaultFileL();
	void GetDefaultNameL(TDes &aDes);
	TBool DatabaseDrive(TDriveUnit& aDriveUnit);
	CDesCArray* ListDatabasesL(TDriveUnit& aDriveUnit);
	void SetOwnCard(TContactItemId id);

private:
	void AddLabelField(CContactItem& aItem);

private:
	CPersistenceLayer* iPersistLayer;
	RFs iFs;
	};

	
	
	
// Persistence layer Test Implementation
// Tests the Persistence layer contacts model.
class CClientSvrImpl : public CCntTestImplementor // Derived from CBase - Zero Initialisation
	{
public:

	static CClientSvrImpl* NewLC();
	~CClientSvrImpl();

	void OpenDatabaseL(const TDesC& aFilename); 
	void CreateDatabaseL(const TDesC& aFilename, TBool aOwerwrite = EFalse); 
	void CloseDatabase(); 
	
	// Virtual Methods
	CContactItem* ReadMinimalLC(TContactItemId id);
	CContactItem* OpenLC(TContactItemId id, const CContactItemViewDef& aViewDef);
	void GroupAddAndRemove(CContactGroup& aGrp, TContactItemId addId, TContactItemId remId);
	TContactItemId CreateL(CContactItem& cntItem);
	void UpdateL(CContactItem& cntItem);
	void DeleteL(TContactItemId id);
	CContactGroup* CreateGroupLC(const TDesC& aGroupLabel);
	CContactItem* CreateContactCardTemplateLC (const TDesC& aGroupLabel);
	const TDesC& TesterType() {return nsCntTestImpl::KPl;};
	CContactItem* ReadLC(TContactItemId id, const CContactItemViewDef& aViewDef) ;

	const CContactTemplate&  GetSysTemplate ();
	void CompactL();
	TBool CompressRequired();
	TBool IsDamaged() const;
	TBool DefaultContactDatabaseExistsL();
	TBool ContactDatabaseExistsL(const TDesC& aFileName);
	TInt FileSize() const;
	TInt WastedSpaceInBytes() const;
	void DeleteDefaultFileL();
	void GetDefaultNameL(TDes &aDes);
	TBool DatabaseDrive(TDriveUnit& aDriveUnit);
	CDesCArray* ListDatabasesL(TDriveUnit& aDriveUnit);
	void SetOwnCard(TContactItemId id);

private:
	CContactDatabase* iCntDatabase;
	CContactTemplate* iGoldenTemplate; 
	};	
#endif //__T_CNTTESTIMPLEMENTOR_H__
