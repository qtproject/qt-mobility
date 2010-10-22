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
  \o Relationship.HasMember
  \o Relationship.Aggregates
  \o Relationship.IsSameAs
  \o Relationship.HasAssistant
  \o Relationship.HasManager
  \o Relationship.HasSpouse
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
