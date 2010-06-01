/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
#ifndef CNTSYMBIANDATABASE_H
#define CNTSYMBIANDATABASE_H

// System includes
#include <QList>
#include <QObject>
#include <e32std.h>
#ifdef CNTSYMBIANDATABASE_UNIT_TEST
#include "mock_cntdb.h"
#else
#include <cntdb.h>
#include <cntdbobs.h>
#endif

// User includes
#include "qcontactmanager.h"

// Forward declarations
QTM_BEGIN_NAMESPACE
class QContactManagerEngine;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE
// External data types

// Constants

class CntSymbianDatabase : public QObject, public MContactDbObserver
{
Q_OBJECT

public:
    CntSymbianDatabase(QContactManagerEngine *engine, QContactManager::Error* error);
    ~CntSymbianDatabase();

public:
    CContactDatabase* contactDatabase();
    void appendContactsEmitted(const QList<QContactLocalId>& contactList);
    void appendContactEmitted(QContactLocalId id);

public:
    // From MContactDbObserver
    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

private:
    void initializeL();
    void updateGroupMembershipsL();
    void updateGroupMembershipsL(QContactLocalId groupId, QSet<QContactLocalId> &added, QSet<QContactLocalId> &removed);
    QSet<QContactLocalId> groupMembersL(QContactLocalId groupId);

private:
    QContactManagerEngine *m_engine;
    CContactDatabase* m_contactDatabase;
#ifndef SYMBIAN_BACKEND_USE_SQLITE
    CContactChangeNotifier* m_contactChangeNotifier;
#endif
    QList<QContactLocalId> m_contactsEmitted;
    QContactLocalId m_currentOwnCardId;
    QMap<QContactLocalId, QSet<QContactLocalId> > m_groupContents;
#ifdef CNTSYMBIANDATABASE_UNIT_TEST
    friend class TestCntSymbianDatabase;
#endif  //CNTSYMBIANDATABASE_UNIT_TEST
};

#endif // CNTSYMBIANDATABASE_H
