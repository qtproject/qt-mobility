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


#ifndef QCONTACTRELATIONSHIP_H
#define QCONTACTRELATIONSHIP_H

#include <QString>
#include <QSharedDataPointer>

#include "qtcontactsglobal.h"

class QContactRelationshipPrivate;

class QTCONTACTS_EXPORT QContactRelationship
{
public:
#ifdef Q_QDOC
    const char* RelationshipTypeIsMemberOf;
    const char* RelationshipTypeIsAggregatedBy;
    const char* RelationshipTypeIs;
    const char* RelationshipTypeAssistant;
    const char* RelationshipTypeManager;
    const char* RelationshipTypeSpouse;
#else
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeIsMemberOf, "IsMemberOf");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeIsAggregatedBy, "IsAggregatedBy");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeIs, "Is");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeAssistant, "Assistant");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeManager, "Manager");
    Q_DECLARE_LATIN1_LITERAL(RelationshipTypeSpouse, "Spouse");
#endif

    QContactRelationship();
    ~QContactRelationship();

    QContactRelationship(const QContactRelationship& other);
    QContactRelationship& operator=(const QContactRelationship& other);
    bool operator==(const QContactRelationship &other) const;
    bool operator!=(const QContactRelationship &other) const { return !(*this==other); }

    QUniqueId leftId() const;
    QUniqueId rightId() const;
    QString leftManagerUri() const;
    QString rightManagerUri() const;
    QString relationshipType() const;
    int priority() const;

    void setLeftId(const QUniqueId& id);
    void setRightId(const QUniqueId& id);
    void setLeftManagerUri(const QString& uri = QString());
    void setRightManagerUri(const QString& uri = QString());
    void setRelationshipType(const QString& relationshipType);
    void setPriority(int priority);

private:
    QSharedDataPointer<QContactRelationshipPrivate> d;
};

#endif

