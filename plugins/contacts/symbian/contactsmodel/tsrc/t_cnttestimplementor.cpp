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
*
*/


#include <e32test.h>
#include <coreappstest/testserver.h>

#include "persistencelayer.h"
#include "nbcnttestlib/nbcnttestlib.h"
#include <cntitem.h>
#include <cntdef.h>

#include "t_persistencelayer.h"

#include "t_cnttestimpldefs.h"
#include "t_cnttestimplementor.h"

static const TUint KPlSessionId = 31415926;

// Implementation of CCntTestImplementor protected 
// non virtual member functions.

void CCntTestImplementor::DumpContactL(const CContactItem& aItem, const TDesC& aLabel)
	{
	TFileName fileName;
	const TDesC& type = TesterType();
	fileName.Format(_L("c:\\%S\\%02d.%S.html"), &type, ++iDumpCounter,  &aLabel);

	PrintContactL(fileName, aItem); //todo remove const_cast
	}
	

void CCntTestImplementor::PrintCntForViewL(const TDesC& aFileName, const CContactItemViewDef& aView)
	{
	CContactItem* cntItem = ReadLC(1, aView);
	DumpContactL(*cntItem, aFileName);
	CleanupStack::PopAndDestroy(cntItem);
	}

void CCntTestImplementor::ConstructL()
	{
	TFileName fileName(_L("c:\\"));
	fileName.Append(TesterType());
	fileName.Append(_L("\\"));

	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);

	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);

	(void)fileMan->RmDir(fileName); //if we can't delete files - who cares?

	CleanupStack::PopAndDestroy(2); // FileServer, FileManager

	}



	
	
	
	
	
	
	
	
	
	
// Implementation of CCntTestImpl member functions

