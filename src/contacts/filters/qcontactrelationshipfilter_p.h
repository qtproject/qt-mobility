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
#include "qcontactid.h"
#include "qcontactrelationship.h"

#include "qcontactrelationshipfilter.h"

#include <QString>
#include <QStringList>
#include <QList>

QTM_BEGIN_NAMESPACE

class QContactRelationshipFilterPrivate : public QContactFilterPrivate
{
public:
    QContactRelationshipFilterPrivate()
        : QContactFilterPrivate(),
        m_relatedContactRole(QContactRelationship::Either)
    {
    }

    QContactRelationshipFilterPrivate(const QContactRelationshipFilterPrivate& other)
        : QContactFilterPrivate(other),
        m_relationshipType(other.m_relationshipType),
        m_relatedContactId(other.m_relatedContactId),
        m_relatedContactRole(other.m_relatedContactRole)
    {
    }

    virtual bool compare(const QContactFilterPrivate* other) const
    {
        const QContactRelationshipFilterPrivate *od = static_cast<const QContactRelationshipFilterPrivate*>(other);
        if (m_relatedContactRole != od->m_relatedContactRole)
            return false;
        if (m_relatedContactId != od->m_relatedContactId)
            return false;
        if (m_relationshipType != od->m_relationshipType)
            return false;
        return true;
    }

    Q_IMPLEMENT_CONTACTFILTER_VIRTUALCTORS(QContactRelationshipFilter, QContactFilter::RelationshipFilter)

    QString m_relationshipType;
    QContactId m_relatedContactId;
    QContactRelationship::Role m_relatedContactRole;
};

QTM_END_NAMESPACE

#endif
