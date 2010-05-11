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

#ifndef QORGANIZERITEMIDFETCHREQUEST_H
#define QORGANIZERITEMIDFETCHREQUEST_H

#include "qtorganizeritemsglobal.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemsortorder.h"
#include "qorganizeritemfilter.h"

#include <QList>
#include <QStringList>

QTM_BEGIN_NAMESPACE

class QOrganizerItemLocalIdFetchRequestPrivate;
class Q_CALENDAR_EXPORT QOrganizerItemLocalIdFetchRequest : public QOrganizerItemAbstractRequest
{
    Q_OBJECT

public:
    QOrganizerItemLocalIdFetchRequest(QObject* parent = 0);

    /* Selection, restriction and sorting */
    void setFilter(const QOrganizerItemFilter& filter);
    void setSorting(const QList<QOrganizerItemSortOrder>& sorting);
    QOrganizerItemFilter filter() const;
    QList<QOrganizerItemSortOrder> sorting() const;

    /* Results */
    QList<QOrganizerItemLocalId> itemIds() const;

private:
    Q_DISABLE_COPY(QOrganizerItemLocalIdFetchRequest)
    friend class QOrganizerItemManagerEngine;
    Q_DECLARE_PRIVATE_D(d_ptr, QOrganizerItemLocalIdFetchRequest)
};

QTM_END_NAMESPACE

#endif