CCntTestImpl* CCntTestImpl::NewLC()
	{
	CCntTestImpl* self = new (ELeave) CCntTestImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCntTestImpl::~CCntTestImpl()
	{
	delete iGoldenTemplate;
//	CloseDatabase();
	}	
	
void CCntTestImpl::OpenDatabaseL(const TDesC& aFilename)
	{
	if (iDatabase)
		{
		delete iDatabase;
		iDatabase = NULL;
		}
	iDatabase  = CContactDatabase::OpenL(aFilename);
	}

void CCntTestImpl::CreateDatabaseL(const TDesC& aFilename, TBool aOwerwrite)
	{
	if (iDatabase)
		{
		delete iDatabase;
		iDatabase = NULL;
		}
		
	if (aOwerwrite)
		{
		iDatabase  = CContactDatabase::ReplaceL(aFilename);
		return;
		}
	iDatabase  = CContactDatabase::CreateL(aFilename);
	}

void CCntTestImpl::CloseDatabase()
	{
	delete iDatabase;
	iDatabase  = NULL;
	}


CContactItem* CCntTestImpl::ReadMinimalLC(TContactItemId id)
	{
	CContactItem* cntItem = iDatabase->ReadMinimalContactL(id);
	CleanupStack::PushL(cntItem);

	return cntItem;
	}



CContactItem* CCntTestImpl::OpenLC(TContactItemId id, const CContactItemViewDef& aViewDef)
	{
	CContactItem* cntItem = iDatabase->OpenContactL(id, aViewDef);

	CleanupStack::PushL(cntItem);
	return cntItem;
	}


TContactItemId CCntTestImpl::CreateL(CContactItem& cntItem)
	{
	TContactItemId  cntID = iDatabase->AddNewContactL(cntItem);
	return cntID;
	}


void CCntTestImpl::UpdateL(CContactItem& cntItem)
	{
	iDatabase->CommitContactL(cntItem);
	}


void CCntTestImpl::DeleteL(TContactItemId id)
	{
	iDatabase->DeleteContactL(id);
	}


CContactGroup* CCntTestImpl::CreateGroupLC(const TDesC& aGroupLabel)
	{
	return static_cast<CContactGroup*> (iDatabase->CreateContactGroupLC(aGroupLabel));
	}

CContactItem* CCntTestImpl::CreateContactCardTemplateLC(const TDesC& aLabel)
	{
	return iDatabase->CreateContactCardTemplateLC(aLabel);
	}

CContactItem* CCntTestImpl::ReadLC(TContactItemId id, const CContactItemViewDef& aViewDef) 
	{
	CContactItem* cntItem = iDatabase->ReadContactL(id, aViewDef);
	CleanupStack::PushL(cntItem);
	
	return cntItem;
	}


void CCntTestImpl::GroupAddAndRemove(CContactGroup& aGrp, TContactItemId addId, TContactItemId remId)
	{
	if (remId != -1)
		{
		CContactItem* cnt = ReadMinimalLC(remId);
		iDatabase->RemoveContactFromGroupL(*cnt, aGrp);
		CleanupStack::PopAndDestroy(cnt);
		cnt = NULL;
		}
	if (addId != -1)
		{
		CContactItem* cnt = ReadMinimalLC(addId);
		iDatabase->AddContactToGroupL(*cnt, aGrp);
		CleanupStack::PopAndDestroy(cnt);
		cnt = NULL;
		}

	}



const CContactTemplate&  CCntTestImpl::GetSysTemplate ()
	{
	if (!iGoldenTemplate)
		{
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
		matchAll->AddL(KUidContactFieldMatchAll);

		iGoldenTemplate = static_cast<CContactTemplate*>(iDatabase->ReadContactL(iDatabase->TemplateId(), *matchAll));

		CleanupStack::PopAndDestroy(matchAll);
		}
	return *iGoldenTemplate;
	}

void CCntTestImpl::CompactL()
	{
		iDatabase->CompactL();
	}

TBool CCntTestImpl::CompressRequired()
	{
	return 	iDatabase->CompressRequired();
	}

TBool CCntTestImpl::IsDamaged() const
	{
	return 	iDatabase->IsDamaged();
	}

TBool CCntTestImpl::DefaultContactDatabaseExistsL()
	{
	return 	iDatabase->DefaultContactDatabaseExistsL();
	}	
	
TBool CCntTestImpl::ContactDatabaseExistsL(const TDesC& aFileName)
	{
	return 	iDatabase->ContactDatabaseExistsL(aFileName);
	}
	
TInt CCntTestImpl::FileSize() const
	{
	return 	iDatabase->FileSize();
	}
	
TInt CCntTestImpl::WastedSpaceInBytes() const
	{
	return 	iDatabase->WastedSpaceInBytes();
	}	
	
void CCntTestImpl::DeleteDefaultFileL()
	{
	iDatabase->DeleteDefaultFileL();
	}	
	
void CCntTestImpl::GetDefaultNameL(TDes &aDes)
	{
	iDatabase->GetDefaultNameL(aDes);
	}	
	
TBool CCntTestImpl::DatabaseDrive(TDriveUnit& aDriveUnit)
	{
	return 	iDatabase->DatabaseDrive(aDriveUnit);
	}
	
CDesCArray* CCntTestImpl::ListDatabasesL(TDriveUnit& aDriveUnit)
	{
	return 	iDatabase->ListDatabasesL(aDriveUnit);
	}	

void CCntTestImpl::SetOwnCard(TContactItemId /*id*/)
	{
	//todo: add implementation
	}

// Implementation of CPlTestImpl member functions

CPlTestImpl* CPlTestImpl::NewLC()
	{
	CPlTestImpl* self = new (ELeave) CPlTestImpl();
	CleanupStack::PushL(self);

	self->ConstructL();
 	
	User::LeaveIfError(self->iFs.Connect());
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
	self->iPersistLayer = CPersistenceLayer::NewLC(self->iFs, NULL, NULL);
#else
	self->iPersistLayer = CPersistenceLayer::NewLC(self->iFs, NULL, NULL, NULL);
#endif//__SYMBIAN_CNTMODEL_USE_SQLITE__
	CleanupStack::Pop(self->iPersistLayer); //It is  a member - don't need it on the cleanup stack
	return self;
	}


CPlTestImpl::~CPlTestImpl()
	{
	CloseDatabase();
	iFs.Close();
	}


void CPlTestImpl::OpenDatabaseL(const TDesC& aFilename)
	{
	MLplContactsFile& cntFile = iPersistLayer->ContactsFileL();

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	TRAPD(err, while(cntFile.OpenStepL(aFilename)){});
	User::LeaveIfError(err);
#else	
	cntFile.OpenL(aFilename);
#endif
	}

void CPlTestImpl::CreateDatabaseL(const TDesC& aFilename, TBool aOwerwrite)
	{
	MLplContactsFile& cntFile = iPersistLayer->ContactsFileL();

	TRAPD(err, cntFile.CreateL(aFilename, aOwerwrite ? MLplContactsFile::EPlOverwrite : MLplContactsFile::EPlLeaveIfExist));
	User::LeaveIfError(err);
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TRAP(err, while(cntFile.OpenStepL(aFilename)){});
	User::LeaveIfError(err);
#else
	cntFile.OpenL(aFilename);
#endif
	}

void CPlTestImpl::CloseDatabase()
	{
	delete iPersistLayer;
	iPersistLayer = NULL;
	}


CContactItem* CPlTestImpl::ReadMinimalLC(TContactItemId id)
	{
	MLplPersistenceBroker& perBro = iPersistLayer->PersistenceBroker();

	CContactItemViewDef* noHidden = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	noHidden->AddL(KUidContactFieldMatchAll);
	CContactItem* item = perBro.ReadLC(id, *noHidden, EPlIdentityInfo | EPlEmailInfo, KPlSessionId); 
	CleanupStack::Pop(item);
	CleanupStack::PopAndDestroy(noHidden);
	
	CleanupStack::PushL(item);
	item->SetTemplateRefId(-1); //Wipe the reference to the template.
	//In contacts model this field is never read in ReadMinimal

	return item;
	}


CContactItem* CPlTestImpl::OpenLC(TContactItemId id, const CContactItemViewDef& aViewDef)
	{
	MLplPersistenceBroker& perBro = iPersistLayer->PersistenceBroker();

	CContactItemViewDef* noHidden = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	noHidden->AddL(KUidContactFieldMatchAll);
	CContactItem* cntItem = perBro.ReadLC(id, *noHidden, EPlAllInfo, KPlSessionId); 
	
	CleanupStack::Pop(cntItem);
	CleanupStack::PopAndDestroy(noHidden);
	CleanupStack::PushL(cntItem);

	//todo: not yet supporting contacts dependent on TemplateCards.
	iPersistLayer->PersistenceLayerTestL().MergeContactWithTemplateL(*cntItem, 
				iPersistLayer->ContactProperties().SystemTemplateL(), aViewDef);
		
	return cntItem;
	}
	


TContactItemId CPlTestImpl::CreateL(CContactItem& cntItem)
	{
	return iPersistLayer->PersistenceBroker().CreateL(cntItem, KPlSessionId);
	}


void CPlTestImpl::UpdateL(CContactItem& cntItem)
	{
	iPersistLayer->PersistenceBroker().UpdateL(cntItem, KPlSessionId);
	}


void CPlTestImpl::DeleteL(TContactItemId id)
	{
	CContactItem* cntItem = iPersistLayer->PersistenceBroker().DeleteLC(id, KPlSessionId, ESendEvent);
	CleanupStack::PopAndDestroy(cntItem);
	}


CContactGroup* CPlTestImpl::CreateGroupLC(const TDesC& aGroupLabel)
	{
	CContactGroup* newGroup = CContactGroup::NewLC();
	
	AddLabelField(*newGroup);
	newGroup->SetGroupLabelL(aGroupLabel);

	MLplPersistenceBroker& perBro = iPersistLayer->PersistenceBroker();
	perBro.CreateL(*newGroup, KPlSessionId);

	return newGroup;
	}


void CPlTestImpl::AddLabelField(CContactItem& aItem)
	{
	CContactItemFieldSet& aFieldSet = aItem.CardFields();

	TInt pos = aFieldSet.Find(KUidContactFieldTemplateLabel);
	if (pos==KErrNotFound)	// !HasItemLabelField()
		{
		CContactItemField* labelField = CContactItemField::NewLC(KStorageTypeText);
		labelField->AddFieldTypeL(KUidContactFieldTemplateLabel);
		if (aItem.Type()==KUidContactGroup)
			{
			_LIT(KGroupLabel,"Group Label");
			labelField->SetLabelL(KGroupLabel);
			}
		else if (aItem.Type()==KUidContactCardTemplate)
			{
			_LIT(KTemplateLabel,"Template Label");
			labelField->SetLabelL(KTemplateLabel);
			}
	// field needs to be first in the list
	// bug in stream retrieval of fields
		aItem.InsertFieldL(*labelField,0);
		CleanupStack::Pop(); // labelField
		}
	else	if (pos!=0)
		{
		aFieldSet.Move(pos,0);
		}
	}


CContactItem* CPlTestImpl::CreateContactCardTemplateLC (const TDesC& aLabel)
	{
	CContactItem* newTemplate = CContactItem::NewLC(KUidContactCardTemplate);
//	newTemplate->ClearFieldContent(); // Clear all data from the new template

	// Add label field
	AddLabelField(*newTemplate);
	static_cast<CContactCardTemplate*>(newTemplate)->SetTemplateLabelL(aLabel);

	// Create the contact in the database	
	MLplPersistenceBroker& perBro = iPersistLayer->PersistenceBroker();
	perBro.CreateL(*newTemplate, KPlSessionId);

	return newTemplate;	

	}


CContactItem* CPlTestImpl::ReadLC(TContactItemId id, const CContactItemViewDef& aViewDef) 
	{
	CContactItem* item = iPersistLayer->PersistenceBroker().ReadLC(id, aViewDef, EPlAllInfo, KPlSessionId); 
	
	//todo: not yet supporting contacts dependent on TemplateCards.
	//temporary switched off
	iPersistLayer->PersistenceLayerTestL().MergeContactWithTemplateL(*item, 
				   iPersistLayer->ContactProperties().SystemTemplateL(), aViewDef);
	return item;
	}



void CPlTestImpl::GroupAddAndRemove(CContactGroup& aGrp, TContactItemId addId, TContactItemId remId)
	{
	if (remId != -1)
		{
		aGrp.RemoveContactL(remId);
		}

	if (addId != -1)
		{
		aGrp.AddContactL(addId); 
		}

	UpdateL(aGrp);
	}


const CContactTemplate&  CPlTestImpl::GetSysTemplate ()
	{
	return iPersistLayer->ContactProperties().SystemTemplateL();
	}


void CPlTestImpl::CompactL()
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	while(iPersistLayer->ContactsFileL().CompactStepL()) 
		{};
#endif
	}

