/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCONTACTDETAILFILTER_P_H
#define QCONTACTDETAILFILTER_P_H

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

#include <QString>
#include <QVariant>

QTM_BEGIN_NAMESPACE

class QContactDetailFilterPrivate : public QContactFilterPrivate
{
public:
    QContactDetailFilterPrivate()
        : QContactFilterPrivate(),
        m_flags(0)
    {
    }

    QContactDetailFilterPrivate(const QContactDetailFilterPrivate& other)
        : QContactFilterPrivate(other),
        m_defId(other.m_defId),
        m_fieldId(other.m_fieldId),
        m_exactValue(other.m_exactValue),
        m_flags(other.m_flags)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactDetailFilterPrivate *od = static_cast<const QContactDetailFilterPrivate*>(other);
        if (m_defId != od->m_defId)
            return false;
        if (m_fieldId != od->m_fieldId)
            return false;
        if (m_exactValue != od->m_exactValue)
            return false;
        if (m_flags != od->m_flags)
            return false;
        return true;
    }

    QDataStream& outputToStream(QDataStream& stream) const
    {
        return stream << m_defId << m_fieldId << m_exactValue << (quint32)m_flags;
    }

    QDataStream& inputFromStream(QDataStream& stream)
    {
        quint32 flags;
        stream >> m_defId >> m_fieldId >> m_exactValue >> flags;
        m_flags = QContactFilter::MatchFlags(flags);
        return stream;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactDetailFilter, QContactFilter::ContactDetailFilter)

    QString m_defId;
    QString m_fieldId;
    QVariant m_exactValue;
    QContactFilter::MatchFlags m_flags;
};

QTM_END_NAMESPACE

#endif
