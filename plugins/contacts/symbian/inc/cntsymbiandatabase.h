/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef CNTSRVCONNECTION_H
#define CNTSRVCONNECTION_H

// System includes
#include <QList>
#include <e32std.h>
#include <cntdb.h>
#include <cntdbobs.h>

// User includes
#include "qcontactmanager.h"

// Forward declarations

// External data types

// Constants

class CntSymbianDatabase : public MContactDbObserver
{
public:
    CntSymbianDatabase(QContactManagerEngine *engine, QContactManager::Error& error);
    ~CntSymbianDatabase();
    
public:
    CContactDatabase* contactDatabase();
    void appendContactsRemovedEmitted(QList<QContactLocalId> *contactList);
    void appendContactsAddedEmitted(QList<QContactLocalId> *contactList);
    void appendContactsChangedEmitted(QList<QContactLocalId> *contactList);
    
public:
    // From MContactDbObserver
    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
    
private:
    CContactDatabase* m_contactDatabase;
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
    CContactChangeNotifier* m_contactChangeNotifier;
#endif
    QContactManagerEngine *m_engine;
    QList<QContactLocalId> m_contactsAddedEmitted;
    QList<QContactLocalId> m_contactsChangedEmitted;
    QList<QContactLocalId> m_contactsRemovedEmitted;    
};





#endif CNTSRVCONNECTION_H
