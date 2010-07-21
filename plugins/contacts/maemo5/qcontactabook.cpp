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
#include "qcontactabook_p.h"

#include <QEventLoop>
#include <libebook/e-book-util.h>

#include <libmcclient/mc-account-manager.h>

/* Error handling Macros */
#define FATAL_IF_ERROR(x) if(x) { \
                            QString message(x->message); \
                            g_error_free(x); \
                            qFatal(qPrintable(message)); \
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

struct jobSharedData{
   QContactABook* that;
   bool *result;
   char *uid;
   QContactManager::Error *error;
};

/* QContactABook */
QContactABook::QContactABook(QObject* parent) :QObject(parent), m_cbSD(0), m_deleteJobSD(0), m_saveJobSD(0)
{
  //Initialize QContactDetail context list
  initAddressBook();
}

QContactABook::~QContactABook()
{
  OssoABookAggregator *roster = reinterpret_cast<OssoABookAggregator*>(m_abookAgregator);
  if (g_signal_handler_is_connected(roster, m_contactAddedHandlerId))
      g_signal_handler_disconnect(roster, m_contactAddedHandlerId);
  if (g_signal_handler_is_connected(roster, m_contactChangedHandlerId))
      g_signal_handler_disconnect(roster, m_contactChangedHandlerId);
  if (g_signal_handler_is_connected(roster, m_contactRemovedHandlerId))
      g_signal_handler_disconnect(roster, m_contactRemovedHandlerId);

  // XXX FIXME: memory leak?
  //g_object_unref(m_abookAgregator);
  delete m_cbSD;
  m_cbSD = 0;
  delete m_deleteJobSD;
  m_deleteJobSD = 0;
  delete m_saveJobSD;
  m_saveJobSD = 0;
}

static void contactsAddedCB(OssoABookRoster *roster, OssoABookContact **contacts, gpointer data)
{
  QCM5_DEBUG << "CONTACT ADDED";
  Q_UNUSED(roster)
  
  cbSharedData* d = static_cast<cbSharedData*>(data);
  if (!d){
    return;
  }
  
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

  if (!contactIds.isEmpty())
    d->that->_contactsAdded(contactIds);
}

static void contactsChangedCB(OssoABookRoster *roster, OssoABookContact **contacts, gpointer data)
{
  QCM5_DEBUG << "CONTACT CHANGED";
  Q_UNUSED(roster)
  
  cbSharedData* d = static_cast<cbSharedData*>(data);
  if (!d){
    return;
  }
  
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

  if (!contactIds.isEmpty())
    d->that->_contactsChanged(contactIds);
}

static void contactsRemovedCB(OssoABookRoster *roster, const char **ids, gpointer data)
{
  QCM5_DEBUG << "CONTACT REMOVED";
  Q_UNUSED(roster)
  
  cbSharedData* d = static_cast<cbSharedData*>(data);
  if (!d){
    return;
  }
  
  const char **p;
  QList<QContactLocalId> contactIds;
  
  for (p = ids; *p; ++p) {
      QContactLocalId id = d->hash->take(*p);
      if (id) {
        QCM5_DEBUG << "Contact" << id << "has been removed";
        contactIds << id;
      }
  }
  
  if (!contactIds.isEmpty())
    d->that->_contactsRemoved(contactIds);
}

void QContactABook::initAddressBook(){
  /* Open AddressBook */
  GError *gError = NULL;
  OssoABookRoster* roster = NULL;
  
  roster = osso_abook_aggregator_get_default(&gError);
  FATAL_IF_ERROR(gError)
  
  osso_abook_waitable_run((OssoABookWaitable *) roster, g_main_context_default(), &gError);
  FATAL_IF_ERROR(gError)
  
  if (!osso_abook_waitable_is_ready ((OssoABookWaitable *) roster, &gError))
    FATAL_IF_ERROR(gError)
  
  m_abookAgregator = reinterpret_cast<OssoABookAggregator*>(roster);
  
  /* Initialize local Id Hash */
  initLocalIdHash();
  
  /* Initialize callbacks shared data */
  m_cbSD = new cbSharedData;
  m_cbSD->hash = &m_localIds;
  m_cbSD->that = this;
   
  /* Setup signals */
  m_contactAddedHandlerId = g_signal_connect(roster, "contacts-added",
                   G_CALLBACK (contactsAddedCB), m_cbSD);
  m_contactChangedHandlerId = g_signal_connect(roster, "contacts-changed",
                   G_CALLBACK (contactsChangedCB), m_cbSD);
  m_contactRemovedHandlerId = g_signal_connect(roster, "contacts-removed",
                   G_CALLBACK (contactsRemovedCB), m_cbSD);
  
  //TEST Lists supported fields
  if (QCM5_DEBUG_ENABLED){
    EBook *book = NULL;
    GList *l;
    book = osso_abook_roster_get_book(roster);
    e_book_get_supported_fields (book, &l, NULL);
    while (l) {
      qDebug() << "SUPPORTED FIELD" << (const char*)l->data;
      l = l->next;  
    }
    g_list_free(l);
  }
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
     QCM5_DEBUG << "There are no Master contacts. LocalId hash is empty.";
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
     if (QCM5_DEBUG_ENABLED)
       e_vcard_dump_structure((EVCard*)contact);
   }
   
   g_list_free(contactList);
}

