/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECONTACTRELATIONSHIP_P_H
#define QDECLARATIVECONTACTRELATIONSHIP_P_H

#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>

#include "qcontactrelationship.h"

QTM_USE_NAMESPACE

class QDeclarativeContactRelationship : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QContactLocalId first READ first WRITE setFirst NOTIFY valueChanged)
    Q_PROPERTY(QContactLocalId second READ second WRITE setSecond NOTIFY valueChanged)
    Q_PROPERTY(QVariant type READ relationshipType WRITE setRelationshipType NOTIFY valueChanged)
    Q_ENUMS(RelationshipRole)
    Q_ENUMS(RelationshipType)
public:
    enum RelationshipRole {
        First = QContactRelationship::First,
        Second = QContactRelationship::Second,
        Either = QContactRelationship::Either
    };

    enum RelationshipType {
        Unknown = 0,
        HasMember,
        Aggregates,
        IsSameAs,
        HasAssistant,
        HasManager,
        HasSpouse
    };

    QDeclarativeContactRelationship(QObject* parent = 0);

    QContactLocalId first() const;
    QContactLocalId second() const;
    QVariant relationshipType() const;

    void setFirst( QContactLocalId firstId);
    void setSecond( QContactLocalId secondId);
    void setRelationshipType(const QVariant& relationshipType);

    QContactRelationship relationship() const;
    void setRelationship(const QContactRelationship& relationship);

signals:
    void valueChanged();
private:
    QContactRelationship m_relationship;
};

QML_DECLARE_TYPE(QDeclarativeContactRelationship)

#endif

