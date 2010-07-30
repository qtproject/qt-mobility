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


#ifndef __C12KEYPREDICTIVESEARCHTABLE_H__
#define __C12KEYPREDICTIVESEARCHTABLE_H__

#include "pltables.h"
#include <QList>

class CContactItem;
class RSqlDatabase;
class QString;


/**
C12keyPredictiveSearchTable class manages the predictive search tables related
to virtual ITU-T 12-key keyboard.
*/
NONSHARABLE_CLASS(C12keyPredictiveSearchTable) : public CPplPredictiveSearchTableBase
	{
public:
	static C12keyPredictiveSearchTable* NewL(RSqlDatabase& aDatabase);
	static C12keyPredictiveSearchTable* NewLC(RSqlDatabase& aDatabase);
	~C12keyPredictiveSearchTable();

public: // From CPplPredictiveSearchTableBase
	void CreateTableL();

	QList<QChar> FillAllTables() const;

private: // From CPplPredictiveSearchTableBase
	HBufC* TableNameL(const QChar aCh) const;
	TBool IsValidChar(const QChar aChar) const;

	void FillKeyboardSpecificFieldsL(RSqlStatement& aSqlStatement,
									 QStringList aTokens);

private:
	void ConstructL();
	C12keyPredictiveSearchTable(RSqlDatabase& aDatabase);
	quint64 ConvertToHex(QString aToken) const;
	const TDesC& GetTableNameL(const QChar aCh) const;

	// For unit testing
	friend class UT_CPplPredictiveSearchTable;
	};

#endif // __C12KEYPREDICTIVESEARCHTABLE_H__