//TODO Use native filters
QList<QContactLocalId> QContactABook::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
  Q_UNUSED(sortOrders)
  Q_UNUSED(filter);
  *error = QContactManager::NoError;
  return m_localIds.keys();

  /*
  // Sorting
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
    return QContactManagerEngine::sortContacts(contacts, sortOrders);
  }
  
  switch(filter.type()){
    case QContactFilter::DefaultFilter: {
      rtn = m_localIds.keys();
    } break;
    default: { 
      EBookQuery* query = convert(filter);
      GList* l = osso_abook_aggregator_find_contacts(m_abookAgregator, query);
      if (query)
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
    }
  }
  *error = QContactManager::NoError;
  return rtn;
  */
}

QContact* QContactABook::getQContact(const QContactLocalId& contactId, QContactManager::Error* error) const
{
  QContact *rtn;
  OssoABookContact* aContact = getAContact(contactId, error);
  if (!aContact) {
    return new QContact;
  }
  
  //Convert aContact => qContact
  rtn = convert(E_CONTACT(aContact));
  
  QContactId cId;
  cId.setLocalId(contactId);
  rtn->setId(cId);
  
  return rtn;
}

static QContactManager::Error getErrorFromStatus(const EBookStatus status){
  switch (status) {
    case E_BOOK_ERROR_OK:
      return QContactManager::NoError;
    case E_BOOK_ERROR_INVALID_ARG:
      return QContactManager::BadArgumentError;
    case E_BOOK_ERROR_BUSY:
      return QContactManager::LockedError;        
    case E_BOOK_ERROR_PERMISSION_DENIED:
    case E_BOOK_ERROR_AUTHENTICATION_FAILED:
    case E_BOOK_ERROR_AUTHENTICATION_REQUIRED:
      return QContactManager::PermissionsError;
    case E_BOOK_ERROR_CONTACT_NOT_FOUND:
      return QContactManager::DoesNotExistError;
    case E_BOOK_ERROR_CONTACT_ID_ALREADY_EXISTS:
      return QContactManager::AlreadyExistsError;
    case E_BOOK_ERROR_NO_SPACE:
      return QContactManager::OutOfMemoryError;
#if 0
    case E_BOOK_ERROR_REPOSITORY_OFFLINE:
    case E_BOOK_ERROR_NO_SUCH_BOOK:
    case E_BOOK_ERROR_NO_SELF_CONTACT:
    case E_BOOK_ERROR_SOURCE_NOT_LOADED:
    case E_BOOK_ERROR_SOURCE_ALREADY_LOADED:
    case E_BOOK_ERROR_PROTOCOL_NOT_SUPPORTED:
    case E_BOOK_ERROR_CANCELLED:
    case E_BOOK_ERROR_COULD_NOT_CANCEL:
    case E_BOOK_ERROR_TLS_NOT_AVAILABLE:
    case E_BOOK_ERROR_CORBA_EXCEPTION:
    case E_BOOK_ERROR_NO_SUCH_SOURCE:
    case E_BOOK_ERROR_OFFLINE_UNAVAILABLE:
    case E_BOOK_ERROR_OTHER_ERROR:
    case E_BOOK_ERROR_INVALID_SERVER_VERSION:
#endif
    default:
      return QContactManager::UnspecifiedError;
  }
}

