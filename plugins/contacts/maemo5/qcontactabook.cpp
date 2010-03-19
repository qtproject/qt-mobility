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
#include "qcontactabook_p.h"

#include <QEventLoop>
#include <libebook/e-book-util.h>


/* Error handling Macros */
#define FATAL_IF_ERROR(x) if(x) { \
                            QString message(x->message); \
                            g_error_free(x); \
                            qFatal(qPrintable(message)); \
                          }
#define WARNING_IF_ERROR(x) if(x) { \
                            QString message(x->message); \
                            g_error_free(x); \
                            qWarning(qPrintable(message)); \
                          }

/* Casting Macros */
#define A_CONTACT(x) reinterpret_cast<OssoABookContact*>(x)
#define A_ROSTER(x) reinterpret_cast<OssoABookRoster*>(x)
#define CONST_CHAR(x) static_cast<const char*>(x)
#define FREE(x) free((void*)x)

struct cbSharedData{
  QContactIDsHash* hash;
  QContactABook *that;
};

/* QContactABook */
QContactABook::QContactABook(QObject* parent) :QObject(parent)
{
  //Initialize QContactDetail context list
  initAddressBook();
}

QContactABook::~QContactABook()
{
  g_object_unref(m_abookAgregator);
  delete cbSD;
}

static void contactsAddedCB(OssoABookRoster *roster, OssoABookContact **contacts, gpointer data)
{
  QCM5_DEBUG << "CONTACT ADDED";
  Q_UNUSED(roster)
  
  cbSharedData* d = static_cast<cbSharedData*>(data);
  OssoABookContact **p;
  QList<QContactLocalId> contactIds;
  
  for (p = contacts; *p; ++p) {
    if (osso_abook_contact_is_roster_contact(*p))
      continue;
    
    // Add a new localID to the local ID hash
    const char* uid = CONST_CHAR(e_contact_get_const(E_CONTACT(*p), E_CONTACT_UID));
    QContactLocalId id = d->hash->append(uid);
    
    if (id)
      contactIds << id;
  }
  d->that->_contactsAdded(contactIds);
}

static void contactsChangedCB(OssoABookRoster *roster, OssoABookContact **contacts, gpointer data)
{
  QCM5_DEBUG << "CONTACT CHANGED";
  Q_UNUSED(roster)
  
  cbSharedData* d = static_cast<cbSharedData*>(data);
  OssoABookContact **p;
  QList<QContactLocalId> contactIds;
  
  for (p = contacts; *p; ++p) {
    if (osso_abook_contact_is_roster_contact(*p))
      continue;
    
    const char* uid = CONST_CHAR(e_contact_get_const(E_CONTACT(*p), E_CONTACT_UID));
    QContactLocalId id = d->hash->find(uid);
    //FREE(uid);
    if (id)
      contactIds << id;
  }
  d->that->_contactsChanged(contactIds);
}

static void contactsRemovedCB(OssoABookRoster *roster, const char **ids, gpointer data)
{
  QCM5_DEBUG << "CONTACT REMOVED";
  Q_UNUSED(roster)
  
  cbSharedData* d = static_cast<cbSharedData*>(data);
  const char **p;
  QList<QContactLocalId> contactIds;
  
  for (p = ids; *p; ++p) {
    QContactLocalId id = d->hash->take(*p);
    if (id)
      contactIds << id;
  }
  
  d->that->_contactsRemoved(contactIds);
}

void QContactABook::initAddressBook(){
  // Open AddressBook
  GError *gError = NULL;
  OssoABookRoster* roster = NULL;
  
  roster = osso_abook_aggregator_get_default(&gError);
  FATAL_IF_ERROR(gError)
  
  // Wait until is ready
  osso_abook_waitable_run((OssoABookWaitable *) roster,
			   g_main_context_default(),
			   &gError);
  FATAL_IF_ERROR(gError)
  
  if (!osso_abook_waitable_is_ready ((OssoABookWaitable *) roster, &gError))
    FATAL_IF_ERROR(gError)
  
  m_abookAgregator = reinterpret_cast<OssoABookAggregator*>(roster);
  
  initLocalIdHash();
  
  cbSD = new cbSharedData;
  cbSD->hash = &m_localIds;
  cbSD->that = this;
  
  //TODO Set up signals for added/changed eContact
  g_signal_connect(roster, "contacts-added",
                   G_CALLBACK (contactsAddedCB), cbSD);
  g_signal_connect(roster, "contacts-changed",
                   G_CALLBACK (contactsChangedCB), cbSD);
  g_signal_connect(roster, "contacts-removed",
                   G_CALLBACK (contactsRemovedCB), cbSD);
  
#if 0
  //TEST List of supported fields
  EBook *book = NULL;
  GList *l;
  book = osso_abook_roster_get_book(roster);
  e_book_get_supported_fields (book, &l, NULL);
  while (l) {
    qDebug() << "SUPPORTED FIELD" << (const char*)l->data;
    l = l->next;  
  }
  g_list_free(l);
#endif
}

/*! Fill LocalId Hash associating an internal QContactLocalId to any
 *  master contact ID.
 *  NOTE: master contact IDs are string like "1" or "osso-abook-tmc1".
 */
void QContactABook::initLocalIdHash()
{  
   GList *contactList = NULL;
   GList *node;
   
   contactList = osso_abook_aggregator_list_master_contacts(m_abookAgregator);

   if (!contactList) {
     qWarning() << "There are no Master contacts. LocalId hash is empty.";
     return;
   }
   
   for (node = contactList; node != NULL; node = g_list_next (node)) {
     EContact *contact = E_CONTACT(node->data);
     const char* data = CONST_CHAR(e_contact_get_const(contact, E_CONTACT_UID));
     QByteArray eContactUID(data);
     //FREE(data);
     m_localIds << eContactUID; //FIXME MemLeak
     QCM5_DEBUG << "eContactID " << eContactUID << "has been stored in m_localIDs with key" << m_localIds[eContactUID];
     
     // Useful for debugging.
     e_vcard_dump_structure((EVCard*)contact);
   }
   
   g_list_free(contactList);
}

QList<QContactLocalId> QContactABook::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
  QList<QContactLocalId> rtn;
  
  /* Sorting */
  //NOTE Native sorting is possible thanks to g_list_sort.
  //     It's limited just to one filter.
  //     Multi filters support need non native sorting.
  //     Native filtering needs a lot of coding since we need
  //     detailDefinitionName * detailFieldName functions
  //     to compare couple of contacts
  if (sortOrders.count()){
    QCM5_DEBUG << "Sorting...";
    // We don't need 
    // Fill Ids
    QList<QContactLocalId> Ids;
    {
      QList<QContactSortOrder> so;
      QContactManager::Error e;
      Ids = contactIds(filter, so, &e);
    }
      
    // Fill Contact List
    QList<QContact> contacts;
    foreach(QContactLocalId id, Ids){
      QContact *c;
      QContactManager::Error e;
      c = getQContact(id, &e);
      if (e == QContactManager::NoError)
	contacts << *c;
      else
        *error = e;
    }
    
    // Non native sorting
    return QContactMaemo5Engine::sortContactsTrampoline(contacts, sortOrders);
  }
  
  /* Matching action filter */
  //NOTE The code was not really tested */
  if(filter.type() == QContactFilter::ActionFilter){
    QContactActionFilter af(filter);
    /* This looks a bit strange for me */
    QList<QContactActionDescriptor> descriptors = QContactAction::actionDescriptors(af.actionName(), af.vendorName(), af.implementationVersion());

    GList *masterContacts = osso_abook_aggregator_list_master_contacts(m_abookAgregator);
    for(; masterContacts; ){
      OssoABookContact *masterContact = A_CONTACT(masterContacts->data);
      bool match = contactActionsMatch(masterContact, descriptors);
      if(!match) {
        GList *rosterContacts = osso_abook_contact_get_roster_contacts(masterContact);
        for(; rosterContacts && !match; ){
          OssoABookContact *rosterContact = A_CONTACT(rosterContacts->data);
          match = contactActionsMatch(rosterContact, descriptors);
          rosterContacts = g_list_delete_link(rosterContacts, rosterContacts);
        }
        g_list_free(rosterContacts);
      }
      if(match){
        EContact *contact = E_CONTACT(masterContact);
        const char* data = CONST_CHAR(e_contact_get_const(contact, E_CONTACT_UID));
        QByteArray localId(data);
        m_localIds << localId;
        rtn.append(m_localIds[localId]);
        QCM5_DEBUG << "eContactID " << localId << "has been stored in m_localIDs with key" << m_localIds[localId];
      }
      masterContacts = g_list_delete_link(masterContacts, masterContacts);
    }
    *error = QContactManager::NoError;
    return  rtn;
  }
  
  EBookQuery* query = convert(filter);
  
  GList* l = osso_abook_aggregator_find_contacts(m_abookAgregator, query);
  e_book_query_unref(query);
  
  while (l){
    EContact *contact = E_CONTACT(l->data);
    const char* data = CONST_CHAR(e_contact_get_const(contact, E_CONTACT_UID));
    QByteArray localId(data);
    m_localIds << localId;
    rtn.append(m_localIds[localId]);
    QCM5_DEBUG << "eContactID " << localId << "has been stored in m_localIDs with key" << m_localIds[localId];
    l = g_list_delete_link(l, l);
  }
  
  *error = QContactManager::NoError;
  return rtn;
}