TBool CPlTestImpl::CompressRequired()
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TBool theFlag(EFalse);
	TRAP_IGNORE(theFlag = iPersistLayer->ContactsFileL().CompressRequired());
	return theFlag;
#else
	return EFalse;
#endif
	}

TBool CPlTestImpl::IsDamaged() const
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TBool theFlag(EFalse);
	TRAP_IGNORE(theFlag = iPersistLayer->ContactsFileL().IsDamaged());
	return theFlag;
#else
	return EFalse;
#endif
	}

TBool CPlTestImpl::DefaultContactDatabaseExistsL()
	{
	// TODO:
	// Need to get default name, this is normally stored in 
	// the server, in FileManagerController.
	return NULL;
	}

TBool CPlTestImpl::ContactDatabaseExistsL(const TDesC& aFileName)
	{
	return iPersistLayer->ContactsFileL().DatabaseExistsL(aFileName);
	}	

TInt CPlTestImpl::FileSize() const
	{
	TBool theFlag(EFalse);
	TRAP_IGNORE(theFlag = iPersistLayer->ContactsFileL().FileSize());
	return theFlag;	
	}
	
TInt CPlTestImpl::WastedSpaceInBytes() const
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TBool theFlag(EFalse);
	TRAP_IGNORE(theFlag = iPersistLayer->ContactsFileL().WastedSpaceInBytes());
	return theFlag;		