static void delContactCB(EBook *book, EBookStatus status, gpointer closure)
{
  Q_UNUSED(book);
  QCM5_DEBUG << "Contact Removed";
  
  jobSharedData *sd = static_cast<jobSharedData*>(closure);
  if (!sd)
    return;
  
  *sd->result = (status != E_BOOK_ERROR_OK &&
                 status != E_BOOK_ERROR_CONTACT_NOT_FOUND) ? false : true;
  *sd->error = getErrorFromStatus(status);
  
  sd->that->_jobRemovingCompleted();
}

bool QContactABook::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
  Q_UNUSED(error);
  QMutexLocker locker(&m_delContactMutex);

  bool ok = false;
  
  OssoABookRoster *roster = A_ROSTER(m_abookAgregator);
  EBook *book = osso_abook_roster_get_book(roster);
  OssoABookContact *aContact = getAContact(contactId, error);
  if (!OSSO_ABOOK_IS_CONTACT(aContact)){
    QCM5_DEBUG << "Specified contact is not a valid ABook contact";
    return false;
  }
  
  // ASync => Sync
  QEventLoop loop;                           
  connect(this, SIGNAL(jobRemovingCompleted()), &loop, SLOT(quit()));
  
  // Prepare shared data
  if (m_deleteJobSD){
    delete m_deleteJobSD;
    m_deleteJobSD = 0;
  }
  m_deleteJobSD = new jobSharedData;
  m_deleteJobSD->that = this;
  m_deleteJobSD->result = &ok;
  m_deleteJobSD->error = error;
  
  //Remove photos
  EContactPhoto *photo = NULL;
  GFile *file = NULL;
  photo = (EContactPhoto*) e_contact_get(E_CONTACT (aContact), E_CONTACT_PHOTO);
  if (photo) {
    if (photo->type == E_CONTACT_PHOTO_TYPE_URI && photo->data.uri) {
      file = g_file_new_for_uri(photo->data.uri);
      g_file_delete(file, NULL, NULL);
      g_object_unref (file);
    }
    e_contact_photo_free (photo);
  }
  
  //Remove all roster contacts from their roster
  GList* rosterContacts = NULL;
  rosterContacts = osso_abook_contact_get_roster_contacts(aContact);
  const char *masterUid = CONST_CHAR(e_contact_get_const(E_CONTACT(aContact), E_CONTACT_UID));
  char *contactUidCopy = strdup(masterUid);
  while(rosterContacts){
    OssoABookContact *rosterContact = A_CONTACT(rosterContacts->data);
    osso_abook_contact_reject_for_uid(rosterContact, masterUid, NULL);
    rosterContacts = rosterContacts->next;
  }  
  
  // Remove contact
  e_book_async_remove_contact(book, E_CONTACT(aContact),
                              delContactCB, m_deleteJobSD);
  
  loop.exec(QEventLoop::AllEvents|QEventLoop::WaitForMoreEvents);

  // update our list of ids...
  QContactLocalId id = m_localIds[contactUidCopy];
  m_localIds.remove(contactUidCopy);
  if (contactUidCopy)
    free(contactUidCopy);
  
  if (id)
    _contactsRemoved(QList<QContactLocalId>() << id);
  
  return ok;
}

static void commitContactCB(EBook* book, EBookStatus  status, gpointer user_data)
{
  Q_UNUSED(book)
  jobSharedData *sd = static_cast<jobSharedData*>(user_data);
  if (!sd)
    return;
  
  *sd->result = (status == E_BOOK_ERROR_OK) ? true : false;
  *sd->error = getErrorFromStatus(status);
  sd->that->_jobSavingCompleted();
}

