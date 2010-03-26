/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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

#include "qcontactmaemo5backend_p.h"

#include <QSharedData>

#include "qcontactmaemo5debug_p.h"

DEFINE_GLOBAL_DEBUG_VAR

QContactManagerEngine* ContactMaemo5Factory::engine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    initDebugLogger();
    return new QContactMaemo5Engine(); //FIXME Wonderfull memory leak :D
}

QString ContactMaemo5Factory::managerName() const
{
    return QString("maemo5");
}

Q_EXPORT_PLUGIN2(qtcontacts_maemo5, ContactMaemo5Factory);

/*!
  \class QContactMaemo5Engine
  \brief The QContactMaemo5Engine class provides an implementation of
  QContactManagerEngine whose functions always return an error.
  
  The Maemo5 engine.
 */

/*! Constructs a new invalid contacts backend. */
QContactMaemo5Engine::QContactMaemo5Engine() : d(new QContactMaemo5EngineData)
{
  QContactABook *abook = d->m_abook;
  connect(abook, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), SIGNAL(contactsAdded(const QList<QContactLocalId>&)));
  connect(abook, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), SIGNAL(contactsChanged(const QList<QContactLocalId>&)));
  connect(abook, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), SIGNAL(contactsRemoved(const QList<QContactLocalId>&)));
}

/*! \reimp */
QContactMaemo5Engine& QContactMaemo5Engine::operator=(const QContactMaemo5Engine& other)
{
    d = other.d;
    return *this;
}

/*! \reimp */
QString QContactMaemo5Engine::managerName() const
{
    return QString(QLatin1String("maemo5"));
}

/* Synthesise the display label of a contact */
QString QContactMaemo5Engine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const
{
  Q_UNUSED(error)
  QString label = QContactManagerEngine::synthesizedDisplayLabel(contact, error);
  
  if (label.isEmpty()) {
    QContactNickname n = contact.detail(QContactNickname::DefinitionName);
    label = n.nickname();
  }
  
  if (label.isEmpty())
    label = "No name";
  
  return label;
}

bool validateContact(const QContact& contact, QContactManager::Error* error) const
{
    return QContactManagerEngine::validateContact(contact, error);
}

bool validateDefinition(const QContactDetailDefinition& detailDefinition, QContactManager::Error* error) const
{
    QContactDetailDefinition existing = detailDefinition(detailDefinition.name());
    if (existing != detailDefinition) {
        *error = QContactManager::NotSupportedError; // XXX TODO: mutable definitions?
        return false;
    }

    *error = QContactManager::NoError;
    return true;
}

QContactLocalId QContactMaemo5Engine::selfContactId(QContactManager::Error* error) const
{
  Q_CHECK_PTR(d->m_abook);

  return d->m_abook->selfContactId(error);
}

QList<QContactLocalId> QContactMaemo5Engine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
  Q_CHECK_PTR(d->m_abook);
  
  //return QContactManagerEngine::contactIds(filter, sortOrders, error);
  return d->m_abook->contactIds(filter, sortOrders, error);
}

QList<QContact> QContactMaemo5Engine::contacts(const QContactFilter & filter, const QList<QContactSortOrder> & sortOrders, const QContactFetchHint & fetchHint,
			  QContactManager::Error* error ) const
{
  Q_UNUSED(fetchHint); // no optimisations currently, ignore the fetchhint.
  Q_CHECK_PTR(d->m_abook);
  QList<QContact> rtn;
  
  QList<QContactLocalId> ids = contactIds(filter, sortOrders,error);
  foreach (QContactLocalId id, ids)
    rtn << contact(id, QContactFetchHint(), error);
  return rtn;
}

QContact QContactMaemo5Engine::contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
  Q_UNUSED(fetchHint); //TODO
  Q_CHECK_PTR(d->m_abook);
  
  QContact *contact = d->m_abook->getQContact(contactId, error);
  
  QString displayLabel = synthesizedDisplayLabel(*contact, error);
  
  QContact rtn = setContactDisplayLabel(displayLabel, *contact);
  delete contact;
  
  return rtn;
}

bool QContactMaemo5Engine::saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    QContactManager::Error tempError = QContactManager::NoError;
    QContact curr;
    for (int i = 0; i < contacts->size(); i++) {
        curr = contacts->at(i);
        if (!saveContact(&curr, &tempError)) {
            errorMap->insert(i, tempError);
            *error = tempError;
        } else {
            contacts->replace(i, curr);
        }
    }

    return (*error == QContactManager::NoError);
}

bool QContactMaemo5Engine::removeContacts(const QList<QContactLocalId>& ids, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    QContactManager::Error tempError = QContactManager::NoError;
    QContact curr;
    for (int i = 0; i < ids.size(); i++) {
        if (!removeContact(ids.at(i), &tempError)) {
            errorMap->insert(i, tempError);
            *error = tempError;
        }
    }

    return (*error == QContactManager::NoError);
}