QContact* QContactABook::getQContact(const QContactLocalId& contactId, QContactManager::Error* error) const
{
  QContact *rtn;
  OssoABookContact* aContact = getAContact(contactId);
  if (!aContact) {
    qWarning() << "Unable to get a valid AContact";
    *error = QContactManager::DoesNotExistError;
    return new QContact;
  }
  
  //Convert aContact => qContact
  rtn = convert(E_CONTACT(aContact));
  return rtn;
}

bool QContactABook::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
  Q_UNUSED(error);
  
  bool ok = false;
  OssoABookRoster* roster = A_ROSTER(m_abookAgregator);
  EBook *book = osso_abook_roster_get_book(roster);
  OssoABookContact* aContact = getAContact(contactId);
  
  // Delete the contact itself with all its roster contacts and photo.
  //NOTE This perform an async operation. If aContact exists, ok var will most probably be true.
  ok = osso_abook_contact_delete(aContact, book, NULL);
  
#if 0
  // This code works syncronously but doesn't remove any photo nor roster contacts
  const char *id = m_localIds[contactId];
  QCM5_DEBUG << "Deleting contact id:" << id;
  GError *gError = NULL;
  ok = e_book_remove_contact(book, id, &gError);
  WARNING_IF_ERROR(gError);
#endif  
  
  return ok;
}

struct svSharedData{
   QContactABook* that;
   bool *result;
};

static void commitContactCB(EBook* book, EBookStatus  status, gpointer user_data)
{
  Q_UNUSED(book)
  svSharedData *sd = static_cast<svSharedData*>(user_data);
  
  *sd->result = (status == E_BOOK_ERROR_OK) ? true : false;  
  sd->that->_savingJobFinished();
}

static void addContactCB(EBook* book, EBookStatus  status, const char  *uid, gpointer user_data)
{
  Q_UNUSED(uid);
  
  //osso_abook_contact_set_roster(OssoABookContact *contact, OssoABookRoster *roster)
  commitContactCB(book, status, user_data);
}

bool QContactABook::saveContact(QContact* contact, QContactManager::Error* error)
{
  QMutexLocker locker(&m_saveContactMutex);
  
  if (!contact) {
    *error = QContactManager::BadArgumentError;
    return false;
  }

  bool ok = false;
  
  OssoABookContact *aContact = NULL;
  const char *uid;
  EBook *book;
  {
    OssoABookRoster* roster = reinterpret_cast<OssoABookRoster*>(m_abookAgregator);
    book = osso_abook_roster_get_book(roster);
  }
  
  // Conver QContact to AContact
  aContact = convert(contact);
  if (!aContact){
    *error = QContactManager::UnspecifiedError;
    return false;
  }  

  // ASync => Sync
  QEventLoop loop;
  connect(this, SIGNAL(savingJobDone()), &loop, SLOT(quit()));

  // Prepare shared data
  svSharedData sd;
  sd.that = this;
  sd.result = &ok;
  
  // Add/Commit the contact
  uid = CONST_CHAR(e_contact_get_const(E_CONTACT (aContact), E_CONTACT_UID)); 
  if (uid) {
    osso_abook_contact_async_commit(aContact, book, commitContactCB, &sd);
  } else {
    osso_abook_contact_async_add(aContact, book, addContactCB, &sd);
  }
  
  loop.exec(QEventLoop::AllEvents|QEventLoop::WaitForMoreEvents);
  
  return ok;
}

QContactLocalId QContactABook::selfContactId(QContactManager::Error* errors) const
{
  QContactLocalId id;
  EContact *self = E_CONTACT(osso_abook_self_contact_get_default());
  if (self) {
    *errors = QContactManager::NoError;
    const char* data = CONST_CHAR(e_contact_get_const(self, E_CONTACT_UID));
    const QByteArray eContactUID(data);
    QContactLocalId localId = m_localIds[eContactUID];
    if (localId)
      id = localId;
    else {
      m_localIds << eContactUID; //FIXME MemLeak
      id = m_localIds[eContactUID];
      QCM5_DEBUG << "eContactID " << eContactUID << "has been stored in m_localIDs with key" << id;
    }
  } else {
    qWarning() << "Cannot find self contact";
    *errors = QContactManager::DoesNotExistError;
    id = 0;
  }
  g_object_unref(self);
  return id;
}