static void addContactCB(EBook* book, EBookStatus  status, const char  *uid, gpointer user_data)
{
  jobSharedData *sd = static_cast<jobSharedData*>(user_data);
  if (!sd)
    return;
  
  if (uid)
    sd->uid = strdup(uid);

  //### FIXME IS THIS LINE REALLY NEEDED: osso_abook_contact_set_roster(OssoABookContact *contact, OssoABookRoster *roster)
  *sd->result = (status == E_BOOK_ERROR_OK) ? true : false;
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
  
  // Convert QContact to AContact
  aContact = convert(contact, error);
  if (!aContact){
    return false;
  }  

  // ASync => Sync
  QEventLoop loop;
  connect(this, SIGNAL(jobSavingCompleted()), &loop, SLOT(quit()));

  // Prepare shared data
  if (m_saveJobSD){
    delete m_saveJobSD;
    m_saveJobSD = 0;
  }
  m_saveJobSD = new jobSharedData;
  m_saveJobSD->that = this;
  m_saveJobSD->result = &ok;
  m_saveJobSD->error = error;
  m_saveJobSD->uid = 0;
  
  // Add/Commit the contact
  uid = CONST_CHAR(e_contact_get_const(E_CONTACT (aContact), E_CONTACT_UID)); 
  if (uid) {
    m_saveJobSD->uid = strdup(uid);
    osso_abook_contact_async_commit(aContact, book, commitContactCB, m_saveJobSD);
  } else {
    osso_abook_contact_async_add(aContact, book, addContactCB, m_saveJobSD);
  }
  
  loop.exec(QEventLoop::AllEvents|QEventLoop::WaitForMoreEvents);

  // save the newly saved contact's id in the hash.
  m_localIds << m_saveJobSD->uid;

  // set the id of the contact.
  QContactId cId;
  cId.setLocalId(m_localIds[m_saveJobSD->uid]);
  contact->setId(cId);
  if (m_saveJobSD->uid)
      free(m_saveJobSD->uid);
  
  return ok;
}

const QString QContactABook::getDisplayName(const QContact& contact) const{
  //Get Osso ABook ID for the contact (stored as GUID detail)
  const char* acontactID = NULL;
  {
    QContactGuid g = contact.detail(QContactGuid::DefinitionName);
    acontactID = qPrintable(g.guid());
  }
  
  if (!acontactID){
    QCM5_DEBUG << "The contact has not been saved yet and it doesn't have any GUID";
    return QString();
  }
    
  //Get OssoABookContact
  OssoABookContact *acontact= NULL;
  {
    GList* l= NULL;
    l = osso_abook_aggregator_lookup(m_abookAgregator, acontactID);
    
    if (g_list_length(l) == 1) {
      acontact = A_CONTACT(l->data);
    }
    g_list_free(l);
    
  }
  
  if (!acontact){
    QCM5_DEBUG << "AContact with ID:" << acontactID << "is null";
    return QString();
  }
  //Get Display name;
  const char* displayName = osso_abook_contact_get_display_name(acontact);  

  return QString::fromUtf8(displayName);
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
    QCM5_DEBUG << "Cannot find the self contact";
    *errors = QContactManager::DoesNotExistError;
    id = 0;
  }
  g_object_unref(self);
  return id;
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
        hash[QContactOrganization::DefinitionName] = "org";
        hash[QContactPhoneNumber::DefinitionName] = "phone";
        hash[QContactUrl::DefinitionName] = "homepage-url";
      }
  
      QString eDetail = hash[f.detailDefinitionName()];
      if (eDetail.isEmpty()){
        return NULL;
      }
      queryStr = queryStr + " \"" + eDetail + "\" \"" + f.value().toString() + "\"";
      query = e_book_query_from_string(qPrintable(queryStr));
    } break;
    case QContactFilter::ActionFilter:
      break;
    case QContactFilter::IntersectionFilter:
    {
      QCM5_DEBUG << "IntersectionFilter";
      const QContactIntersectionFilter f(filter);
      const QList<QContactFilter>  fs= f.filters();
      QContactFilter i;
      foreach(i, fs){
        EBookQuery* q = convert(i);
        if (!q)
          continue;
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
      query = convert(QContactInvalidFilter());
  }
 
  //Debugging
  if (QCM5_DEBUG_ENABLED){
    const char *queryString = e_book_query_to_string(query);
    QCM5_DEBUG << "QUERY" << queryString;
    FREE(queryString);
  }
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

  /* Online Account & presences*/
  QList<QContactOnlineAccount*> onlineAccounts;
  QList<QContactPresence*> presences;
  getOnlineAccountAndPresenceDetails(eContact, onlineAccounts, presences);
  
  QContactOnlineAccount* onlineAccount;
  foreach(onlineAccount, onlineAccounts)
    detailList << onlineAccount;
  
  QContactPresence* presence;
  foreach(presence, presences)
    detailList << presence;

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

OssoABookContact* QContactABook::getAContact(const QContactLocalId& contactId, QContactManager::Error* error) const
{
  OssoABookContact* rtn = NULL;

  QCM5_DEBUG << "Getting aContact with id " << m_localIds[contactId] << "local contactId is" << contactId;

  if(QString::fromAscii(m_localIds[contactId]).compare("osso-abook-self") == 0) {
    *error = QContactManager::NoError;
    rtn = A_CONTACT(osso_abook_self_contact_get_default());
  } else {
    GList* c = osso_abook_aggregator_lookup(m_abookAgregator, m_localIds[contactId]);
    if (c)
      rtn = A_CONTACT(c->data);
    *error = rtn ? QContactManager::NoError : QContactManager::DoesNotExistError;
    return rtn;
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
                << AddressFieldExtension //XXX FIXME I'm not sure we have to use a new field
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
    if (!v) {
      // ADR attribute data is corrupted.  Skipping.
      g_list_free(attrList);
      return rtnList;
    }

    int i = 0;
    while (v && i < 7) {
      // we only deal with the first 7 fields: pobox, extension, street, locality, region, postcode, country.
      map[addressFields[i]] = QString::fromUtf8(CONST_CHAR(v->data));
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
    map[QContactName::FieldPrefix] = eContactName->prefixes;
    map[QContactName::FieldSuffix] = eContactName->suffixes;
    e_contact_name_free (eContactName);
  } else {
    //Looks that Maemo use just these two fields
    map[QContactName::FieldFirstName] = QString::fromUtf8(CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_GIVEN_NAME)));
    map[QContactName::FieldLastName] = QString::fromUtf8(CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_FAMILY_NAME)));
  }
  setDetailValues(map, rtn);
  return rtn;
}