#else
	return 0;
#endif
	}	

void CPlTestImpl::DeleteDefaultFileL()
	{
	// TODO?:
	// Default name is normally stored in 
	// the server, in FileManagerController.	
//	return 	iPersistLayer->ContactsFileL().DeleteDefaultFileL();
	}	

void CPlTestImpl::GetDefaultNameL(TDes &aDes)
	{
	aDes = NULL;
	// TODO?:
	// Default name is normally stored in 
	// the server, in FileManagerController.	
//	return 	iDatabase->GetDefaultNameL(aDes);
	}

TBool CPlTestImpl::DatabaseDrive(TDriveUnit& aDriveUnit)
	{
	TRAP_IGNORE(iPersistLayer->ContactsFileL().DatabaseDrive(aDriveUnit));
	return EFalse;
	}
	
CDesCArray* CPlTestImpl::ListDatabasesL(TDriveUnit& aDriveUnit)
	{
	return iPersistLayer->ContactsFileL().ListL(&aDriveUnit);
	}	

void CPlTestImpl::SetOwnCard(TContactItemId anId)
	{
	TContactItemId oldOwnCard = iPersistLayer->ContactProperties().OwnCardIdL();
	if (oldOwnCard > 0)
		{
		iPersistLayer->PersistenceBroker().ChangeTypeL(anId, KUidContactCard);
		}
	iPersistLayer->PersistenceBroker().ChangeTypeL(anId, KUidContactOwnCard);
	}