bool QContactABook::contactActionsMatch(OssoABookContact *contact, QList<QContactActionDescriptor> descriptors) const
{
  OssoABookCapsFlags capsFlags = osso_abook_caps_get_capabilities(OSSO_ABOOK_CAPS(contact));

  if(capsFlags & OSSO_ABOOK_CAPS_NONE)
    return false;

  /* ActionNames could be incorrect */
  OssoABookCapsFlags actionFlags = OSSO_ABOOK_CAPS_NONE;
  for(int i = 0; i < descriptors.size(); i++){
    QString actionName = descriptors.at(i).actionName();
    QCM5_DEBUG << actionName;
    if(!actionName.compare("Phone"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_PHONE);
    else if(!actionName.compare("Voice"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_VOICE);
    else if(!actionName.compare("SendEmail"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_EMAIL);
    else if(!actionName.compare("Chat"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_CHAT);
    else if(!actionName.compare("ChatAdditional"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_CHAT_ADDITIONAL);
    else if(!actionName.compare("VoiceAdditional"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_VOICE_ADDITIONAL);
    else if(!actionName.compare("Video"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_VIDEO);
    else if(!actionName.compare("Addressbook"))
      actionFlags = (OssoABookCapsFlags)(actionFlags | OSSO_ABOOK_CAPS_ADDRESSBOOK);
  }
  return ((actionFlags & capsFlags) == actionFlags);
}

EBookQuery* QContactABook::convert(const QContactFilter& filter) const
{
  EBookQuery* query = NULL;
  
  switch(filter.type()){
    case QContactFilter::DefaultFilter:
    {
      QCM5_DEBUG << "QContactFilter::DefaultFilter";
      query = e_book_query_any_field_contains(""); //Match all contacts
    } break;
    case QContactFilter::LocalIdFilter:
    {
      QCM5_DEBUG << "LocalIdFilter";
      const QContactLocalIdFilter f(filter);
      QList<QContactLocalId> ids = f.ids();
      if (ids.isEmpty())
        return NULL;
      
      query= NULL;
      foreach(const QContactLocalId id, ids){
        EBookQuery* q = NULL;
        
        // Looking for the eContact local id inside the localId hash
        const char* eContactId = m_localIds[id];
        if (!eContactId[0])
          return NULL;
        
        q = e_book_query_field_test(E_CONTACT_UID, E_BOOK_QUERY_IS, eContactId);
        if (!q)
          continue;
        if (query)
          query = e_book_query_orv(query, q, NULL);
        else
          query = q;
      }
    } break;
    case QContactFilter::ContactDetailFilter:
    {
      QCM5_DEBUG << "ContactDetailFilter";
      const QContactDetailFilter f(filter);
      QString queryStr;
      if (!f.value().isValid())
        return NULL;
      switch (f.matchFlags()){
        case QContactFilter::MatchContains: queryStr = "contains"; break;
        case QContactFilter::MatchFixedString:
        case QContactFilter::MatchCaseSensitive:
        case QContactFilter::MatchExactly: queryStr = "is"; break;
        case QContactFilter::MatchStartsWith: queryStr = "beginswith"; break;
        case QContactFilter::MatchEndsWith: queryStr = "endswith"; break;
        default:
          queryStr = "contains";
          qWarning() << "Match flag not supported";
      }
      static QHash<QString,QString> hash;
      if (hash.isEmpty()){
        hash[QContactAddress::DefinitionName] = "address";
        hash[QContactBirthday::DefinitionName] = "birth-date";
        hash[QContactDisplayLabel::DefinitionName] = "full-name"; //hack
        hash[QContactEmailAddress::DefinitionName] = "email";
        hash[QContactName::DefinitionName] = "full-name";
        hash[QContactNickname::DefinitionName] = "nickname";
        hash[QContactNote::DefinitionName] = "note";
        hash[QContactOrganization::DefinitionName] = "title";
        hash[QContactPhoneNumber::DefinitionName] = "phone";
        hash[QContactUrl::DefinitionName] = "homepage-url";
      }
  
      QString eDetail = hash[f.detailDefinitionName()];
      if (eDetail.isEmpty()){
        qWarning() << "Unable to found an ebook detail for " << f.detailDefinitionName();
        return NULL;
      }
      queryStr = queryStr + " \"" + eDetail + "\" \"" + f.value().toString() + "\"";
      query = e_book_query_from_string(qPrintable(queryStr));
    } break;
    case QContactFilter::ActionFilter:
      QCM5_DEBUG << "ActionFilter"; //eQuery doesn't support ActionFilter
      break;
    case QContactFilter::IntersectionFilter:
    {
      QCM5_DEBUG << "IntersectionFilter";
      const QContactIntersectionFilter f(filter);
      const QList<QContactFilter>  fs= f.filters();
      QContactFilter i;
      foreach(i, fs){
        EBookQuery* q = convert(i);
        if (!q){
          qWarning() << "Query is null";
          continue;
        }
        if (query)
          query = e_book_query_andv(query, q, NULL);
        else
          query = q;
      } 
    } break;
    case QContactFilter::UnionFilter:
    {
      QCM5_DEBUG << "UnionFilter";
      const QContactUnionFilter f(filter);
      const QList<QContactFilter>  fs= f.filters();
      QContactFilter i;
      foreach(i, fs){
        EBookQuery* q = convert(i);
        if (!q){
          qWarning() << "Query is null";
          continue;
        }
        if (query)
          query = e_book_query_orv(query, q, NULL);
        else
          query = q;
      }
    } break;
    case QContactFilter::InvalidFilter:
    {
      QCM5_DEBUG << "InvalidFilter";
      query = e_book_query_from_string("(is \"id\" \"-1\")");
    } break;
    default:
      QCM5_DEBUG << "Filter not supported";
  }
 
  //Debugging
  const char *queryString = e_book_query_to_string(query);
  QCM5_DEBUG << "QUERY" << queryString;
  FREE(queryString);
  
  return query;
} 

QContact* QContactABook::convert(EContact *eContact) const
{
  QContact *contact = new QContact();
  QList<QContactDetail*> detailList;

  /* Id */
  contact->setId(getContactId(eContact));

  /* Address */
  QList<QContactAddress*> addressList = getAddressDetail(eContact);
  QContactAddress* address;
  foreach(address, addressList)
    detailList << address;

  /* Avatar */
  detailList << getAvatarDetail(eContact); // XXX TODO: FIXME
  detailList << getThumbnailDetail(eContact);

  /* BirthDay */
  detailList << getBirthdayDetail(eContact);

  /* Email */
  QList<QContactEmailAddress*> emailList = getEmailDetail(eContact);
  QContactEmailAddress* email;
  foreach(email, emailList)
    detailList << email;

  /* Gender */
  detailList << getGenderDetail(eContact);

  /* Global UID*/
  detailList << getGuidDetail(eContact);

  /* Name & NickName*/
  detailList << getNameDetail(eContact);
  detailList << getNicknameDetail(eContact);

  /* Note */
  detailList << getNoteDetail(eContact);

  /* Online Account */
  QList<QContactOnlineAccount*> onlineAccountList = getOnlineAccountDetail(eContact);
  QContactOnlineAccount* onlineAccount;
  foreach(onlineAccount, onlineAccountList)
    detailList << onlineAccount;

  /* Organization */
  detailList << getOrganizationDetail(eContact);

  /* Phone*/
  QList<QContactPhoneNumber*> phoneNumberList = getPhoneDetail(eContact);
  QContactPhoneNumber* phoneNumber;
  foreach(phoneNumber, phoneNumberList)
    detailList << phoneNumber;

  /* TimeStamp */
  detailList << getTimestampDetail(eContact);

  /* Url */
  detailList << getUrlDetail(eContact);

  bool ok;
  QContactDetail* detail;
 
  foreach(detail, detailList){
    if (detail->isEmpty()){
      delete detail;
      continue;
    }

    ok = contact->saveDetail(detail);
    if (!ok){
      qWarning() << "Detail can't be saved into QContact";
      delete detail;
      continue;
    }
    delete detail;
  }

  return contact;
}

bool QContactABook::setDetailValues(const QVariantMap& data, QContactDetail* detail) const
{
  QMapIterator<QString, QVariant> i(data);
  QVariant value;
  while (i.hasNext()) {
     i.next();
     value = i.value();
     
     if (value.isNull())
       continue;
     
     if (value.canConvert<QString>() && value.toString().isEmpty())
       continue;
     
     QCM5_DEBUG << "Set" << i.key() << i.value();
     detail->setValue(i.key(), i.value());

  }
  
  if (detail->isEmpty())
    return false;
  return true;
}

OssoABookContact* QContactABook::getAContact(const QContactLocalId& contactId) const
{
  OssoABookContact* rtn = NULL;

  QCM5_DEBUG << "Getting aContact with id " << m_localIds[contactId] << "local contactId is" << contactId;

  if(QString(m_localIds[contactId]).compare("osso-abook-self") == 0) {
    rtn = A_CONTACT(osso_abook_self_contact_get_default());
  } else {
    EBookQuery* query;
    GList* contacts;

    query = e_book_query_field_test(E_CONTACT_UID, E_BOOK_QUERY_IS, m_localIds[contactId]);
    contacts = osso_abook_aggregator_find_contacts(m_abookAgregator, query);
    e_book_query_unref(query);

    if (g_list_length(contacts) == 1) {
      rtn = A_CONTACT(contacts->data);
    } else {
      qWarning("List is empty or several contacts have the same UID or contactId belongs to a roster contact.");
    }
    if (contacts)
      g_list_free(contacts);
  }

  return rtn;
}

QContactId QContactABook::getContactId(EContact *eContact) const
{
  QContactId rtn;
  /* Set LocalId */
  {
    const char* data = CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_UID));
    const QByteArray eContactUID(data);
    QContactLocalId localId = m_localIds[eContactUID];
    if (!localId)
      qWarning("Unable to get valid localId for the specified eContaact UID");
    rtn.setLocalId(localId);
  }
  return rtn;
}

QList<QContactAddress*> QContactABook::getAddressDetail(EContact *eContact) const
{
  QList<QContactAddress*> rtnList;

  //Ordered list of Fields
  QStringList addressFields;
  addressFields << QContactAddress::FieldPostOfficeBox
                << "Estension" //FIXME I'm not sure we have to use a new field 
                << QContactAddress::FieldStreet
                << QContactAddress::FieldLocality
                << QContactAddress::FieldRegion 
                << QContactAddress::FieldPostcode 
                << QContactAddress::FieldCountry;
  
  GList* attrList = osso_abook_contact_get_attributes(eContact, EVC_ADR);
  
  for (GList *node = g_list_last(attrList); node != NULL; node = g_list_previous(node)) {
    QContactAddress *address = new QContactAddress;
    QVariantMap map;
 
    EVCardAttribute *attr = static_cast<EVCardAttribute*>(node->data);
    
    
    // Set Address Context using attribute parameter value
    EVCardAttributeParam *param = NULL;
    GList* p = e_vcard_attribute_get_params(attr);
    
    if (p)
      param = static_cast<EVCardAttributeParam*>(p->data);
    
    if (param){
      GList *v = e_vcard_attribute_param_get_values(param);
      QString context = CONST_CHAR(v->data);
      if (context == "HOME")
        address->setContexts(QContactDetail::ContextHome);
      else if (context == "WORK")
        address->setContexts(QContactDetail::ContextWork);
    }
    
    // Set Address Values
    GList *v = NULL;
    v =e_vcard_attribute_get_values(attr);
    if (!v)
      qFatal("ADR attribute data is corrupted"); 
    if (g_list_length(v) != 7){
      g_list_free(v);
      qCritical() << "ADR attribute data is corrupted"; 
    }
    int i = 0;
    while (v){
      map[addressFields[i]] = QString::fromLatin1(CONST_CHAR(v->data));
      i++;
      v = v->next;
    }
    g_list_free(v);
    map[QContactDetail::FieldDetailUri] = QString::number(g_list_position(attrList, node));
    setDetailValues(map, address);
    
    rtnList << address;
  }
  
  g_list_free(attrList);
  
  return rtnList;
}

QContactName* QContactABook::getNameDetail(EContact *eContact) const
{
  QContactName* rtn = new QContactName;
  QVariantMap map;

  //Try to get the structure (looks that this is not supported in Maemo5)
  EContactName* eContactName = static_cast<EContactName*> (e_contact_get(eContact, E_CONTACT_NAME));
  if (eContactName){
    map[QContactName::FieldCustomLabel] = eContactName->additional;
    map[QContactName::FieldFirstName] = eContactName->given;
    map[QContactName::FieldLastName] = eContactName->family;
    //map[QContactName::FieldMiddleName] = eContactName->
    map[QContactName::FieldPrefix] = eContactName->prefixes;
    map[QContactName::FieldSuffix] = eContactName->suffixes;
    e_contact_name_free (eContactName);
  } else {
    //Looks that Maemo use just these two fields
    map[QContactName::FieldFirstName] = CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_GIVEN_NAME));
    map[QContactName::FieldLastName] = CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_FAMILY_NAME));
  }
  setDetailValues(map, rtn);
  return rtn;
}

QContactNickname* QContactABook::getNicknameDetail(EContact *eContact) const
{
  QContactNickname* rtn = new QContactNickname;
  QVariantMap map;
  map[QContactNickname::FieldNickname] = CONST_CHAR (e_contact_get_const(eContact, E_CONTACT_NICKNAME));
  setDetailValues(map, rtn);
  return rtn;
}

QList<QContactEmailAddress*> QContactABook::getEmailDetail(EContact *eContact) const
{
  QList<QContactEmailAddress*> rtnList;
  
  GList* attrList = osso_abook_contact_get_attributes(eContact, EVC_EMAIL); //FIXME MemLeak
  
  for (GList *node = g_list_last(attrList); node != NULL; node = g_list_previous(node)) {
    QContactEmailAddress *email = new QContactEmailAddress;
    QVariantMap map;
  
    EVCardAttribute *attr = static_cast<EVCardAttribute*>(node->data);

    // Set Address Context using attribute parameter value
    EVCardAttributeParam *param = NULL;
    GList* p = e_vcard_attribute_get_params(attr);
    
    if (p)
      param = static_cast<EVCardAttributeParam*>(p->data);
    
    if (param){
      GList *v = e_vcard_attribute_param_get_values(param);
      QString context = CONST_CHAR(v->data);
      if (context == "HOME")
        email->setContexts(QContactDetail::ContextHome);
      else if (context == "WORK")
        email->setContexts(QContactDetail::ContextWork);
    }
    
    // Set Address Values
    GList *v = e_vcard_attribute_get_values(attr);
    int i = 0;
    while (v){
      map[QContactEmailAddress::FieldEmailAddress] = QString::fromLatin1(CONST_CHAR(v->data));
      i++;
      v = v->next;
    }
    g_list_free(v);
    
    map[QContactDetail::FieldDetailUri] = QString::number(g_list_position(attrList, node));
    setDetailValues(map, email);
    rtnList << email;
  }
  g_list_free(attrList);
  
  return rtnList;
}

QContactAvatar* QContactABook::getAvatarDetail(EContact *eContact) const
{  
// XXX TODO: FIXME
//  QContactAvatar* rtn = new QContactAvatar;
//  QVariantMap map;
//
//  OssoABookContact *aContact = A_CONTACT(eContact);
//  if (!aContact)
//    return rtn;
//
//  //GdkPixbuf* pixbuf = osso_abook_contact_get_avatar_pixbuf(aContact, NULL, NULL);
//  GdkPixbuf* pixbuf = osso_abook_avatar_get_image_rounded(OSSO_ABOOK_AVATAR(aContact), 64, 64, true, 4, NULL);
//  if (!GDK_IS_PIXBUF(pixbuf)){
//    FREE(pixbuf);
//    return rtn;
//  }
//
//  const uchar* bdata = (const uchar*)gdk_pixbuf_get_pixels(pixbuf);
//  QSize bsize(gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf));
//
//  //Convert GdkPixbuf to QPixmap
//  QImage converted(bdata, bsize.width(), bsize.height(), QImage::Format_ARGB32_Premultiplied);
//  map[QContactAvatar::FieldPixmap] = QPixmap::fromImage(converted);
//  g_object_unref(pixbuf);
//  setDetailValues(map, rtn);
//
//  return rtn;

    QContactAvatar* empty = new QContactAvatar;
    return empty;
}

QContactBirthday* QContactABook::getBirthdayDetail(EContact *eContact) const
{
  QContactBirthday* rtn = new QContactBirthday;
  QVariantMap map;
  EContactDate *date =static_cast<EContactDate*>(e_contact_get(eContact, E_CONTACT_BIRTH_DATE));
  if (!date)
    return rtn;
  QDate birthday(date->year, date->month, date->day);
  e_contact_date_free(date);
  map[QContactBirthday::FieldBirthday] = birthday;
  setDetailValues(map, rtn);
  return rtn;
}

QContactGender* QContactABook::getGenderDetail(EContact *eContact) const
{
  QContactGender* rtn = new QContactGender;
  QVariantMap map;
  const char* g = CONST_CHAR(osso_abook_contact_get_value(eContact, "X-GENDER"));
  QString gender = QString::fromLatin1(g);
  if (gender == "male")
    gender = "Male";
  else if (gender == "female")
    gender = "Female";
  else if (gender == "unspecified")
    gender = "Unspecified";
  
  map[QContactGender::FieldGender] = gender;
  FREE(g);
  setDetailValues(map, rtn);
  return rtn;
}  

//NOTE Using UID as GUID
QContactGuid* QContactABook::getGuidDetail(EContact *eContact) const
{
  QContactGuid* rtn = new QContactGuid;
  QVariantMap map;
  const char* uid = CONST_CHAR(e_contact_get(eContact, E_CONTACT_UID));
  map[QContactGuid::FieldGuid] = uid;
  FREE(uid);
  setDetailValues(map, rtn);
  return rtn;
}

QContactNote* QContactABook::getNoteDetail(EContact *eContact) const
{
  QContactNote* rtn = new QContactNote;
  QVariantMap map;
  const char* note = CONST_CHAR(e_contact_get(eContact, E_CONTACT_NOTE));
  map[QContactNote::FieldNote] = QString::fromLatin1(note);
  FREE(note);
  setDetailValues(map, rtn);
  return rtn;
}

static const QStringList vcardsManagedByTelepathy(){
  QStringList rtn;
  OssoABookAccountManager* accountMgr = osso_abook_account_manager_get_default();
  const GList *vcardFields = osso_abook_account_manager_get_primary_vcard_fields(accountMgr);
  while (vcardFields){
    QString field = (const char*)vcardFields->data;
    if (!rtn.contains(field))
      rtn << field;
    vcardFields = vcardFields->next;
  }
  
  return rtn;
}

QList<QContactOnlineAccount*> QContactABook::getOnlineAccountDetail(EContact *eContact) const
{
  QList<QContactOnlineAccount*> rtnList;
  
  QStringList evcardToSkip = vcardsManagedByTelepathy();
  
  // Gets info of online accounts from roster contacts associated to the master one  
  if (!osso_abook_contact_is_roster_contact (A_CONTACT(eContact))) {
    QContactOnlineAccount* rtn = new QContactOnlineAccount;
    
    GList *contacts = osso_abook_contact_get_roster_contacts(A_CONTACT(eContact));
    GList *node;
    for (node = contacts; node != NULL; node = g_list_next(node)){
      OssoABookContact *rosterContact = A_CONTACT(node->data);
     
      McProfile* id = osso_abook_contact_get_profile(rosterContact);
      McAccount* account = osso_abook_contact_get_account(rosterContact);
      
      // Avoid to look for Roster contacts into the VCard
      QString accountVCard = QString::fromLatin1(mc_profile_get_vcard_field(id));
      evcardToSkip.removeOne(accountVCard);
      
      // Presence
      OssoABookPresence *presence = OSSO_ABOOK_PRESENCE (rosterContact);
      TpConnectionPresenceType presenceType = osso_abook_presence_get_presence_type (presence);
      QString presenceTypeString;
      QContactPresence presenceTypeEnum;
      switch (presenceType) {
        case TP_CONNECTION_PRESENCE_TYPE_UNSET: presenceTypeString = "Unset"; presenceTypeEnum = QContactPresence::PresenceUnknown; break;
        case TP_CONNECTION_PRESENCE_TYPE_OFFLINE: presenceTypeString = "Offline"; presenceTypeEnum = QContactPresence::PresenceOffline; break;
        case TP_CONNECTION_PRESENCE_TYPE_AVAILABLE: presenceTypeString = "Available"; presenceTypeEnum = QContactPresence::PresenceAvailable; break;
        case TP_CONNECTION_PRESENCE_TYPE_AWAY: presenceTypeString = "Away"; presenceTypeEnum = QContactPresence::PresenceAway; break;
        case TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY: presenceTypeString = "Extended Away"; presenceTypeEnum = QContactPresence::PresenceExtendedAway; break;
        case TP_CONNECTION_PRESENCE_TYPE_HIDDEN: presenceTypeString = "Hidden"; presenceTypeEnum = QContactPresence::PresenceHidden; break;
        case TP_CONNECTION_PRESENCE_TYPE_BUSY: presenceTypeString = "Busy"; presenceTypeEnum = QContactPresence::PresenceBusy; break;
        case TP_CONNECTION_PRESENCE_TYPE_UNKNOWN: presenceTypeString = "Unknown"; presenceTypeEnum = QContactPresence::PresenceUnknown; break;
        case TP_CONNECTION_PRESENCE_TYPE_ERROR: presenceTypeString = "Error"; presenceTypeEnum = QContactPresence::PresenceUnknown; break;
        default:
          qCritical() << "Presence type is not vaild" << presenceType;
      }
      
      QVariantMap map;
      map[QContactOnlineAccount::FieldServiceProvider] = mc_profile_get_unique_name(id);
      map[QContactOnlineAccount::FieldDetailUri] = mc_profile_get_unique_name(id); // use this as detail URI so we can link to presence.
      map["AccountPath"] = account->name; //MCAccount name: variable part of the D-Bus object path.
      
      setDetailValues(map, rtn);
    }
    rtnList << rtn;
    g_list_free (contacts);
  }
  
  /* Users can add Online account details manually. Eg: IRC username.
   * evcardToSkip stringlist contains evCard attributes that have been already processed.
   */
  GList *attributeList = e_vcard_get_attributes((EVCard*)eContact);
  GList *node;

  if (attributeList) {
    for (node = attributeList; node != NULL; node = g_list_next (node)) {
      EVCardAttribute* attr = (EVCardAttribute*)node->data;
      if (!attr)
        continue;
      QString attributeName = QString::fromLatin1(e_vcard_attribute_get_name(attr));
      
      // Skip attributes processed scanning roster contacts.
      if (!evcardToSkip.contains(attributeName))
        continue;
      
      GList *params = e_vcard_attribute_get_params(attr);
      GList *nodeP;
      QString type;
      // If the parameter list lenght is 1, X-OSSO-VALID is not specified
      bool ossoValidIsOk = (g_list_length(params) == 1) ? true : false;

      for (nodeP = params; nodeP != NULL; nodeP = g_list_next (nodeP)) {
        EVCardAttributeParam* p = (EVCardAttributeParam*) nodeP->data;
        QString paramName = QString::fromLatin1(e_vcard_attribute_param_get_name(p));
        bool attrIsType = false;
        bool attrIsOssoValid = false;
        
        //If type is empty check if the attribute is "TYPE"
        if (type.isEmpty())
          attrIsType = paramName.contains(EVC_TYPE);
        
        if(!ossoValidIsOk)
          attrIsOssoValid = paramName.contains("X-OSSO-VALID");
        
        if (!attrIsType && !attrIsOssoValid) {
          qWarning () << "Skipping attribute parameter checking for" << paramName;
          continue;
        }
        
        GList *values = e_vcard_attribute_param_get_values(p);
        GList *node;
        for (node = values; node != NULL; node = g_list_next (node)) {
          QString attributeParameterValue = QString::fromLatin1(CONST_CHAR(node->data));
          if (attrIsOssoValid) {
            ossoValidIsOk = (attributeParameterValue == "yes")? true : false;
            if (!ossoValidIsOk) {
              qWarning() << "X-OSSO-VALID is false.";
              break;
            }
          } else if (type.isEmpty()) {
            type = attributeParameterValue;
            if (type.isEmpty())
              qCritical() << "TYPE is empty"; 
          }
        }
        
        if (ossoValidIsOk && !type.isEmpty()) {
          QContactOnlineAccount* rtn = new QContactOnlineAccount;
          QVariantMap map;
          map[QContactOnlineAccount::FieldServiceProvider] = type;
          setDetailValues(map, rtn);
          rtnList << rtn;
        }
      }
    }
  }

  return rtnList;
}

QContactOrganization* QContactABook::getOrganizationDetail(EContact *eContact) const
{
  QContactOrganization* rtn = new QContactOrganization;
  QVariantMap map;
  const char* title = CONST_CHAR(e_contact_get(eContact, E_CONTACT_TITLE));
  map[QContactOrganization::FieldTitle] = title;
  FREE(title);
  setDetailValues(map, rtn);
  return rtn;
}

QList<QContactPhoneNumber*> QContactABook::getPhoneDetail(EContact *eContact) const
{
  QList<QContactPhoneNumber*> rtnList;
  
  GList *l = osso_abook_contact_get_attributes(eContact, EVC_TEL);
  
  for (GList *node = g_list_last(l); node != NULL; node = g_list_previous(node)) {
    QContactPhoneNumber* phoneNumber = new QContactPhoneNumber;
    QVariantMap map;
    
    EVCardAttribute *attr = static_cast<EVCardAttribute*>(node->data);
    GList* p = e_vcard_attribute_get_param(attr, EVC_TYPE);
    
    //Set Contexts and SubTypes
    while (p) {
      QString value = QString::fromLatin1(CONST_CHAR(p->data));
      
      if (value == "HOME")
        phoneNumber->setContexts(QContactDetail::ContextHome);
      else if (value == "WORK")
        phoneNumber->setContexts(QContactDetail::ContextWork);
      else
      if (value == "CELL")
        phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeMobile);
      else if (value == "VOICE")
        phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeVoice);
      
      p = p->next;
    }
    g_list_free(p);
    
    //Set Phone Number
    GList* phoneNumbers = e_vcard_attribute_get_values(attr);
    const char* normalized = e_normalize_phone_number(CONST_CHAR(phoneNumbers->data)); //FIXME Valgrind complains about this
    QString phoneNumberStr(normalized);
    FREE(normalized);
    map[QContactPhoneNumber::FieldNumber] = phoneNumberStr;
    map[QContactDetail::FieldDetailUri] = QString::number(g_list_position(l, node));
    setDetailValues(map, phoneNumber);
    
    rtnList << phoneNumber;
  }
  g_list_free(l);
  
  return rtnList;
}