QContactNickname* QContactABook::getNicknameDetail(EContact *eContact) const
{
  QContactNickname* rtn = new QContactNickname;
  QVariantMap map;
  map[QContactNickname::FieldNickname] = QString::fromUtf8(CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_NICKNAME)));
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
      map[QContactEmailAddress::FieldEmailAddress] = QString::fromUtf8(CONST_CHAR(v->data));
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
    Q_UNUSED(eContact);
    // XXX TODO
    // Should be retrieved from EVC_PHOTO

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
  QString gender = g;
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
  map[QContactNote::FieldNote] = QString::fromUtf8(note);
  FREE(note);
  setDetailValues(map, rtn);
  return rtn;
}

static const QStringList vcardsManagedByTelepathy(){
  static QStringList rtn;
  
  if (rtn.isEmpty()){
    OssoABookAccountManager* accountMgr = osso_abook_account_manager_get_default();
    const GList *vcardFields = osso_abook_account_manager_get_primary_vcard_fields(accountMgr);
    while (vcardFields){
      QString field = (const char*)vcardFields->data;
      if (!rtn.contains(field) && field != "TEL")
        rtn << field;
      vcardFields = vcardFields->next;
    }
    FREE(vcardFields);
  }
  
  QCM5_DEBUG << "VCards managed by Telepathy:" << rtn;
  return rtn;
}

static void populateProfilesCapabilitiesMap(QMap<QString, QStringList>* map){
  GList* l =  mc_profiles_list(); //Don't free this!!
  GList* node;
  
  map->clear();

  for (node = l; node != NULL; node = g_list_next(node)) {
    McProfile* p = (McProfile*) node->data; //Don't free this!!
    
    //Get serviceProvider name from the profile
    QString serviceProvider = mc_profile_get_unique_name(p);
    if (serviceProvider.isEmpty())
      continue;
    
    // Get capabilities
    McProfileCapabilityFlags caps = mc_profile_get_capabilities(p);
    QStringList capList;
    
    // MC_PROFILE_CAPABILITY_NONE skipped
    if (caps & MC_PROFILE_CAPABILITY_CHAT_P2P)
      capList << "CHAT_P2P";
    if (caps & MC_PROFILE_CAPABILITY_CHAT_ROOM)
      capList << "CHAT_ROOM";
    if (caps & MC_PROFILE_CAPABILITY_CHAT_ROOM_LIST)
      capList << "CHAT_ROOM_LIST";
    if (caps & MC_PROFILE_CAPABILITY_VOICE_P2P)
      capList << "VOICE_P2P";
    if (caps & MC_PROFILE_CAPABILITY_CONTACT_SEARCH)
      capList << "CONTACT_SEARCH";
    if (caps & MC_PROFILE_CAPABILITY_SPLIT_ACCOUNT)
      capList << "SPLIT_ACCOUNT";
    if (caps & MC_PROFILE_CAPABILITY_REGISTRATION_UI)
      capList << "REGISTRATION_UI";
    if (caps & MC_PROFILE_CAPABILITY_SUPPORTS_AVATARS)
      capList << "SUPPORTS_AVATARS";
    if (caps & MC_PROFILE_CAPABILITY_SUPPORTS_ALIAS)
      capList << "SUPPORTS_ALIAS";
    if (caps & MC_PROFILE_CAPABILITY_SUPPORTS_ROSTER)
      capList << "SUPPORTS_ROSTER";
    if (caps & MC_PROFILE_CAPABILITY_VIDEO_P2P)
      capList << "VIDEO_P2P";
    if (caps & MC_PROFILE_CAPABILITY_CHAT_UPGRADE)
      capList << "CHAT_UPGRADE";
    
    // Store into the map
    map->insert(serviceProvider, capList); 
  }  
  
}