// Implementation of CPlTestImpl member functions

CClientSvrImpl* CClientSvrImpl::NewLC()
	{
	CClientSvrImpl* self = new (ELeave) CClientSvrImpl();
	CleanupStack::PushL(self);
	// todo - remove pl
	return self;
	}


CClientSvrImpl::~CClientSvrImpl()
	{
	CloseDatabase();
	}


void CClientSvrImpl::OpenDatabaseL(const TDesC& aFilename)
	{
	if (iCntDatabase)
		{
		delete iCntDatabase;
		iCntDatabase = NULL;
		}
	iCntDatabase = CContactDatabase::OpenL(aFilename, CContactDatabase::ESingleThread); 
	}

void CClientSvrImpl::CreateDatabaseL(const TDesC& aFilename, TBool aOwerwrite)
	{
	if (iCntDatabase)
		{
		delete iCntDatabase;
		iCntDatabase = NULL;
		}

	if (aOwerwrite)
		{
		iCntDatabase  = CContactDatabase::ReplaceL(aFilename);
		return;
		}
	iCntDatabase = CContactDatabase::CreateL(aFilename);
	}

void CClientSvrImpl::CloseDatabase()
	{
	delete iCntDatabase;
	iCntDatabase = NULL;
	delete iGoldenTemplate;
	iGoldenTemplate = NULL; 
	}


CContactItem* CClientSvrImpl::ReadMinimalLC(TContactItemId id)
	{
	// TODO: This is not a direct comparison with the old contact model
	// ReadMinimalLC has not been implemented in the pl
	CContactItemViewDef* noHidden = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	noHidden->AddL(KUidContactFieldMatchAll);

	CContactItem* cntItem = iCntDatabase->ReadContactL(id, *noHidden);
	CleanupStack::PushL(cntItem);
	return cntItem;
	}


