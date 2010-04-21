/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MOCK_CNTD_H
#define MOCK_CNTD_H

enum TContactDbObserverEventType
    {
    /** Null event type. */
    EContactDbObserverEventNull,
    /** Not currently used. */
    EContactDbObserverEventUnused,
    /** A contact item (not a template, group or own card) has been changed. */
    EContactDbObserverEventContactChanged,
    /** A contact item (not a template, group or own card) has been deleted. */
    EContactDbObserverEventContactDeleted,
    /** A contact item (not a template or group) has been added. */
    EContactDbObserverEventContactAdded,
    /** Unknown change event type. */
    EContactDbObserverEventUnknownChanges,
    /** Recover from a damaged database. */
    EContactDbObserverEventRecover,
    /** Rollback from a change to the database. */
    EContactDbObserverEventRollback,
    /** Database tables closed. */
    EContactDbObserverEventTablesClosed,
    /** Database tables opened. */
    EContactDbObserverEventTablesOpened,
    /** A contact card template has changed. */
    EContactDbObserverEventTemplateChanged,
    /** A contact card template has been deleted. */
    EContactDbObserverEventTemplateDeleted, 
    /** A contact card template has been added. */
    EContactDbObserverEventTemplateAdded,
    /** The database's current item has been deleted. */
    EContactDbObserverEventCurrentItemDeleted,
    /** The database's current item has changed. */
    EContactDbObserverEventCurrentItemChanged,
    /** The database's own card has changed. */
    EContactDbObserverEventOwnCardChanged,
    /** The database's preferred template has been set. (See CContactDatabase::SetPrefTemplateL()). */
    EContactDbObserverEventPreferredTemplateChanged,
    /** An own card has been deleted from the database. */
    EContactDbObserverEventOwnCardDeleted,
    /** A contact item group has been added. */
    EContactDbObserverEventGroupAdded,
    /** A contact item group has been updated. */
    EContactDbObserverEventGroupChanged,
    /** A contact item group has been deleted from the database. */
    EContactDbObserverEventGroupDeleted,
    /** The current database has changed. */
    EContactDbObserverEventCurrentDatabaseChanged,
    /** The phone number assigned to one of the speed dial positions 
    has been replaced, removed or modified.*/
    EContactDbObserverEventSpeedDialsChanged, 
    /** Not currently used. */
    EContactDbObserverEventSortOrderChanged,
    /** Contacts DB about to be backed up  */
    EContactDbObserverEventBackupBeginning,
    /** Contacts DB about to be restored */
    EContactDbObserverEventRestoreBeginning,
    /** The backup/restore operation has completed. */
    EContactDbObserverEventBackupRestoreCompleted,
    /** The restore operation has completed but the database could not be opened. */
    EContactDbObserverEventRestoreBadDatabase,
    /** Database has been compressed. */
    EContactDbObserverEventCompress
    };

typedef long int TContactItemId;

struct TContactDbObserverEvent
{
    TContactDbObserverEventType iType;
    TContactItemId iContactId;
};

class MContactDbObserver
{
public:
    virtual void HandleDatabaseEventL(TContactDbObserverEvent aEvent)=0;
};

class CContactDatabase
{
public:
    CContactDatabase(){};
    ~CContactDatabase(){}
    static CContactDatabase* OpenL();
    static CContactDatabase* CreateL();
    void AddObserverL(MContactDbObserver& aObserver);
    void RemoveObserver(MContactDbObserver& /*aObserver*/){};
    TContactItemId OwnCardId() const{return TContactItemId(12);};
    void sendEventsL(); // test HandleDatabaseEventL
private:
    MContactDbObserver *iObserver;
};

class CContactChangeNotifier
{
public:
    CContactChangeNotifier(CContactDatabase& aDbase, MContactDbObserver* aObserver);
    static CContactChangeNotifier* NewL(CContactDatabase& aDatabase, MContactDbObserver *aObserver);
    ~CContactChangeNotifier(){};
private:
    CContactDatabase *iDbase;
};

#endif //MOCK_CNTD_H
