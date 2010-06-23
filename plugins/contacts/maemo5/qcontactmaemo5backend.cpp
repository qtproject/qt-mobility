/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QTimer>

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
  QString label;
  
  // Try to get the display name from the OSSO-ABook Contact
  label = d->m_abook->getDisplayName(contact);
  
  // Synthesise the display label for not saved contacts
  // A. FirstName + LastName
  if (label.isEmpty()){
    QContactName name = contact.detail(QContactName::DefinitionName);
    QStringList nameList;
    
    nameList << name.firstName();
    if (name.lastName().count()){
      nameList << name.lastName();
    }
    
    label = nameList.join(QString(' '));
  }
  
  // B. Email
  if (label.isEmpty()){
    QContactEmailAddress email = contact.detail(QContactEmailAddress::DefinitionName);
    label = email.emailAddress();
  }
  
  // 
  if (label.isEmpty()){
    *error = QContactManager::UnspecifiedError;
    return QString("No name");
  }
  
  *error = QContactManager::NoError;
  return label;
}

bool QContactMaemo5Engine::validateContact(const QContact& contact, QContactManager::Error* error) const
{
    return QContactManagerEngine::validateContact(contact, error);
}

bool QContactMaemo5Engine::validateDefinition(const QContactDetailDefinition& definition, QContactManager::Error* error) const
{
    QContactDetailDefinition existing = detailDefinition(definition.name(), QContactType::TypeContact, error);
    if (existing == definition) {
        *error = QContactManager::NoError;
        return true;
    }

    *error = QContactManager::NotSupportedError; // XXX TODO: mutable definitions?
    return false;
}

QContact QContactMaemo5Engine::compatibleContact(const QContact& contact, QContactManager::Error* error) const
{
    return QContactManagerEngine::compatibleContact(contact, error);
}

QContactLocalId QContactMaemo5Engine::selfContactId(QContactManager::Error* error) const
{
  Q_CHECK_PTR(d->m_abook);

  return d->m_abook->selfContactId(error);
}