CContactItem* CClientSvrImpl::OpenLC(TContactItemId id, const CContactItemViewDef& /*aViewDef*/)
	{
	CContactItemViewDef* noHidden = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	noHidden->AddL(KUidContactFieldMatchAll);
	// TODO: replace with ReadLC
	CContactItem* cntItem = iCntDatabase->ReadContactL(id, *noHidden); 
	
	CleanupStack::PopAndDestroy(noHidden);
	CleanupStack::PushL(cntItem);

	return cntItem;
	}
	


TContactItemId CClientSvrImpl::CreateL(CContactItem& cntItem)
	{
	return iCntDatabase->AddNewContactL(cntItem);
	}


void CClientSvrImpl::UpdateL(CContactItem& cntItem)
	{
	CContactItem* cntItem2 = iCntDatabase->UpdateContactLC(cntItem.Id(), &cntItem);
	CleanupStack::PopAndDestroy(cntItem2);
	}


void CClientSvrImpl::DeleteL(TContactItemId id)
	{
	iCntDatabase->DeleteContactL(id);
	}


CContactGroup* CClientSvrImpl::CreateGroupLC(const TDesC& /*aGroupLabel*/)
	{
	// TODO	
	CContactGroup* newGroup = CContactGroup::NewLC();
	return newGroup;
	}

CContactItem* CClientSvrImpl::CreateContactCardTemplateLC(const TDesC& /*aGroupLabel*/)
	{
	return NULL; 	// TODO	
	}

CContactItem* CClientSvrImpl::ReadLC(TContactItemId id, const CContactItemViewDef& aViewDef) 
	{
	return iCntDatabase->ReadContactLC(id, aViewDef); 
	}



void CClientSvrImpl::GroupAddAndRemove(CContactGroup& /*aGrp*/, TContactItemId /*addId*/, TContactItemId /*remId*/)
	{
	// TODO
	}


const CContactTemplate&  CClientSvrImpl::GetSysTemplate ()
	{
	// TODO
	CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	matchAll->AddL(KUidContactFieldMatchAll);

	// TODO - Method call to get the GoldenTemplate ID
	iGoldenTemplate = static_cast<CContactTemplate*>(iCntDatabase->ReadContactL(0, *matchAll));
	CleanupStack::PopAndDestroy(matchAll);

	return *iGoldenTemplate;
	}


void CClientSvrImpl::CompactL()
	{
		iCntDatabase->CompactL();
	}


TBool CClientSvrImpl::CompressRequired()
	{
	return 	iCntDatabase->CompressRequired();
	}
	
TBool CClientSvrImpl::IsDamaged() const
	{
	return 	iCntDatabase->IsDamaged();
	}	
	
TBool CClientSvrImpl::DefaultContactDatabaseExistsL()
	{
	return 	iCntDatabase->DefaultContactDatabaseExistsL();
	}

TBool CClientSvrImpl::ContactDatabaseExistsL(const TDesC& aFileName)
	{
	return 	iCntDatabase->ContactDatabaseExistsL(aFileName);
	}
	
TInt CClientSvrImpl::FileSize() const
	{
	return 	iCntDatabase->FileSize();
	}
	
TInt CClientSvrImpl::WastedSpaceInBytes() const
	{
	return 	iCntDatabase->WastedSpaceInBytes();
	}		
	
void CClientSvrImpl::DeleteDefaultFileL()
	{
	iCntDatabase->DeleteDefaultFileL();
	}		
	
	
void CClientSvrImpl::GetDefaultNameL(TDes &aDes)
	{
	iCntDatabase->GetDefaultNameL(aDes);
	}
	
TBool CClientSvrImpl::DatabaseDrive(TDriveUnit& aDriveUnit)
	{
	return 	iCntDatabase->DatabaseDrive(aDriveUnit);
	}		
	
	
CDesCArray* CClientSvrImpl::ListDatabasesL(TDriveUnit& aDriveUnit)
	{
	return 	iCntDatabase->ListDatabasesL(aDriveUnit);
	}


void CClientSvrImpl::SetOwnCard(TContactItemId /*id*/)
	{
	//todo: add implementation
	}
