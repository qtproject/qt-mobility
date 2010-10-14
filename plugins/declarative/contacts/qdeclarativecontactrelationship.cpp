#include "qdeclarativecontactrelationship_p.h"


QDeclarativeContactRelationship::QDeclarativeContactRelationship(QObject* parent)
    :QObject(parent)
{

}

int QDeclarativeContactRelationship::first() const
{
    return m_relationship.first().localId();
}

int  QDeclarativeContactRelationship::second() const
{
    return m_relationship.second().localId();
}

QDeclarativeContactRelationship::Type QDeclarativeContactRelationship::relationshipType() const
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
    return QDeclarativeContactRelationship::InvalidType;
}

void QDeclarativeContactRelationship::setFirst(int firstId)
{
    QContactId id;
    id.setLocalId(firstId);
    m_relationship.setFirst(id);
}

void QDeclarativeContactRelationship::setSecond(int secondId)
{
    QContactId id;
    id.setLocalId(secondId);
    m_relationship.setSecond(id);
}

void QDeclarativeContactRelationship::setRelationshipType(QDeclarativeContactRelationship::Type relationshipType)
{
    switch (relationshipType) {
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
        break;
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