QList<QContactPresence*> QContactABook::getPresenceDetail(EContact *eContact) const
{
  QList<QContactPresence*> rtnList;

  QStringList evcardToSkip = vcardsManagedByTelepathy();

  // Gets info of online accounts from roster contacts associated to the master one
  if (!osso_abook_contact_is_roster_contact (A_CONTACT(eContact))) {
    QContactPresence* rtn = new QContactPresence;

    GList *contacts = osso_abook_contact_get_roster_contacts(A_CONTACT(eContact));
    GList *node;
    for (node = contacts; node != NULL; node = g_list_next(node)){
      OssoABookContact *rosterContact = A_CONTACT(node->data);

      McProfile* id = osso_abook_contact_get_profile(rosterContact);
      McAccount* account = osso_abook_contact_get_account(rosterContact);

      // Avoid to look for Roster contacts into the VCard
      QString accountVCard = QString::fromLatin1(mc_profile_get_vcard_field(id));
      evcardToSkip.removeOne(accountVCard);

      // Presence
      OssoABookPresence *presence = OSSO_ABOOK_PRESENCE (rosterContact);
      TpConnectionPresenceType presenceType = osso_abook_presence_get_presence_type (presence);
      QString presenceTypeString;
      QContactPresence presenceTypeEnum;
      switch (presenceType) {
        case TP_CONNECTION_PRESENCE_TYPE_UNSET: presenceTypeString = "Unset"; presenceTypeEnum = QContactPresence::PresenceUnknown; break;
        case TP_CONNECTION_PRESENCE_TYPE_OFFLINE: presenceTypeString = "Offline"; presenceTypeEnum = QContactPresence::PresenceOffline; break;
        case TP_CONNECTION_PRESENCE_TYPE_AVAILABLE: presenceTypeString = "Available"; presenceTypeEnum = QContactPresence::PresenceAvailable; break;
        case TP_CONNECTION_PRESENCE_TYPE_AWAY: presenceTypeString = "Away"; presenceTypeEnum = QContactPresence::PresenceAway; break;
        case TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY: presenceTypeString = "Extended Away"; presenceTypeEnum = QContactPresence::PresenceExtendedAway; break;
        case TP_CONNECTION_PRESENCE_TYPE_HIDDEN: presenceTypeString = "Hidden"; presenceTypeEnum = QContactPresence::PresenceHidden; break;
        case TP_CONNECTION_PRESENCE_TYPE_BUSY: presenceTypeString = "Busy"; presenceTypeEnum = QContactPresence::PresenceBusy; break;
        case TP_CONNECTION_PRESENCE_TYPE_UNKNOWN: presenceTypeString = "Unknown"; presenceTypeEnum = QContactPresence::PresenceUnknown; break;
        case TP_CONNECTION_PRESENCE_TYPE_ERROR: presenceTypeString = "Error"; presenceTypeEnum = QContactPresence::PresenceUnknown; break;
        default:
          qCritical() << "Presence type is not valid" << presenceType;
      }

      QVariantMap map; // XXX FIXME
      map[QContactPresence::FieldNickname] = osso_abook_contact_get_display_name(rosterContact);
      map[QContactPresence::FieldPresenceState] = presenceTypeEnum;
      map[QContactPresence::FieldPresenceStateText] = QString::fromLatin1(osso_abook_presence_get_presence_status_message(presence));
      map[QContactPresence::FieldLinkedDetailUris] = mc_profile_get_unique_name(id); //use the unique name as a detail uri of the online account.
      map["AccountPath"] = account->name; //MCAccount name: variable part of the D-Bus object path.

      setDetailValues(map, rtn);
    }
    rtnList << rtn;
    g_list_free (contacts);
  }

  /* Users can add Online account details manually. Eg: IRC username.
   * evcardToSkip stringlist contains evCard attributes that have been already processed.
   */
  GList *attributeList = e_vcard_get_attributes((EVCard*)eContact);
  GList *node;

  if (attributeList) {
    for (node = attributeList; node != NULL; node = g_list_next (node)) {
      EVCardAttribute* attr = (EVCardAttribute*)node->data;
      if (!attr)
        continue;
      QString attributeName = QString::fromLatin1(e_vcard_attribute_get_name(attr));

      // Skip attributes processed scanning roster contacts.
      if (!evcardToSkip.contains(attributeName))
        continue;

      GList *params = e_vcard_attribute_get_params(attr);
      GList *nodeP;
      QString type;
      // If the parameter list lenght is 1, X-OSSO-VALID is not specified
      bool ossoValidIsOk = (g_list_length(params) == 1) ? true : false;

      for (nodeP = params; nodeP != NULL; nodeP = g_list_next (nodeP)) {
        EVCardAttributeParam* p = (EVCardAttributeParam*) nodeP->data;
        QString paramName = QString::fromLatin1(e_vcard_attribute_param_get_name(p));
        bool attrIsType = false;
        bool attrIsOssoValid = false;

        //If type is empty check if the attribute is "TYPE"
        if (type.isEmpty())
          attrIsType = paramName.contains(EVC_TYPE);

        if(!ossoValidIsOk)
          attrIsOssoValid = paramName.contains("X-OSSO-VALID");

        if (!attrIsType && !attrIsOssoValid) {
          qWarning () << "Skipping attribute parameter checking for" << paramName;
          continue;
        }

        GList *values = e_vcard_attribute_param_get_values(p);
        GList *node;
        for (node = values; node != NULL; node = g_list_next (node)) {
          QString attributeParameterValue = QString::fromLatin1(CONST_CHAR(node->data));
          if (attrIsOssoValid) {
            ossoValidIsOk = (attributeParameterValue == "yes")? true : false;
            if (!ossoValidIsOk) {
              qWarning() << "X-OSSO-VALID is false.";
              break;
            }
          } else if (type.isEmpty()) {
            type = attributeParameterValue;
            if (type.isEmpty())
              qCritical() << "TYPE is empty";
          }
        }

        if (ossoValidIsOk && !type.isEmpty()) {
          QContactPresence* rtn = new QContactOnlineAccount;
          QVariantMap map;
          map[QContactPresence::FieldNickname] = QString::fromLatin1(e_vcard_attribute_get_value(attr));
          map[QContactPresence::FieldLinkedDetailUris] = type; // XXX FIXME
          setDetailValues(map, rtn);
          rtnList << rtn;
        }
      }
    }
  }

  return rtnList;
}

