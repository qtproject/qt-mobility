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

#ifndef QORGANIZERITEMDETAILRANGEFILTER_P_H
#define QORGANIZERITEMDETAILRANGEFILTER_P_H

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

#include "qorganizeritemdetailrangefilter.h"

#include <QString>
#include <QVariant>

QTM_BEGIN_NAMESPACE

class QOrganizerItemDetailRangeFilterPrivate : public QOrganizerItemFilterPrivate
{
public:
    QOrganizerItemDetailRangeFilterPrivate()
        : QOrganizerItemFilterPrivate(),
        m_flags(0),
        m_rangeflags(0)
    {
    }

    QOrganizerItemDetailRangeFilterPrivate(const QOrganizerItemDetailRangeFilterPrivate& other)
        : QOrganizerItemFilterPrivate(other),
        m_defId(other.m_defId),
        m_fieldId(other.m_fieldId),
        m_minValue(other.m_minValue),
        m_maxValue(other.m_maxValue),
        m_flags(other.m_flags),
        m_rangeflags(other.m_rangeflags)
    {
    }

    virtual bool compare(const QOrganizerItemFilterPrivate* other) const
    {
        const QOrganizerItemDetailRangeFilterPrivate *od = static_cast<const QOrganizerItemDetailRangeFilterPrivate*>(other);
        if (m_defId != od->m_defId)
            return false;
        if (m_fieldId != od->m_fieldId)
            return false;
        if (m_minValue != od->m_minValue)
            return false;
        if (m_maxValue!= od->m_maxValue)
            return false;
        if (m_flags != od->m_flags)
            return false;
        if (m_rangeflags != od->m_rangeflags)
            return false;
        return true;
    }

    Q_IMPLEMENT_ORGANIZERITEMFILTER_VIRTUALCTORS(QOrganizerItemDetailRangeFilter, QOrganizerItemFilter::OrganizerItemDetailRangeFilter)

    QString m_defId;
    QString m_fieldId;
    QVariant m_minValue;
    QVariant m_maxValue;
    QOrganizerItemFilter::MatchFlags m_flags;
    QOrganizerItemDetailRangeFilter::RangeFlags m_rangeflags;
};

QTM_END_NAMESPACE

#endif
