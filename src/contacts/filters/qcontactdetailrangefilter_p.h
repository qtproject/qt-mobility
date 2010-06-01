/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCONTACTDETAILRANGEFILTER_P_H
#define QCONTACTDETAILRANGEFILTER_P_H

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

#include "qcontactfilter_p.h"
#include "qcontactfilter.h"

#include "qcontactdetailrangefilter.h"

#include <QString>
#include <QVariant>

QTM_BEGIN_NAMESPACE

class QContactDetailRangeFilterPrivate : public QContactFilterPrivate
{
public:
    QContactDetailRangeFilterPrivate()
        : QContactFilterPrivate(),
        m_flags(0),
        m_rangeflags(0)
    {
    }

    QContactDetailRangeFilterPrivate(const QContactDetailRangeFilterPrivate& other)
        : QContactFilterPrivate(other),
        m_defId(other.m_defId),
        m_fieldId(other.m_fieldId),
        m_minValue(other.m_minValue),
        m_maxValue(other.m_maxValue),
        m_flags(other.m_flags),
        m_rangeflags(other.m_rangeflags)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactDetailRangeFilterPrivate *od = static_cast<const QContactDetailRangeFilterPrivate*>(other);
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

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactDetailRangeFilter, QContactFilter::ContactDetailRangeFilter)

    QString m_defId;
    QString m_fieldId;
    QVariant m_minValue;
    QVariant m_maxValue;
    QContactFilter::MatchFlags m_flags;
    QContactDetailRangeFilter::RangeFlags m_rangeflags;
};

QTM_END_NAMESPACE

#endif
