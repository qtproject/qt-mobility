/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#include "qdeclarativecontactrelationship_p.h"
#include <QDebug>

/*!
   \qmlclass Relationship QDeclarativeContactRelationship
   \brief The Relationship element describes a one-to-one relationship
  between a locally-stored contact and another (possibly remote) contact.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactRelationship
   \sa RelationshipModel
 */






QDeclarativeContactRelationship::QDeclarativeContactRelationship(QObject* parent)
    :QObject(parent)
{

}


/*!
  \qmlproperty int Relationship::first

  This property holds the id of the locally-stored contact which has a relationship of the given type with the second contact.
  */

QContactLocalId QDeclarativeContactRelationship::first() const
{
    return m_relationship.first().localId();
}

/*!
  \qmlproperty int Relationship::second

  This property holds the id of the contact with which the first contact has a relationship of the given type.
  */
QContactLocalId  QDeclarativeContactRelationship::second() const
{
    return m_relationship.second().localId();
}

/*!
  \qmlproperty int Relationship::type

  This property holds the type of relationship which the source contact has with the destination contacts.
  The value for this property can be one of:
  \list
  \a Relationship.HasMember
  \a Relationship.Aggregates
  \a Relationship.IsSameAs
  \a Relationship.HasAssistant
  \a Relationship.HasManager
  \a Relationship.HasSpouse
  \endlist
  or any other customized relationship type string.
  */

QVariant QDeclarativeContactRelationship::relationshipType() const
{
    QString type = m_relationship.relationshipType();
    if (type == QContactRelationship::HasAssistant)
        return QDeclarativeContactRelationship::HasAssistant;
    if (type == QContactRelationship::Aggregates)
        return QDeclarativeContactRelationship::Aggregates;
    if (type == QContactRelationship::HasManager)
        return QDeclarativeContactRelationship::HasManager;
    if (type == QContactRelationship::HasMember)
        return QDeclarativeContactRelationship::HasMember;
    if (type == QContactRelationship::HasSpouse)
        return QDeclarativeContactRelationship::HasSpouse;
    if (type == QContactRelationship::IsSameAs)
        return QDeclarativeContactRelationship::IsSameAs;
    //customized type
    return type;
}

void QDeclarativeContactRelationship::setFirst(QContactLocalId firstId)
{
    QContactId id;
    id.setLocalId(firstId);
    m_relationship.setFirst(id);
}

void QDeclarativeContactRelationship::setSecond(QContactLocalId secondId)
{
    QContactId id;
    id.setLocalId(secondId);
    m_relationship.setSecond(id);
}

void QDeclarativeContactRelationship::setRelationshipType(const QVariant& relationshipType)
{
    if (relationshipType.type() == QVariant::Double) {//numbers in qml are set to double, even it's integer
        switch (relationshipType.toInt()) {
        case QDeclarativeContactRelationship::HasMember:
            m_relationship.setRelationshipType(QContactRelationship::HasMember);
            break;
        case QDeclarativeContactRelationship::Aggregates:
            m_relationship.setRelationshipType(QContactRelationship::Aggregates);
            break;
        case QDeclarativeContactRelationship::IsSameAs:
            m_relationship.setRelationshipType(QContactRelationship::IsSameAs);
            break;
        case QDeclarativeContactRelationship::HasAssistant:
            m_relationship.setRelationshipType(QContactRelationship::HasAssistant);
            break;
        case QDeclarativeContactRelationship::HasManager:
            m_relationship.setRelationshipType(QContactRelationship::HasManager);
            break;
        case QDeclarativeContactRelationship::HasSpouse:
            m_relationship.setRelationshipType(QContactRelationship::HasSpouse);
            break;
        default:
            //unknown type
            qWarning() << "unknown relationship type:" << relationshipType;
            break;
        }
    } else {
        m_relationship.setRelationshipType(relationshipType.toString());
    }

}


QContactRelationship QDeclarativeContactRelationship::relationship() const
{
    return m_relationship;
}
void QDeclarativeContactRelationship::setRelationship(const QContactRelationship& relationship)
{
    m_relationship = relationship;
    emit valueChanged();
}
