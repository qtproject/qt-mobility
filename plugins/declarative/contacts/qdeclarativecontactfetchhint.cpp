#include "qdeclarativecontactfetchhint_p.h"



/*!
   \qmlclass FetchHint QDeclarativeContactFetchHint
   \brief The FetchHint element provides hints to the manager about which contact
  information needs to be retrieved in an asynchronous fetch request or a synchronous
  function call.

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactFetchHint
 */

QDeclarativeContactFetchHint::QDeclarativeContactFetchHint(QObject* parent)
    :QObject(parent)
{
}

/*!
  \qmlproperty list<string> FetchHint::detailDefinitionsHint

  This property holds a list of definition names that identify detail definitions of which details
  the manager should (at a minimum) retrieve when fetching contacts.
  */
QStringList QDeclarativeContactFetchHint::detailDefinitionsHint() const
{
    return m_fetchHint.detailDefinitionsHint();
}
void QDeclarativeContactFetchHint::setDetailDefinitionsHint(const QStringList& definitionNames)
{
    m_fetchHint.setDetailDefinitionsHint(definitionNames);
}

/*!
  \qmlproperty list<string> FetchHint::relationshipTypesHint

  This property holds a list of relationship types that the manager should (at a minimum) retrieve
  when fetching contacts.
  */
QStringList QDeclarativeContactFetchHint::relationshipTypesHint() const
{
    return m_fetchHint.relationshipTypesHint();
}
void QDeclarativeContactFetchHint::setRelationshipTypesHint(const QStringList& relationshipTypes)
{
    m_fetchHint.setRelationshipTypesHint(relationshipTypes);
}

/*!
  \qmlproperty size FetchHint::preferredImageSize

  This property holds the preferred pixel dimensions for any images returned
  by the manager for a given request.  This hint may be ignored by the manager.
  */
QSize QDeclarativeContactFetchHint::preferredImageSize() const
{
    return m_fetchHint.preferredImageSize();
}
void QDeclarativeContactFetchHint::setPreferredImageSize(const QSize& size)
{
    m_fetchHint.setPreferredImageSize(size);
}

/*!
  \qmlproperty FetchHint::OptimizationHints  FetchHint::optimizationHints

  This property holds the optimization hint flags specified by the client.
  These hints may be ignored by the backend, in which case it will return
  the full set of information accessible in a contact, including
  relationships, action preferences, and binary blobs.
  */
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
