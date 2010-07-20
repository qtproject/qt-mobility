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


#ifndef __CPREDICTIVESEARCHSETTINGSTABLE_H__
#define __CPREDICTIVESEARCHSETTINGSTABLE_H__

#include "pltables.h"
#include <QLocale>

class CCntSqlStatement;
class RSqlDatabase;

/**
The CPplPredictiveSearchTable class contains numeric representation of the
fields that are checked in predictive search.
*/
NONSHARABLE_CLASS(CPredictiveSearchSettingsTable) : public CPplPredictiveSearchTableBase
	{
public:
	static CPredictiveSearchSettingsTable* NewL(RSqlDatabase& aDatabase);
	static CPredictiveSearchSettingsTable* NewLC(RSqlDatabase& aDatabase);
	virtual ~CPredictiveSearchSettingsTable();

public: // From CPplTableBase
	void CreateInDbL(CContactItem& aItem);
	void UpdateL(const CContactItem& aItem);
	void DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred);
	void CreateTableL();

public: // CPplPredictiveSearchTableBase
	QList<QChar> FillAllTables() const;

private: // From CPplPredictiveSearchTableBase
	HBufC* TableNameL(const QChar aCh) const;

	TBool IsValidChar(const QChar /*aChar*/) const { return ETrue; }
	void FillKeyboardSpecificFieldsL(RSqlStatement& /*aSqlStatement*/,
									 QStringList /*aTokens*/) {}

public: // New functions
	TBool IsLanguageValidL() const;
	void StoreCurrentLanguageL() const;

protected:
	void ConstructL();
	CPredictiveSearchSettingsTable(RSqlDatabase& aDatabase);

private:
	void WriteToDbL(const CContactItem& aItem);

	enum QLocale::Language GetCurrentLanguageL() const;

	enum QLocale::Language ReadStoredLanguageL() const;

	// For unit testing
	friend class UT_CPredictiveSearchSettingsTable;
	};

#endif // __CPREDICTIVESEARCHSETTINGSTABLE_H__