QContactTimestamp* QContactABook::getTimestampDetail(EContact *eContact) const
{
   QContactTimestamp* rtn = new QContactTimestamp;
   QVariantMap map;
   const char* rev = CONST_CHAR(e_contact_get(eContact, E_CONTACT_REV));
   map[QContactTimestamp::FieldModificationTimestamp] = QDateTime::fromString(rev, Qt::ISODate);
   FREE(rev);
   setDetailValues(map, rtn);
   return rtn;
}

QContactThumbnail* QContactABook::getThumbnailDetail(EContact *eContact) const
{
  QContactThumbnail* rtn = new QContactThumbnail;
  QVariantMap map;

  OssoABookContact *aContact = A_CONTACT(eContact);
  if (!aContact)
    return rtn;

  //GdkPixbuf* pixbuf = osso_abook_contact_get_avatar_pixbuf(aContact, NULL, NULL);
  GdkPixbuf* pixbuf = osso_abook_avatar_get_image_rounded(OSSO_ABOOK_AVATAR(aContact), 64, 64, true, 4, NULL);
  if (!GDK_IS_PIXBUF(pixbuf)){
    FREE(pixbuf);
    return rtn;
  }

  const uchar* bdata = (const uchar*)gdk_pixbuf_get_pixels(pixbuf);
  QSize bsize(gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf));

  //Convert GdkPixbuf to QPixmap
  QImage converted(bdata, bsize.width(), bsize.height(), QImage::Format_ARGB32_Premultiplied);
  map[QContactThumbnail::FieldThumbnail] = converted;
  g_object_unref(pixbuf);
  setDetailValues(map, rtn);

  return rtn;
}

