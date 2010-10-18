#include "qdeclarativecontactrelationship_p.h"
#include <QDebug>

QDeclarativeContactRelationship::QDeclarativeContactRelationship(QObject* parent)
    :QObject(parent)
{

}

QContactLocalId QDeclarativeContactRelationship::first() const
{
    return m_relationship.first().localId();
}

QContactLocalId  QDeclarativeContactRelationship::second() const
{
    return m_relationship.second().localId();
}

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
