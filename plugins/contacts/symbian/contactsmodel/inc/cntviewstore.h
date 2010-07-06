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


#ifndef CNTVIEWSTORE_H_
#define CNTVIEWSTORE_H_

class RContactViewSortOrder;


/** Definition of a default remote view.

Holds all the information (excluding a database) required to create 
a standard or named remote view.

@publishedPartner
@released
*/
NONSHARABLE_CLASS(CContactDefaultViewDefinition) : public CBase
	{
public:

	enum TViewType
		{
		ERemoteView,
		ENamedRemoteView
		};
		
public:
	IMPORT_C static CContactDefaultViewDefinition* NewLC(TViewType aViewType, const TDesC& aViewName, const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName);
	IMPORT_C static CContactDefaultViewDefinition* NewLC(RReadStream& aStream);
	IMPORT_C ~CContactDefaultViewDefinition();
	
	IMPORT_C const TDesC& ViewNameL() const;
	IMPORT_C const TDesC8& SortPluginNameL() const;
	IMPORT_C const RContactViewSortOrder& SortOrder() const;
	IMPORT_C TContactViewPreferences ViewPreferences() const;
	IMPORT_C TViewType ViewType() const;
	
	IMPORT_C void InternalizeL(RReadStream &aStream);
	IMPORT_C void ExternalizeL(RWriteStream &aStream) const;
		
private:
	CContactDefaultViewDefinition();
	CContactDefaultViewDefinition(TViewType aViewType, TContactViewPreferences aContactTypes);
	void ConstructL(const TDesC& aViewName, const RContactViewSortOrder &aSortOrder, const TDesC8& aSortPluginName);
	
	void Close();
	
private:
	HBufC* iViewName;
	HBufC8* iSortPluginName;
	RContactViewSortOrder iSortOrder;
	TContactViewPreferences iContactTypes;
	TViewType iViewType;	
	};

/** Class to access the default settings store within 
    Contacts model.

@publishedPartner
@released
*/
class TContactViewStore
	{
public:
	IMPORT_C static void SetDefaultViewsL(const RPointerArray<CContactDefaultViewDefinition> &aDefaultViews);
	IMPORT_C static void GetDefaultViewsL(RPointerArray<CContactDefaultViewDefinition> &aDefaultViews);
	};

#endif // CNTVIEWSTORE_H_