QContactUrl* QContactABook::getUrlDetail(EContact *eContact) const
{
   QContactUrl* rtn = new QContactUrl;
   QVariantMap map;
   const char* url = CONST_CHAR(e_contact_get(eContact, E_CONTACT_HOMEPAGE_URL));
   map[QContactUrl::FieldUrl] = url;
   FREE(url);
   setDetailValues(map, rtn);
   return rtn;
}

static void addAttributeToAContact(const OssoABookContact* contact,
                                   const QString& attrName, const QStringList& attrValues,
                                   const QString& paramName = QString(), const QStringList& paramValues = QStringList(),
                                   bool overwrite = true,
                                   const int index = 0)
{
  if (!contact)
    return;
  
  EVCard *vcard = E_VCARD (contact);
  EVCardAttribute *attr = NULL;
  EVCardAttributeParam* param = NULL;
  
  QCM5_DEBUG << "Adding attribute" << attrName << "AttrValues:" << attrValues
             << "ParamName:" << paramName << "ParamValues:" << paramValues
             << "overwrite" << overwrite << "Index" << index;
  
  if (overwrite)
  {
    GList *attributeList = osso_abook_contact_get_attributes(E_CONTACT(contact), qPrintable(attrName));
    
    for (GList *node = g_list_last(attributeList); node != NULL; node = g_list_previous(node)) {
      EVCardAttribute* eAttr = (EVCardAttribute*)node->data;
      int pos =  g_list_position(attributeList, node);
      
      if (index > pos){
        qWarning() << "Attribute doesn't found at position" << index;
        return;
      }
      
      if (index != pos)
        continue;
      
      // Select the current EVCard Attribute if it contains the same parameters of
      // attribute we want to modify/add
      int matchedParams = 0;
      GList* p = NULL;
      p = e_vcard_attribute_get_param(eAttr, qPrintable(paramName));

      while (p){
          foreach(QString paramV, paramValues){
          QString value = CONST_CHAR(p->data);
          if (paramV == value)
            ++matchedParams;
        }
        p = p->next;
      }
      g_list_free(p);

      if (matchedParams == paramValues.count()) {
        attr = eAttr;
        break;
      }    
    }
  }
  
  // Check if attrValues contains something
  bool noValues = true;
  foreach(QString s, attrValues){
    if (!s.isEmpty()){
      noValues = false;
      break;
    }
  }
  
  if (attr) {
    if (noValues){
      e_vcard_remove_attribute(vcard, attr);
      return;
    } else {
      e_vcard_attribute_remove_values(attr);
    }
  } else {
    if (noValues)
      return;
    
    // Create Attribute with right parameters
    attr = e_vcard_attribute_new(NULL, qPrintable(attrName));
    if (!paramName.isEmpty()){
      param = e_vcard_attribute_param_new(qPrintable(paramName));
      
      foreach(QString paramV, paramValues)
        e_vcard_attribute_param_add_value(param, qPrintable(paramV));

      e_vcard_attribute_add_param(attr, param);
    }
    // Save the attribute to the VCard
    e_vcard_add_attribute(vcard, attr);
  }
  
  // Add values to the attribute
  foreach(QString attrV, attrValues) {
    e_vcard_attribute_add_value(attr, qPrintable(attrV));
  }
  
  // Debugging
  {
    const char* dbgStr = e_vcard_to_string(vcard, EVC_FORMAT_VCARD_30);
    QCM5_DEBUG << "Modified VCard" << dbgStr;
    FREE(dbgStr);
  }
}