bool QContactMaemo5Engine::saveContact(QContact* contact, QContactManager::Error* error)
{
  Q_CHECK_PTR(d->m_abook);
  
  if (!contact) {
    *error = QContactManager::BadArgumentError;
    return false;
  }

  // synthesize the display label for the contact
  *contact = setContactDisplayLabel(synthesizedDisplayLabel(*contact, error), *contact);
  
  // ensure that the contact's details conform to their definitions
  if (!validateContact(*contact, error)) {
    QCM5_DEBUG << "Validate Contact failed";
    *error = QContactManager::InvalidDetailError;
    return false;
  }
  return d->m_abook->saveContact(contact, error);
}

#if 0
QList<QContactManager::Error> QContactMaemo5Engine::removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error* error)
{
  bool ok = true;
  
  if (contactIds->isEmpty())
    return false;
  
  QContactLocalId id;
  foreach(id, contactIds){
      if (!removeContact(id, error))
	ok = false;
  }
  
  return ok;
}
#endif

bool QContactMaemo5Engine::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
  Q_CHECK_PTR(d->m_abook);
  return d->m_abook->removeContact(contactId, error);
}

QMap<QString, QContactDetailDefinition> QContactMaemo5Engine::detailDefinitions(const QString& contactType, QContactManager::Error* error) const
{
    QMap<QString, QMap<QString, QContactDetailDefinition> > defns = QContactManagerEngine::schemaDefinitions();
    QMap<QString, QContactDetailFieldDefinition> fields;
    
    QContactDetailFieldDefinition gsfd; //Generic string field definition
    gsfd.setDataType(QVariant::String);
    
    // QContactAddress
    fields = defns[contactType][QContactAddress::DefinitionName].fields();
    //fields.remove(QContactAddress::FieldSubTypes);
    fields.insert("Estension", gsfd);
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    defns[contactType][QContactAddress::DefinitionName].setFields(fields);
    
    // QContactAnniversary
    defns[contactType].remove(QContactAnniversary::DefinitionName);
    
    // QContactAvatar
    // QContactBirthday
    // QContactDisplayLabel
    // QContactEmailAddress
    fields = defns[contactType][QContactEmailAddress::DefinitionName].fields();
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    defns[contactType][QContactEmailAddress::DefinitionName].setFields(fields);
    
    // QContactFamily
    // QContactGender
    // QContactGeoLocation
    defns[contactType].remove(QContactGeoLocation::DefinitionName);
    
    // QContactGuid
    // QContactName
    // QContactNickname
    // QContactNote
    // QContactOnlineAccount
    fields = defns[contactType][QContactOnlineAccount::DefinitionName].fields();
    fields.remove(QContactOnlineAccount::FieldAccountUri);
    fields.remove(QContactOnlineAccount::FieldSubTypes);
    fields.insert("AccountPath", gsfd);
    defns[contactType][QContactOnlineAccount::DefinitionName].setFields(fields);
    
    // QContactOrganization
    fields = defns[contactType][QContactOrganization::DefinitionName].fields();
    fields.remove(QContactOrganization::FieldAssistantName);
    fields.remove(QContactOrganization::FieldDepartment);
    fields.remove(QContactOrganization::FieldLocation);
    fields.remove(QContactOrganization::FieldLogoUrl);
    fields.remove(QContactOrganization::FieldName);
    fields.remove(QContactOrganization::FieldRole);
    defns[contactType][QContactOrganization::DefinitionName].setFields(fields);
    
    // QContactPhoneNumber
    fields = defns[contactType][QContactPhoneNumber::DefinitionName].fields();
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    defns[contactType][QContactPhoneNumber::DefinitionName].setFields(fields);
    
    // QContactSyncTarget
    defns[contactType].remove(QContactSyncTarget::DefinitionName);
    
    // QContactTimestamp
    // QContactType
    // QContactUrl
    fields = defns[contactType][QContactUrl::DefinitionName].fields();
    fields.remove(QContactUrl::FieldSubType);
    defns[contactType][QContactUrl::DefinitionName].setFields(fields);
  
    *error = QContactManager::NoError;
    return defns[contactType];
}

QContactDetailDefinition detailDefinition(const QString& definitionName, const QString& contactType, QContactManager::Error* error) const
{
    QMap<QString, QContactDetailDefinition> defMap = detailDefinitions(contactType, error);
    if (!defMap.contains(definitionName)) {
        *error = QContactManager::DoesNotExistError;
        return QContactDetailDefinition();
    }

    *error = QContactManager::NoError;
    return defMap.value(contactType);
}

bool QContactMaemo5Engine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const {
  Q_UNUSED(contactType);
  if (feature == QContactManager::Anonymous)
    return true;
  
  return false;
}

bool QContactMaemo5Engine::isFilterSupported(const QContactFilter& filter) const {
  switch (filter.type()) {
    case QContactFilter::InvalidFilter:
    case QContactFilter::DefaultFilter:
    case QContactFilter::LocalIdFilter:
    case QContactFilter::ContactDetailFilter:
    case QContactFilter::ActionFilter:
    case QContactFilter::IntersectionFilter:
    case QContactFilter::UnionFilter:
      return true;
  }
  return false;
}

QList<QVariant::Type> QContactMaemo5Engine::supportedDataTypes() const {
  QList<QVariant::Type> st;
  st.append(QVariant::String);
  st.append(QVariant::Int);
  st.append(QVariant::UInt);
  st.append(QVariant::Double);
  st.append(QVariant::Date);
  st.append(QVariant::DateTime);

  return st;   
}
