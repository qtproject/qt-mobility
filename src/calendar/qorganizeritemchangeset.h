/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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


#ifndef QORGANIZERITEMCHANGESET_H
#define QORGANIZERITEMCHANGESET_H

#include "qtorganizeritemsglobal.h"
#include "qorganizeritemid.h"

#include <QSet>
#include <QPair>
#include <QSharedDataPointer>

QTM_BEGIN_NAMESPACE

class QOrganizerItemManagerEngine;
class QOrganizerItemChangeSetData;
class Q_CONTACTS_EXPORT QOrganizerItemChangeSet
{
public:
    QOrganizerItemChangeSet();
    QOrganizerItemChangeSet(const QOrganizerItemChangeSet& other);
    ~QOrganizerItemChangeSet();

    QOrganizerItemChangeSet& operator=(const QOrganizerItemChangeSet& other);

    void setDataChanged(bool dataChanged);
    bool dataChanged();

    QSet<QOrganizerItemLocalId> addedContacts() const;
    void insertAddedContact(QOrganizerItemLocalId addedContactId);
    void insertAddedContacts(const QList<QOrganizerItemLocalId>& addedContactIds);
    void clearAddedContacts();

    QSet<QOrganizerItemLocalId> changedContacts() const;
    void insertChangedContact(QOrganizerItemLocalId addedContactId);
    void insertChangedContacts(const QList<QOrganizerItemLocalId>& addedContactIds);
    void clearChangedContacts();

    QSet<QOrganizerItemLocalId> removedContacts() const;
    void insertRemovedContact(QOrganizerItemLocalId addedContactId);
    void insertRemovedContacts(const QList<QOrganizerItemLocalId>& addedContactIds);
    void clearRemovedContacts();

    QSet<QOrganizerItemLocalId> addedRelationshipsContacts() const;
    void insertAddedRelationshipsContact(QOrganizerItemLocalId affectedContactId);
    void insertAddedRelationshipsContacts(const QList<QOrganizerItemLocalId>& affectedContactIds);
    void clearAddedRelationshipsContacts();

    QSet<QOrganizerItemLocalId> removedRelationshipsContacts() const;
    void insertRemovedRelationshipsContact(QOrganizerItemLocalId affectedContactId);
    void insertRemovedRelationshipsContacts(const QList<QOrganizerItemLocalId>& affectedContactIds);
    void clearRemovedRelationshipsContacts();

    void setOldAndNewSelfContactId(const QPair<QOrganizerItemLocalId, QOrganizerItemLocalId>& oldAndNewContactId);
    QPair<QOrganizerItemLocalId, QOrganizerItemLocalId> oldAndNewSelfContactId() const;

    void clearAll();

    void emitSignals(QOrganizerItemManagerEngine *engine);

private:
    QSharedDataPointer<QOrganizerItemChangeSetData> d;
};

QTM_END_NAMESPACE

#endif