static const QStringList serviceProviderCapabilities(const QString& serviceProvider){
  static QMap<QString, QStringList> map;
  
  if (map.isEmpty())
    populateProfilesCapabilitiesMap(&map);
  
  if (serviceProvider.isEmpty())
    return QStringList();
  
  QStringList rtn = map.value(serviceProvider);
  if (!rtn.isEmpty())
    return rtn;
  else
    return QStringList();
}

#if 0
// BUG? This code has been commented out because osso_abook_presence_get_presence_status returns empty strings
static QContactPresence::PresenceState telepathyStatusToPresenceState(const QString& status){
  if (status == "offline")
    return QContactPresence::PresenceOffline;
  else if (status == "unknown" || status == "error")
    return QContactPresence::PresenceUnknown;
  else if (status == "available")
    return QContactPresence::PresenceAvailable;
  else if (status == "away" || status == "brb" ) //Be Right Back (a more specific form of Away)
    return QContactPresence::PresenceAway;
  else if (status == "busy" || status == "dnd") //Do Not Disturb (a more specific form of Busy)
    return QContactPresence::PresenceBusy;
  else if (status == "xa") //Extended Away
    return QContactPresence::PresenceExtendedAway;
  else if (status == "hidden") // "Invisible" or "Appear Offline"
    return QContactPresence::PresenceHidden;
}
#endif