OssoABookContact* QContactABook::convert(const QContact *contact) const
{
  Q_CHECK_PTR(contact);
  
  OssoABookContact* rtn;
  
  // Get aContact if it exists or create a new one if it doesn't
  QContactLocalId id = contact->localId();
  QCM5_DEBUG << "Converting QContact id:" << id << " to aContact";
  if (id){
    rtn = getAContact(id);
    // It's not safe to commit changes to a contact that has been modified.
    // This problem affects attributes with the same name and parameters such as
    // EMail, Address...
    QContactTimestamp* ts = getTimestampDetail(E_CONTACT(rtn));
    if (*ts != contact->detail<QContactTimestamp>()){
      delete ts;
      return NULL;
    }
    delete ts;
  } else {
    rtn = osso_abook_contact_new();
  }
  
  QList<QContactDetail> allDetails = contact->details();
  foreach(const QContactDetail &detail, allDetails){
    QString definitionName = detail.definitionName();
    
    QCM5_DEBUG << "Saving" << definitionName;
    
    //QContactDisplayLabel::DefinitionName
    if (definitionName == QContactAddress::DefinitionName){
      setAddressDetail(rtn, detail);
    } else
    if (definitionName == QContactAvatar::DefinitionName){
      setAvatarDetail(rtn, detail);
    } else
    if (definitionName == QContactBirthday::DefinitionName){
      setBirthdayDetail(rtn, detail);
    } else
    if (definitionName == QContactEmailAddress::DefinitionName){
      setEmailDetail(rtn, detail);
    } else
    if (definitionName == QContactGender::DefinitionName){
      setGenderDetail(rtn, detail);
    } else
    if (definitionName == QContactName::DefinitionName){
      setNameDetail(rtn, detail);
    } else
    if (definitionName == QContactNickname::DefinitionName){
      setNicknameDetail(rtn, detail);
    } else
    if (definitionName == QContactNote::DefinitionName){
      setNoteDetail(rtn, detail);
    } else
    if (definitionName == QContactOnlineAccount::DefinitionName){
      setOnlineAccountDetail(rtn, detail);
    } else
    if (definitionName == QContactOrganization::DefinitionName){
      setOrganizationDetail(rtn, detail);
    } else
    if (definitionName == QContactPhoneNumber::DefinitionName){
      setPhoneDetail(rtn, detail);
    } else
    if (definitionName == QContactThumbnail::DefinitionName){
      setThumbnailDetail(rtn, detail);
    } else
    if (definitionName == QContactUrl::DefinitionName){
      setUrlDetail(rtn, detail);
    }
  }  
  
  return rtn;
}

