/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONTACTRELATIONSHIPFILTER_P_H
#define QCONTACTRELATIONSHIPFILTER_P_H

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
#include <QStringList>
#include <QList>

class QContactRelationshipFilterPrivate : public QContactFilterPrivate
{
public:
    QContactRelationshipFilterPrivate()
        : QContactFilterPrivate(),
        m_minimumPriority(-1),
        m_maximumPriority(-1)
    {
    }

    QContactRelationshipFilterPrivate(const QContactRelationshipFilterPrivate& other)
        : QContactFilterPrivate(other),
        m_leftIds(other.m_leftIds),
        m_rightIds(other.m_rightIds),
        m_leftManagerUris(other.m_leftManagerUris),
        m_rightManagerUris(other.m_rightManagerUris),
        m_relationshipTypes(other.m_relationshipTypes),
        m_minimumPriority(other.m_minimumPriority),
        m_maximumPriority(other.m_maximumPriority)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactRelationshipFilterPrivate *od = static_cast<const QContactRelationshipFilterPrivate*>(other);
        if (m_leftIds != od->m_leftIds)
            return false;
        if (m_rightIds != od->m_rightIds)
            return false;
        if (m_leftManagerUris != od->m_leftManagerUris)
            return false;
        if (m_rightManagerUris != od->m_rightManagerUris)
            return false;
        if (m_relationshipTypes != od->m_relationshipTypes)
            return false;
        if (m_minimumPriority != od->m_minimumPriority)
            return false;
        if (m_maximumPriority != od->m_maximumPriority)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactRelationshipFilter, QContactFilter::RelationshipFilter)

    QList<QUniqueId> m_leftIds;
    QList<QUniqueId> m_rightIds;
    QStringList m_leftManagerUris;
    QStringList m_rightManagerUris;
    QStringList m_relationshipTypes;
    int m_minimumPriority;
    int m_maximumPriority;
};

#endif