QList<QContactLocalId> QContactMaemo5Engine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
  Q_CHECK_PTR(d->m_abook);
  
  QList<QContactLocalId> rtn;

  // do this naively for now...
  QContactManager::Error tempError = QContactManager::NoError;
  QList<QContactLocalId> allIds = d->m_abook->contactIds(filter, sortOrders, error);
  QList<QContact> sortedAndFiltered;
  
  foreach (const QContactLocalId& currId, allIds) {
    QContact curr = contact(currId, QContactFetchHint(), &tempError);
    if (tempError != QContactManager::NoError)
      *error = tempError;
    if (QContactManagerEngine::testFilter(filter, curr)) {
      QContactManagerEngine::addSorted(&sortedAndFiltered, curr, sortOrders);
    }
  }

  foreach (const QContact& contact, sortedAndFiltered) {
    rtn.append(contact.localId());
  }
  return rtn;
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
  
  //NOTE getQContact can't set the displayLabel
  QContact *contact = d->m_abook->getQContact(contactId, error);
  QContact rtn(*contact);
  delete contact;
  if (*error == QContactManager::NoError) {
    setContactDisplayLabel(&rtn, synthesizedDisplayLabel(rtn, error));
    QContactId cid;
    cid.setLocalId(contactId);
    cid.setManagerUri(managerUri());
    rtn.setId(cid);
  }
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
            if (errorMap)
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
            if (errorMap)
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
  setContactDisplayLabel(contact, synthesizedDisplayLabel(*contact, error));

  // ensure that the contact's details conform to their definitions
  if (!validateContact(*contact, error)) {
    QCM5_DEBUG << "Validate Contact failed";
    return false;
  }

  bool retn = d->m_abook->saveContact(contact, error);
  QContactId cId = contact->id();
  cId.setManagerUri(managerUri());
  contact->setId(cId);
  return retn;
}

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

    // XXX NOTE: only QContactPhoneNumber, QContactOnlineAccount,
    // QContactEmailAddress and QContactAddress are currently non-unique.
    
    // QContactAddress
    fields = defns[contactType][QContactAddress::DefinitionName].fields();
    fields.remove(QContactAddress::FieldSubTypes);
    fields.insert(AddressFieldExtension, gsfd);
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    defns[contactType][QContactAddress::DefinitionName].setFields(fields);
    defns[contactType][QContactAddress::DefinitionName].setUnique(false);
    
    // No QContactAnniversary
    defns[contactType].remove(QContactAnniversary::DefinitionName);
    
    // No QContactAvatar
    defns[contactType].remove(QContactAvatar::DefinitionName);

    // QContactBirthday
    fields = defns[contactType][QContactBirthday::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactBirthday::DefinitionName].setFields(fields);
    defns[contactType][QContactBirthday::DefinitionName].setUnique(true);

    // QContactDisplayLabel
    fields = defns[contactType][QContactDisplayLabel::DefinitionName].fields();
    defns[contactType][QContactDisplayLabel::DefinitionName].setFields(fields);
    defns[contactType][QContactDisplayLabel::DefinitionName].setUnique(true);
    
    // QContactEmailAddress
    fields = defns[contactType][QContactEmailAddress::DefinitionName].fields();
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    defns[contactType][QContactEmailAddress::DefinitionName].setFields(fields);
    defns[contactType][QContactEmailAddress::DefinitionName].setUnique(false);
    
    // QContactFamily
    fields = defns[contactType][QContactFamily::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactFamily::DefinitionName].setFields(fields);
    defns[contactType][QContactFamily::DefinitionName].setUnique(true);

    // QContactGender
    fields = defns[contactType][QContactGender::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactGender::DefinitionName].setFields(fields);
    defns[contactType][QContactGender::DefinitionName].setUnique(true);

    // No QContactGeoLocation
    defns[contactType].remove(QContactGeoLocation::DefinitionName);
    
    // QContactGuid
    fields = defns[contactType][QContactGuid::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactGuid::DefinitionName].setFields(fields);
    defns[contactType][QContactGuid::DefinitionName].setUnique(true);
  
    // No QContactGlobalPresence
    defns[contactType].remove(QContactGlobalPresence::DefinitionName);

    // QContactName
    fields = defns[contactType][QContactName::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    fields.remove(QContactName::FieldCustomLabel);
    fields.remove(QContactName::FieldMiddleName);
    fields.remove(QContactName::FieldPrefix);
    fields.remove(QContactName::FieldSuffix);
    defns[contactType][QContactName::DefinitionName].setFields(fields);
    defns[contactType][QContactName::DefinitionName].setUnique(true);
    
    // QContactNickname
    fields = defns[contactType][QContactNickname::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactNickname::DefinitionName].setFields(fields);
    defns[contactType][QContactNickname::DefinitionName].setUnique(true);

    // QContactNote
    fields = defns[contactType][QContactNote::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactNote::DefinitionName].setFields(fields);
    defns[contactType][QContactNote::DefinitionName].setUnique(true);

    // QContactOnlineAccount
    fields = defns[contactType][QContactOnlineAccount::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    fields.remove(QContactOnlineAccount::FieldAccountUri);
    fields.remove(QContactOnlineAccount::FieldSubTypes);
    fields.insert("AccountPath", gsfd);
    defns[contactType][QContactOnlineAccount::DefinitionName].setFields(fields);
    defns[contactType][QContactOnlineAccount::DefinitionName].setUnique(false);
    
    // QContactOrganization
    fields = defns[contactType][QContactOrganization::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    fields.remove(QContactOrganization::FieldAssistantName);
    fields.remove(QContactOrganization::FieldDepartment);
    fields.remove(QContactOrganization::FieldLocation);
    fields.remove(QContactOrganization::FieldLogoUrl);
    fields.remove(QContactOrganization::FieldTitle);
    fields.remove(QContactOrganization::FieldRole);
    defns[contactType][QContactOrganization::DefinitionName].setFields(fields);
    defns[contactType][QContactOrganization::DefinitionName].setUnique(true);
    
    // QContactPhoneNumber
    fields = defns[contactType][QContactPhoneNumber::DefinitionName].fields();
    fields.insert(QContactDetail::FieldDetailUri, gsfd);
    QContactDetailFieldDefinition phoneSubtype = fields.value(QContactPhoneNumber::FieldSubTypes);
    QVariantList allowableValues;
    allowableValues << QContactPhoneNumber::SubTypeMobile;
    allowableValues << QContactPhoneNumber::SubTypeVoice;
    phoneSubtype.setAllowableValues(allowableValues);
    fields.insert(QContactPhoneNumber::FieldSubTypes, phoneSubtype);
    defns[contactType][QContactPhoneNumber::DefinitionName].setFields(fields);
    defns[contactType][QContactPhoneNumber::DefinitionName].setUnique(false);

    // No QContactPresence
    defns[contactType].remove(QContactPresence::DefinitionName);

    // No QContactRingtone
    defns[contactType].remove(QContactRingtone::DefinitionName);
    
    // No QContactSyncTarget
    defns[contactType].remove(QContactSyncTarget::DefinitionName);

    // No QContactTag
    defns[contactType].remove(QContactTag::DefinitionName);
    
    // QContactTimestamp
    fields = defns[contactType][QContactTimestamp::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactTimestamp::DefinitionName].setFields(fields);

    // QContactType
    fields = defns[contactType][QContactType::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactType::DefinitionName].setFields(fields);

    // QContactUrl
    fields = defns[contactType][QContactUrl::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    fields.remove(QContactUrl::FieldSubType);
    defns[contactType][QContactUrl::DefinitionName].setFields(fields);
    defns[contactType][QContactUrl::DefinitionName].setUnique(true);
    
    QCM5_DEBUG << "Contact type" << contactType << "Keys" <<  defns.keys();
    
    *error = QContactManager::NoError;
    return defns[contactType];
}

QContactDetailDefinition QContactMaemo5Engine::detailDefinition(const QString& definitionName, const QString& contactType, QContactManager::Error* error) const
{
    return QContactManagerEngine::detailDefinition(definitionName, contactType, error);
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
    default:
      return false;
  }
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

void QContactMaemo5Engine::requestDestroyed(QContactAbstractRequest* req){
  m_asynchronousOperations.removeOne(req);
}

bool QContactMaemo5Engine::startRequest(QContactAbstractRequest* req){
  if (!m_asynchronousOperations.contains(req))
    m_asynchronousOperations.enqueue(req);
  updateRequestState(req, QContactAbstractRequest::ActiveState);
  QTimer::singleShot(0, this, SLOT(performAsynchronousOperation()));
  return true;
}

bool QContactMaemo5Engine::cancelRequest(QContactAbstractRequest* req){
  updateRequestState(req, QContactAbstractRequest::CanceledState);
  return true;
}

bool QContactMaemo5Engine::waitForRequestProgress(QContactAbstractRequest* req, int msecs){
  Q_UNUSED(msecs);
  
  if (!m_asynchronousOperations.removeOne(req))
    return false; // didn't exist.

  // replace at head of queue
  m_asynchronousOperations.insert(0, req);

  // and perform the operation.
  performAsynchronousOperation();

  return true;
}

bool QContactMaemo5Engine::waitForRequestFinished(QContactAbstractRequest* req, int msecs){
  // in our implementation, we always complete any operation we start.
  // so, waitForRequestFinished is equivalent to waitForRequestProgress.
  return waitForRequestProgress(req, msecs);
}

void QContactMaemo5Engine::performAsynchronousOperation(){
  QContactAbstractRequest *currentRequest;

  // take the first pending request and finish it
  if (m_asynchronousOperations.isEmpty())
    return;
  currentRequest = m_asynchronousOperations.dequeue();

  // check to see if it is cancelling; if so, remove it from the queue and return.
  if (currentRequest->state() == QContactAbstractRequest::CanceledState)
    return;

  // Now perform the active request and emit required signals.
  Q_ASSERT(currentRequest->state() == QContactAbstractRequest::ActiveState);
  switch (currentRequest->type()) {
    case QContactAbstractRequest::ContactFetchRequest:
    {
      QContactFetchRequest* r = static_cast<QContactFetchRequest*>(currentRequest);
      QContactFilter filter = r->filter();
      QList<QContactSortOrder> sorting = r->sorting();
      QContactFetchHint fh = r->fetchHint();

      QContactManager::Error operationError;
      QList<QContact> requestedContacts = contacts(filter, sorting, fh, &operationError);

      // update the request with the results.
      updateContactFetchRequest(r, requestedContacts, operationError, QContactAbstractRequest::FinishedState); // emit resultsAvailable()
    } break;
    case QContactAbstractRequest::ContactLocalIdFetchRequest:
    {
      QContactLocalIdFetchRequest* r = static_cast<QContactLocalIdFetchRequest*>(currentRequest);
      QContactFilter filter = r->filter();
      QList<QContactSortOrder> sorting = r->sorting();

      QContactManager::Error operationError = QContactManager::NoError;
      QList<QContactLocalId> requestedContactIds = contactIds(filter, sorting, &operationError);

      updateContactLocalIdFetchRequest(r, requestedContactIds, operationError, QContactAbstractRequest::FinishedState);
    } break;
    case QContactAbstractRequest::ContactSaveRequest:
    {
      QContactSaveRequest* r = static_cast<QContactSaveRequest*>(currentRequest);
      QList<QContact> contacts = r->contacts();

      QContactManager::Error operationError = QContactManager::NoError;
      QMap<int, QContactManager::Error> errorMap;
      saveContacts(&contacts, &errorMap, &operationError);

      updateContactSaveRequest(r, contacts, operationError, errorMap, QContactAbstractRequest::FinishedState); // there will always be results of some form.  emit resultsAvailable().
     } break;
     case QContactAbstractRequest::ContactRemoveRequest:
     {
       // this implementation provides scant information to the user
       // the operation either succeeds (all contacts matching the filter were removed)
       // or it fails (one or more contacts matching the filter could not be removed)
       // if a failure occurred, the request error will be set to the most recent
       // error that occurred during the remove operation.
       QContactRemoveRequest* r = static_cast<QContactRemoveRequest*>(currentRequest);
       QContactManager::Error operationError = QContactManager::NoError;
       QList<QContactLocalId> contactsToRemove = r->contactIds();
       QMap<int, QContactManager::Error> errorMap;

       for (int i = 0; i < contactsToRemove.size(); i++) {
         QContactManager::Error tempError;
         removeContact(contactsToRemove.at(i), /*changeSet,*/ &tempError);

         errorMap.insert(i, tempError);                
         if (tempError != QContactManager::NoError) {
           operationError = tempError;
         }
       }

       updateContactRemoveRequest(r, operationError, errorMap, QContactAbstractRequest::FinishedState);
     } break;
     case QContactAbstractRequest::DetailDefinitionFetchRequest:
     {
       QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(currentRequest);
       QContactManager::Error operationError = QContactManager::NoError;
       QMap<int, QContactManager::Error> errorMap;
       QMap<QString, QContactDetailDefinition> requestedDefinitions;
       QStringList names = r->definitionNames();
       if (names.isEmpty())
         names = detailDefinitions(r->contactType(), &operationError).keys(); // all definitions.

       QContactManager::Error tempError;
       for (int i = 0; i < names.size(); i++) {
         QContactDetailDefinition current = detailDefinition(names.at(i), r->contactType(), &tempError);
         requestedDefinitions.insert(names.at(i), current);

         errorMap.insert(i, tempError);              
         if (tempError != QContactManager::NoError) {
           operationError = tempError;
         }
       }

       updateDefinitionFetchRequest(r, requestedDefinitions, operationError, errorMap, QContactAbstractRequest::FinishedState);
     } break;
     default:
     {
       // symbian engine currently does not support mutable definitions.
     } break;

    
  } 
}

