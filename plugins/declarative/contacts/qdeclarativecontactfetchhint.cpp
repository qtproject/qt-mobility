#include "qdeclarativecontactfetchhint_p.h"

QDeclarativeContactFetchHint::QDeclarativeContactFetchHint(QObject* parent)
    :QObject(parent)
{
}

QStringList QDeclarativeContactFetchHint::detailDefinitionsHint() const
{
    return m_fetchHint.detailDefinitionsHint();
}
void QDeclarativeContactFetchHint::setDetailDefinitionsHint(const QStringList& definitionNames)
{
    m_fetchHint.setDetailDefinitionsHint(definitionNames);
}

QStringList QDeclarativeContactFetchHint::relationshipTypesHint() const
{
    return m_fetchHint.relationshipTypesHint();
}
void QDeclarativeContactFetchHint::setRelationshipTypesHint(const QStringList& relationshipTypes)
{
    m_fetchHint.setRelationshipTypesHint(relationshipTypes);
}

QSize QDeclarativeContactFetchHint::preferredImageSize() const
{
    return m_fetchHint.preferredImageSize();
}
void QDeclarativeContactFetchHint::setPreferredImageSize(const QSize& size)
{
    m_fetchHint.setPreferredImageSize(size);
}


QDeclarativeContactFetchHint::OptimizationHints QDeclarativeContactFetchHint::optimizationHints() const
{
    switch (m_fetchHint.optimizationHints()) {
    case QContactFetchHint::NoRelationships:
        return QDeclarativeContactFetchHint::NoRelationships;
    case QContactFetchHint::NoActionPreferences:
        return QDeclarativeContactFetchHint::NoActionPreferences;
    case QContactFetchHint::NoBinaryBlobs:
        return QDeclarativeContactFetchHint::NoBinaryBlobs;
    }
    return QDeclarativeContactFetchHint::AllRequired;
}
void QDeclarativeContactFetchHint::setOptimizationHints(QDeclarativeContactFetchHint::OptimizationHints hints)
{
    switch (hints) {
    case QDeclarativeContactFetchHint::NoRelationships:
        m_fetchHint.setOptimizationHints(QContactFetchHint::NoRelationships);
    case QDeclarativeContactFetchHint::NoActionPreferences:
        m_fetchHint.setOptimizationHints(QContactFetchHint::NoActionPreferences);
    case QDeclarativeContactFetchHint::NoBinaryBlobs:
        m_fetchHint.setOptimizationHints(QContactFetchHint::NoBinaryBlobs);
    }
    m_fetchHint.setOptimizationHints(QContactFetchHint::AllRequired);
}

QContactFetchHint QDeclarativeContactFetchHint::fetchHint() const
{
    return m_fetchHint;
}

void QDeclarativeContactFetchHint::setFetchHint(const QContactFetchHint& fetchHint)
{
    m_fetchHint = fetchHint;
    emit fetchHintChanged();
}
