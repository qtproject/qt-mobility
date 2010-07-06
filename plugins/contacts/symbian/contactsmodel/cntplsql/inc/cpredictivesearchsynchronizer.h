/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/


#ifndef __CPREDICTIVESEARCHSYNCHRONIZER_H__
#define __CPREDICTIVESEARCHSYNCHRONIZER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RSqlDatabase;
class CContactItem;
class CPplPredictiveSearchTableBase;
class C12keyPredictiveSearchTable;
class CQwertyPredictiveSearchTable;
class CPredictiveSearchSettingsTable;


/**
The CPredictiveSearchSynchronizer class takes care of populating predictive
search tables with the relevant data from contacts.
*/
NONSHARABLE_CLASS(CPredictiveSearchSynchronizer) : public CBase
	{
public:
	static CPredictiveSearchSynchronizer*
		NewL(RSqlDatabase& aDatabase,
			 C12keyPredictiveSearchTable& a12keyTable,
			 CQwertyPredictiveSearchTable& aQwertyTable,
			 CPredictiveSearchSettingsTable& aSettingsTable);
	static CPredictiveSearchSynchronizer*
		NewLC(RSqlDatabase& aDatabase,
			  C12keyPredictiveSearchTable& a12keyTable,
			  CQwertyPredictiveSearchTable& aQwertyTable,
			  CPredictiveSearchSettingsTable& aSettingsTable);
	virtual ~CPredictiveSearchSynchronizer();

public: // New functions
	void SynchronizeTablesL();

	/**
	 * Create and fill the specified predictive search tables.
	 *
	 * aAllTables If ETrue, then 12-key, QWERTY table and settings table are
	 *            created. Otherwise just QWERTY table is created.
	 */
	void CreatePredSearchTablesL(TBool aAllTables = ETrue);
	void DeletePredSearchTablesL();
	
private:
	void ConstructL();
	CPredictiveSearchSynchronizer(RSqlDatabase& aDatabase,
								  C12keyPredictiveSearchTable& a12keyTable,
								  CQwertyPredictiveSearchTable& aQwertyTable,
								  CPredictiveSearchSettingsTable& aSettingsTable);

private: // New functions
	TBool CheckIfPredSearchTableExistsL(const TDesC& aTableName) const;
	void DeletePredSearchTableL(CPplPredictiveSearchTableBase& aTable);
	TBool ReadMailAddressesL(CContactItem& aContact);

private: // Data
	RSqlDatabase&					iDatabase;
	C12keyPredictiveSearchTable&	i12keyTable;
	CQwertyPredictiveSearchTable&	iQwertyTable;
	CPredictiveSearchSettingsTable& iSettingsTable;

	// For unit testing
	friend class UT_CPplPredictiveSearchTable;
	};

#endif // __CPREDICTIVESEARCHSYNCHRONIZER_H__
