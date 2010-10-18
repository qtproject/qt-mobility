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

QString QDeclarativeContactRelationship::relationshipType() const
{
    return m_relationship.relationshipType();
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

void QDeclarativeContactRelationship::setRelationshipType(const QString& relationshipType)
{
    m_relationship.setRelationshipType(relationshipType);
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
