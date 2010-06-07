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

#ifndef QORGANIZERITEMDATETIMEPERIODFILTER_P_H
#define QORGANIZERITEMDATETIMEPERIODFILTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qorganizeritemfilter_p.h"
#include "qorganizeritemfilter.h"

#include "qorganizeritemdatetimeperiodfilter.h"

#include <QDateTime>


QTM_BEGIN_NAMESPACE

class QOrganizerItemDateTimePeriodFilterPrivate : public QOrganizerItemFilterPrivate
{
public:
    QOrganizerItemDateTimePeriodFilterPrivate()
        : QOrganizerItemFilterPrivate()
    {
    }

    QOrganizerItemDateTimePeriodFilterPrivate(const QOrganizerItemDateTimePeriodFilterPrivate& other)
        : QOrganizerItemFilterPrivate(other),
        m_start(other.m_start),
        m_end(other.m_end)
    {
    }

    virtual bool compare(const QOrganizerItemFilterPrivate* other) const
    {
        const QOrganizerItemDateTimePeriodFilterPrivate *od = static_cast<const QOrganizerItemDateTimePeriodFilterPrivate*>(other);
        if (m_start != od->m_start)
            return false;
        if (m_end != od->m_end)
            return false;
        return true;
    }

    Q_IMPLEMENT_ORGANIZERITEMFILTER_VIRTUALCTORS(QOrganizerItemDateTimePeriodFilter, QOrganizerItemFilter::OrganizerItemDateTimePeriodFilter)

    QDateTime  m_start;
    QDateTime  m_end;
};

QTM_END_NAMESPACE

#endif