void QContactABook::setAddressDetail(const OssoABookContact* aContact, const QContactAddress& detail) const
{
  if (!aContact) return;
  
  uint detailUri;
  const uint nAddressElems = 7;
  QStringList adrAttrValues, 
              lblAttrValues,
              paramValues;
  
  // Get parameters
  foreach(QString c, detail.contexts())
    paramValues << c.toUpper();
  
  // Initialize adrAttrValues;
  for (uint i = 0; i < nAddressElems; ++i)
    adrAttrValues << "";

  // Fill adrAttrValues
  QVariantMap vm = detail.variantValues();
  QMapIterator<QString, QVariant> i(vm);
  
  while (i.hasNext()) {
    i.next();
    int index = -1;
    QString key = i.key();
      
    if (key == QContactAddress::FieldPostOfficeBox) index = 0;
    else if (key == "Estension") index = 1;
    else if (key == QContactAddress::FieldStreet) index = 2;
    else if (key == QContactAddress::FieldLocality) index = 3;
    else if (key == QContactAddress::FieldRegion) index = 4;
    else if (key == QContactAddress::FieldPostcode) index = 5;
    else if (key == QContactAddress::FieldCountry) index = 6;  
    else if (key == QContactDetail::FieldContext) continue;
    else if (key == QContactDetail::FieldDetailUri) detailUri = i.value().toInt();
    else {
      qWarning() << "Address contains an invalid field:" << key;
      return;
    }
    
    if (index != -1)
      adrAttrValues[index] = i.value().toString();
  }

  // Fill lblAttrValues
  QStringList labelValues;
  labelValues << adrAttrValues[1] 
              << adrAttrValues[2]
              << adrAttrValues[0]
              << adrAttrValues[3]
              << adrAttrValues[4]
              << adrAttrValues[5]
              << adrAttrValues[6];
  lblAttrValues << labelValues.join(", ");
  
  // Skip if adrAttrValues contains only empty strings
  bool noValues = true;
  foreach(QString s, adrAttrValues){
    if (!s.isEmpty()){
      noValues = false;
      break;
    }
  } 
  if (noValues)
    return;
  
  // Saving LABEL and ADR attributes into the VCard
  addAttributeToAContact(aContact, EVC_ADR, adrAttrValues, EVC_TYPE, paramValues, true, detailUri);
  
  //BUG Label attribute contains a bug
  //It contains TYPE(TYPE) if ADDRESS doesn't contain any parameter value.
  if (paramValues.isEmpty())
    paramValues << EVC_TYPE;
  
  addAttributeToAContact(aContact, EVC_LABEL, lblAttrValues, EVC_TYPE, paramValues, true, detailUri);
}

void QContactABook::setThumbnailDetail(const OssoABookContact* aContact, const QContactThumbnail& detail) const
{
    if (!aContact) return;

    EBook *book;
    {
      OssoABookRoster* roster = A_ROSTER(m_abookAgregator);
      book = osso_abook_roster_get_book(roster);
    }

    QImage image = detail.thumbnail();
    if (image.format() != QImage::Format_ARGB32_Premultiplied)
        image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(image.bits(), GDK_COLORSPACE_RGB,
                                                 image.hasAlphaChannel(), 8,
                                                 image.width(), image.height(),
                                                 image.bytesPerLine(), 0, 0);
    osso_abook_contact_set_pixbuf((OssoABookContact*)aContact, pixbuf, book, 0);
    g_object_unref(pixbuf);
}

void QContactABook::setAvatarDetail(const OssoABookContact* aContact, const QContactAvatar& detail) const
{
// XXX TODO: FIXME
//  if (!aContact) return;
//
//  EBook *book;
//  {
//    OssoABookRoster* roster = A_ROSTER(m_abookAgregator);
//    book = osso_abook_roster_get_book(roster);
//  }
//
//  QImage image = detail.pixmap().toImage();
//  if (image.format() != QImage::Format_ARGB32_Premultiplied)
//      image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
//  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(image.bits(), GDK_COLORSPACE_RGB,
//                                               image.hasAlphaChannel(), 8,
//                                               image.width(), image.height(),
//                                               image.bytesPerLine(), 0, 0);
//  osso_abook_contact_set_pixbuf((OssoABookContact*)aContact, pixbuf, book, 0);
//  g_object_unref(pixbuf);
}

void QContactABook::setBirthdayDetail(const OssoABookContact* aContact, const QContactBirthday& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  attrValues << detail.value(QContactBirthday::FieldBirthday);
  
  addAttributeToAContact(aContact, EVC_BDAY, attrValues);
}

void QContactABook::setEmailDetail(const OssoABookContact* aContact, const QContactEmailAddress& detail) const
{
  if (!aContact) return;
  QStringList attrValues,
              paramValues;

  QVariantMap vm = detail.variantValues();
  QMapIterator<QString, QVariant> i(vm);
  while (i.hasNext()) {
    i.next();
    QString key = i.key();
    
    // We don't want to save the Detail URI
    if (key == QContactDetail::FieldDetailUri)
      continue;
    
    if (key == QContactDetail::FieldContext)
      paramValues << i.value().toString().toUpper();
    else
      attrValues << i.value().toString();
  }
  
  addAttributeToAContact(aContact, EVC_EMAIL, attrValues, EVC_TYPE, paramValues, true, detail.detailUri().toInt());
}

void QContactABook::setGenderDetail(const OssoABookContact* aContact, const QContactGender& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  attrValues << detail.value(QContactGender::FieldGender).toLower();
  
  addAttributeToAContact(aContact, "X-GENDER", attrValues);
}

void QContactABook::setNameDetail(const OssoABookContact* aContact, const QContactName& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  // Save First and Last name in the N vcard attribute
  {  
    QStringList supportedDetailValues;
    supportedDetailValues << QContactName::FieldFirstName << QContactName::FieldLastName;
  
    foreach(QString key, supportedDetailValues){
      attrValues << detail.value(key);
    }
  
    //REMOVE ME - We don't want to support custom label
    if (attrValues[0].isEmpty()){
      qWarning() << "QContactName::FieldFirstName is empty";
      attrValues[0] = detail.customLabel();
    }
  
    addAttributeToAContact(aContact, EVC_N, attrValues);
  }
  
  // Save Fist + Last name in the FN card attribute
  {
    attrValues << attrValues.join(" ");
    addAttributeToAContact(aContact, EVC_FN, attrValues);
  }
}

void QContactABook::setNicknameDetail(const OssoABookContact* aContact, const QContactNickname& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  attrValues << detail.value(QContactNickname::FieldNickname);
  
  addAttributeToAContact(aContact, EVC_NICKNAME, attrValues);
}

void QContactABook::setNoteDetail(const OssoABookContact* aContact, const QContactNote& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  attrValues << detail.value(QContactNote::FieldNote);
  
  addAttributeToAContact(aContact, EVC_NOTE, attrValues);
}

/*NOTE: Online details comes from Telepathy or can be added manually by the user.
 *      OnlineDetals coming from Telepathy/Roster contacts can't be saved.
 */
void QContactABook::setOnlineAccountDetail(const OssoABookContact* aContact, const QContactOnlineAccount& detail) const
{
   if (!aContact)
     return;
   
   Q_UNUSED(detail);
}

void QContactABook::setOrganizationDetail(const OssoABookContact* aContact, const QContactOrganization& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  attrValues << detail.value(QContactOrganization::FieldTitle);
  
  addAttributeToAContact(aContact, EVC_ORG, attrValues);
}

void QContactABook::setPhoneDetail(const OssoABookContact* aContact, const QContactPhoneNumber& detail) const
{
  if (!aContact) return;
  QStringList attrValues,
              paramValues;

  QVariantMap vm = detail.variantValues();
  QMapIterator<QString, QVariant> i(vm);
  while (i.hasNext()) {
    i.next();
    const QString key = i.key();
    
    // We don't want to save the Detail URI
    if (key == QContactDetail::FieldDetailUri)
      continue;
    
    if (key == QContactDetail::FieldContext ||
        key == QContactPhoneNumber::FieldSubTypes){
      QString value = i.value().toString();
      if (value == QContactPhoneNumber::SubTypeMobile)
        value = "CELL";
      paramValues << value.toUpper();
    } else
      attrValues << i.value().toString();
  }
  
  // Avoid unsupported type
  if (paramValues.isEmpty())
    paramValues << "VOICE";
  
  addAttributeToAContact(aContact, EVC_TEL, attrValues, EVC_TYPE, paramValues, true, detail.detailUri().toInt());
}

void QContactABook::setUrlDetail(const OssoABookContact* aContact, const QContactUrl& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  attrValues << detail.value(QContactUrl::FieldUrl);
  
  addAttributeToAContact(aContact, EVC_URL, attrValues);
}