void QContactABook::getOnlineAccountAndPresenceDetails(EContact *eContact, 
                                                      QList<QContactOnlineAccount*>& onlineAccounts,
                                                      QList<QContactPresence*>& presences) const
{
  const QStringList telepathyVCards = vcardsManagedByTelepathy();
  
  // Parsing Attributes associated to the Telepathy VCards
  GList *attributeList = e_vcard_get_attributes((EVCard*)eContact);
  GList *node;

  if (!attributeList)
    return;
  
  for (node = attributeList; node != NULL; node = g_list_next (node)) {
    QContactOnlineAccount* onlineAccount = new QContactOnlineAccount;
    QContactPresence* contactPresence = new QContactPresence;
    
    const char* accountUri = NULL;
    const char* serviceProvider = NULL;
    const char* accountPath = NULL; // Outgoing account path eg: SERVICE_NAME/PROTOCOL_NAME/USER_NAME
    EVCardAttribute* attr = NULL;
    QVariantMap map;
    QString presenceMsg, presenceIcon, presenceDisplayState, presenceNickname;
    QStringList caps;
    
    attr = (EVCardAttribute*)node->data;
    if (!attr)
      continue;
    
    // Continue if the attribute doesn't contain Online Account info
    QString attributeName = e_vcard_attribute_get_name(attr);
    if (!telepathyVCards.contains(attributeName))
      continue;
    
    // Get the account URI
    accountUri = e_vcard_attribute_get_value(attr);

    // Get AccountPath and service provider for the roster contact associated to the attribute
    // Note: there should be just one roster contact associated to the attribute
    GList* rContacts = osso_abook_contact_find_roster_contacts_for_attribute(A_CONTACT(eContact), attr);
    for (GList * node = rContacts; node != NULL; node = g_list_next(node)){
      OssoABookContact* c = NULL;
      c = A_CONTACT(node->data);
      if (c) {
       McAccount* a = NULL;
       OssoABookPresence *p = NULL;
       
       a = osso_abook_contact_get_account(c);
       if (a){
         accountPath = a->name;
         serviceProvider = mc_account_compat_get_profile(a);
       }
       
       p = OSSO_ABOOK_PRESENCE(c);
       presenceMsg = QString::fromUtf8(osso_abook_presence_get_presence_status_message(p));
       presenceIcon = QString::fromUtf8(osso_abook_presence_get_icon_name(p));
       // presenceState = QString::fromUtf8(osso_abook_presence_get_presence_status(p)); //BUG in osso_abook_presence_get_presence_status??
       presenceDisplayState = QString::fromUtf8(osso_abook_presence_get_display_status(p));
       presenceNickname = QString::fromUtf8(osso_abook_contact_get_display_name(c));
       // Set OnlineAccount details
       map.clear();
       map[QContactOnlineAccount::FieldAccountUri] = accountUri;
       map[QContactOnlineAccount::FieldCapabilities] = serviceProviderCapabilities(serviceProvider);
       map[QContactOnlineAccount::FieldServiceProvider] = serviceProvider; // eg: facebook-chat,
       map["AccountPath"] = accountPath;
       setDetailValues(map, onlineAccount);
       onlineAccounts << onlineAccount;
    
       // Set OnlinePresence details
       map.clear();
       map[QContactDetail::FieldLinkedDetailUris] = accountUri;
       map[QContactPresence::FieldCustomMessage] = presenceMsg;
       map[QContactPresence::FieldPresenceStateImageUrl] = presenceIcon;
       map[QContactPresence::FieldPresenceStateText] = presenceDisplayState;
       map[QContactPresence::FieldNickname] = presenceNickname;
       //map[QContactPresence::FieldPresenceState] = telepathyStatusToPresenceState(presenceState);
       //map[QContactPresence::FieldTimestamp] = ;
       setDetailValues(map, contactPresence);
       presences << contactPresence;
      }
    }
  }
}

