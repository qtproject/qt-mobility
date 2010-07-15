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


#ifndef __CQWERTYPREDICTIVESEARCHTABLE_H__
#define __CQWERTYPREDICTIVESEARCHTABLE_H__

#include "pltables.h"
#include <QList>

class CContactItem;
class RSqlDatabase;
class QStringList;


/**
CQwertyPredictiveSearchTable class manages the predictive search tables related
to the virtual QWERTY keyboard.
*/
NONSHARABLE_CLASS(CQwertyPredictiveSearchTable) : public CPplPredictiveSearchTableBase
	{
public:
	static CQwertyPredictiveSearchTable* NewL(RSqlDatabase& aDatabase);
	static CQwertyPredictiveSearchTable* NewLC(RSqlDatabase& aDatabase);
	~CQwertyPredictiveSearchTable();

public: // From CPplPredictiveSearchTableBase
	void CreateTableL();
	
	QList<QChar> FillAllTables() const;

private: // From CPplPredictiveSearchTableBase
	HBufC* TableNameL(const QChar aCh) const;
	TBool IsValidChar(const QChar aChar) const;

	void FillKeyboardSpecificFieldsL(RSqlStatement& aSqlStatement,
									 QStringList aTokens);
	QStringList GetTableSpecificFields(const CContactItem& aItem,
									   TBool& aMandatoryFieldsPresent) const;

private:
	void ConstructL();
	CQwertyPredictiveSearchTable(RSqlDatabase& aDatabase);


	// For unit testing
	friend class UT_CQwertyPredictiveSearchTable;
	};

#endif // __CQWERTYPREDICTIVESEARCHTABLE_H__
