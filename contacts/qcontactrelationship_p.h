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

#ifndef QCONTACTRELATIONSHIP_P_H
#define QCONTACTRELATIONSHIP_P_H

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

#include <QString>
#include <QSharedData>

#include "qtcontactsglobal.h"

class QContactRelationshipPrivate : public QSharedData
{
public:
    QContactRelationshipPrivate()
            : QSharedData(),
            m_leftId(0),
            m_rightId(0),
            m_priority(0)
    {
    }

    QContactRelationshipPrivate(const QContactRelationshipPrivate& other)
            : QSharedData(other),
            m_leftId(other.m_leftId),
            m_rightId(other.m_rightId),
            m_leftManagerUri(other.m_leftManagerUri),
            m_rightManagerUri(other.m_rightManagerUri),
            m_relationshipType(other.m_relationshipType),
            m_priority(other.m_priority)
    {
    }

    ~QContactRelationshipPrivate()
    {
    }

    QUniqueId m_leftId;
    QUniqueId m_rightId;
    QString m_leftManagerUri;
    QString m_rightManagerUri;
    QString m_relationshipType;
    int m_priority;
};

#endif