QContactOrganization* QContactABook::getOrganizationDetail(EContact *eContact) const
{
  QContactOrganization* rtn = new QContactOrganization;
  QVariantMap map;
  const char* org = CONST_CHAR(e_contact_get(eContact, E_CONTACT_ORG));
  map[QContactOrganization::FieldName] = QString::fromUtf8(org);
  FREE(org);
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
      QString value = CONST_CHAR(p->data);
      
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

  GdkPixbuf* pixbuf = osso_abook_avatar_get_image_rounded(OSSO_ABOOK_AVATAR(aContact), -1, -1, false, 0, NULL);
  if (!GDK_IS_PIXBUF(pixbuf)){
    FREE(pixbuf);
    return rtn;
  }

  const uchar* bdata = (const uchar*)gdk_pixbuf_get_pixels(pixbuf);
  QSize bsize(gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf));

  //Convert GdkPixbuf to QPixmap
  QImage::Format format = gdk_pixbuf_get_has_alpha(pixbuf) ? QImage::Format_ARGB32 : QImage::Format_RGB32;
  int stride = gdk_pixbuf_get_rowstride(pixbuf);
  QImage converted(bdata, bsize.width(), bsize.height(), stride, format);
  converted = converted.rgbSwapped();
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
                                   const QString& paramName = QString(), const QStringList& paramValues = QStringList())
{
  if (!contact)
    return;
  
  EVCard *vcard = E_VCARD (contact);
  EVCardAttribute *attr = NULL;
  EVCardAttributeParam* param = NULL;
  
  QCM5_DEBUG << "Adding attribute" << attrName << "AttrValues:" << attrValues
             << "ParamName:" << paramName << "ParamValues:" << paramValues;
  
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

OssoABookContact* QContactABook::convert(const QContact *contact, QContactManager::Error* error) const
{
  Q_CHECK_PTR(contact);

  // first, check for uniqueness constraints.
  // currently, it is only addresses, email addresses, phone numbers
  // and online accounts which are NOT unique.
  if (contact->details<QContactAvatar>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactBirthday>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactGender>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactName>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactNickname>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactNote>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactOrganization>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactThumbnail>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }
  if (contact->details<QContactUrl>().count() > 1) {
      *error = QContactManager::LimitReachedError;
      return 0;
  }

  OssoABookContact* rtn;
  
  // Get aContact if it exists or create a new one if it doesn't
  QContactLocalId id = contact->localId();
  QCM5_DEBUG << "Converting QContact id:" << id << " to aContact";
  if (id) {
    rtn = getAContact(id, error);
    EVCardAttribute *uidAttr = e_vcard_get_attribute(E_VCARD(rtn), e_contact_vcard_attribute(E_CONTACT_UID));

    // remove all current attributes, since we rewrite them all.
    EVCardAttribute *attr;
    GList *attr_list = e_vcard_get_attributes (E_VCARD (rtn));
    while (attr_list) {
        attr = static_cast<EVCardAttribute*>(attr_list->data);
        attr_list = attr_list->next;
        if (!osso_abook_contact_attribute_is_readonly (attr)) {
            if (attr != uidAttr) {
                // we don't remove the uid, since we are updating the contact.
                e_vcard_remove_attribute (E_VCARD (rtn), attr);
            }
        }
    }
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
    else if (key == AddressFieldExtension) index = 1;
    else if (key == QContactAddress::FieldStreet) index = 2;
    else if (key == QContactAddress::FieldLocality) index = 3;
    else if (key == QContactAddress::FieldRegion) index = 4;
    else if (key == QContactAddress::FieldPostcode) index = 5;
    else if (key == QContactAddress::FieldCountry) index = 6;  
    else if (key == QContactDetail::FieldContext) continue;
    else if (key == QContactDetail::FieldDetailUri) detailUri = i.value().toInt();
    else {
      //qWarning() << "Address contains an invalid field:" << key;
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
  addAttributeToAContact(aContact, EVC_ADR, adrAttrValues, EVC_TYPE, paramValues);
  
  //BUG Label attribute contains a bug
  //It contains TYPE(TYPE) if ADDRESS doesn't contain any parameter value.
  if (paramValues.isEmpty())
    paramValues << EVC_TYPE;
  
  addAttributeToAContact(aContact, EVC_LABEL, lblAttrValues, EVC_TYPE, paramValues);
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
    
    if (image.isNull())
      return;

    if (image.hasAlphaChannel()) {
        image = image.convertToFormat(QImage::Format_ARGB32);
        image = image.rgbSwapped();
    } else {
        image = image.convertToFormat(QImage::Format_RGB888);
    }

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(image.bits(), GDK_COLORSPACE_RGB,
                                                 image.hasAlphaChannel(), 8,
                                                 image.width(), image.height(),
                                                 image.bytesPerLine(), 0, 0);

    osso_abook_contact_set_pixbuf((OssoABookContact*)aContact, pixbuf, 0, 0);
    g_object_unref(pixbuf);
}

void QContactABook::setAvatarDetail(const OssoABookContact* aContact, const QContactAvatar& detail) const
{
  Q_UNUSED(aContact)
  Q_UNUSED(detail);
  // XXX TODO: FIXME
  // We should set the path of the avatar in EVC_PHOTO
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
  
  addAttributeToAContact(aContact, EVC_EMAIL, attrValues, EVC_TYPE, paramValues);
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
    supportedDetailValues << QContactName::FieldLastName << QContactName::FieldFirstName;
  
    foreach(QString key, supportedDetailValues){
      attrValues << detail.value(key);
    }
  
    //REMOVE ME - We don't want to support custom label
    if (attrValues[1].isEmpty()){
      //qWarning() << "QContactName::FieldFirstName is empty";
      attrValues[1] = detail.customLabel();
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
  attrValues << detail.value(QContactOrganization::FieldName);
  
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
      else if (value == QContactPhoneNumber::SubTypeVoice)
        value = "VOICE";
      paramValues << value.toUpper();
    } else
      attrValues << i.value().toString();
  }
  
  // Avoid unsupported type
  if (paramValues.isEmpty())
    paramValues << "VOICE";

  // new phone number detail.
  addAttributeToAContact(aContact, EVC_TEL, attrValues, EVC_TYPE, paramValues);
}

void QContactABook::setUrlDetail(const OssoABookContact* aContact, const QContactUrl& detail) const
{
  if (!aContact) return;
  
  QStringList attrValues;
  attrValues << detail.value(QContactUrl::FieldUrl);
  
  addAttributeToAContact(aContact, EVC_URL, attrValues);
}
