/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef CNTSYMBIANDATABASE_H
#define CNTSYMBIANDATABASE_H

// System includes
#include <QList>
#include <QObject>
#include <e32std.h>
#include <cntdb.h>
#include <cntdbobs.h>

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
#ifdef SYMBIAN_BACKEND_USE_CNTMODEL_V2  
, public MContactDbObserverV2
#endif
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

#ifdef SYMBIAN_BACKEND_USE_CNTMODEL_V2    
    // From MContactDbObserverV2
    void HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent);
#endif    
    
private:
    void initializeL();
    void updateGroupMembershipsL();
    void updateGroupMembershipsL(QContactLocalId groupId, QSet<QContactLocalId> &added, QSet<QContactLocalId> &removed);
    QSet<QContactLocalId> groupMembersL(QContactLocalId groupId);

private:
#ifndef SYMBIAN_BACKEND_USE_CNTMODEL_V2
    CContactChangeNotifier* m_contactChangeNotifier;
#endif
    QContactManagerEngine *m_engine;
    CContactDatabase* m_contactDatabase;
    QList<QContactLocalId> m_contactsEmitted;
    QContactLocalId m_currentOwnCardId;
    QMap<QContactLocalId, QSet<QContactLocalId> > m_groupContents;
    bool m_restoreStarted;
#ifdef CNTSYMBIANDATABASE_UNIT_TEST
    friend class TestCntSymbianDatabase;
#endif  //CNTSYMBIANDATABASE_UNIT_TEST
};

#endif // CNTSYMBIANDATABASE_H
