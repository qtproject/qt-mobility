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
class Q_CALENDAR_EXPORT QOrganizerItemChangeSet
{
public:
    QOrganizerItemChangeSet();
    QOrganizerItemChangeSet(const QOrganizerItemChangeSet& other);
    ~QOrganizerItemChangeSet();

    QOrganizerItemChangeSet& operator=(const QOrganizerItemChangeSet& other);

    void setDataChanged(bool dataChanged);
    bool dataChanged();

    QSet<QOrganizerItemLocalId> addedItems() const;
    void insertAddedItem(QOrganizerItemLocalId addedItemId);
    void insertAddedItems(const QList<QOrganizerItemLocalId>& addedItemIds);
    void clearAddedItems();

    QSet<QOrganizerItemLocalId> changedItems() const;
    void insertChangedItem(QOrganizerItemLocalId addedItemId);
    void insertChangedItems(const QList<QOrganizerItemLocalId>& addedItemIds);
    void clearChangedItems();

    QSet<QOrganizerItemLocalId> removedItems() const;
    void insertRemovedItem(QOrganizerItemLocalId addedItemId);
    void insertRemovedItems(const QList<QOrganizerItemLocalId>& addedItemIds);
    void clearRemovedItems();

    void clearAll();

    void emitSignals(QOrganizerItemManagerEngine *engine);

private:
    QSharedDataPointer<QOrganizerItemChangeSetData> d;
};

QTM_END_NAMESPACE

#endif
