/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pltables.h"
#include "dbsqlconstants.h"
#include "cntpersistenceutility.h"
#include <cntdef.h>


/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplPresenceTable object.
*/
CPplPresenceTable*
CPplPresenceTable::NewL(RSqlDatabase& aDatabase)
    {
    CPplPresenceTable* self = CPplPresenceTable::NewLC(aDatabase);
    CleanupStack::Pop(self);
    return self;
    }


/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplPresenceTable object.
*/
CPplPresenceTable*
CPplPresenceTable::NewLC(RSqlDatabase& aDatabase)
    {
    CPplPresenceTable* self =
        new (ELeave) CPplPresenceTable(aDatabase);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


/**
Destructor
*/
CPplPresenceTable::~CPplPresenceTable()
    {
    }


/**
Set up the CCntSqlStatement objects held by the class.
*/
void CPplPresenceTable::ConstructL()
    {
    }


/**
Constructor
*/
CPplPresenceTable::CPplPresenceTable(RSqlDatabase& aDatabase) :
    iDatabase(aDatabase)
    {
    }


/**
Creates the presence table and its indexes in the database.
*/
void CPplPresenceTable::CreateTableL()
    {
    User::LeaveIfError(iDatabase.Exec(KPresenceCreateStmnt));
    }


/**
@param aItem A contact item whose data are added to the table.
*/
void CPplPresenceTable::CreateInDbL(CContactItem& /*aItem*/)
    {
    }


/**
@param aItem A contact item whose data is updated in the database.
*/
void CPplPresenceTable::UpdateL(const CContactItem& /*aItem*/)
    {
    }

/**
Deletes all the presence information of the contact item. Should be used when
deleting a particular online account information from the Contact.

@param aItem The contact item whose communcation addresses are to be deleted.
*/
void CPplPresenceTable::DeleteL(const CContactItem& /*aItem*/, TBool& /*aLowDiskErrorOccurred*/)
    {
    }

// End of File
