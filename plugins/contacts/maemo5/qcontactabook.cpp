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

#include "qcontactabook_p.h"

#include <libebook/e-book-util.h>

#include "qcontactmaemo5debug_p.h"


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
                          
#define A_CONTACT(x) reinterpret_cast<OssoABookContact*>(x)
#define A_ROSTER(x) reinterpret_cast<OssoABookRoster*>(x)
#define CONST_CHAR(x) static_cast<const char*>(x)

#define FREE(x) free((void*)x)

/* QContactABook */
QContactABook::QContactABook(QObject* parent) :QObject(parent)
{
  //Initialize QContactDetail context list
  initAddressBook();
}

QContactABook::~QContactABook()
{
  g_object_unref(m_abookAgregator);
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
  
  //TODO Set up signals for added/changed eContact
  
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
     QByteArray localId = QByteArray::fromRawData(data, sizeof(data));
     m_localIds << localId;
     QCM5_DEBUG << "eContactID " << localId << "has been stored in m_localIDs with key" << m_localIds[localId];
     
     // Useful for debugging.
     //e_vcard_dump_structure((EVCard*)contact);
   }
   
   g_list_free(contactList);
}

QList<QContactLocalId> QContactABook::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
  QList<QContactLocalId> rtn;
  
  EBookQuery* query = convert(filter);
  
  GList* l = osso_abook_aggregator_find_contacts(m_abookAgregator, query);
  e_book_query_unref(query);
  
  /* Sorting */
  switch (sortOrders.count()){
    case 0:
      QCM5_DEBUG << "No sorting"; break;
#if 0 
    //TODO Native sorting needs a creation of copare function for each couple of detailDefinitionName/detailFieldName
    case 1: {
      QContactSortOrder so = sortOrders[0];
      QCM5_DEBUG << "Native sorting" << so.detailDefinitionName() << so.detailFieldName();
      //l = g_list_sort(l, osso_abook_contact_uid_compare);
    } break;
#endif 
    default: // > 1
      QCM5_DEBUG << "Multi filtering"; break;
      
      // Fill Ids
      QList<QContactLocalId> Ids;
      {
        QContactFilter f;
        QList<QContactSortOrder> so;
        QContactManager::Error e;
        Ids = contactIds(f, so, e);
      }
      
      // Fill Contact List
      QList<QContact> contacts;
      foreach(QContactLocalId id, Ids){
	QContact *c;
	QContactManager::Error e;
	c = contact(id, e);
	if (e == QContactManager::NoError)
	  contacts << *c;
      }
      
      // Non native sorting
      return QContactManagerEngine::sortContacts(contacts, sortOrders);
  }
  
  while (l){
    EContact *contact = E_CONTACT(l->data);
    const char* data = CONST_CHAR(e_contact_get_const(contact, E_CONTACT_UID));
    QByteArray localId(data);
    m_localIds << localId;
    rtn.append(m_localIds[localId]);
    QCM5_DEBUG << "eContactID " << localId << "has been stored in m_localIDs with key" << m_localIds[localId];
    l = l->next;
  }
  g_list_free(l);
  
  error = QContactManager::NoError;
  return rtn;
}

QContact* QContactABook::contact(const QContactLocalId& contactId, QContactManager::Error& error) const
{
  QContact *contact;
  EContact *eContact = NULL; 
  GList* contacts = NULL;
  EBookQuery* query;
  
  query = e_book_query_field_test(E_CONTACT_UID, E_BOOK_QUERY_IS, m_localIds[contactId]);
  contacts = osso_abook_aggregator_find_contacts(m_abookAgregator, query);
  e_book_query_unref(query);
  
  QCM5_DEBUG << "Getting eContact with id " << m_localIds[contactId] << "local contactId is" << contactId;
 
  eContact = E_CONTACT(contacts->data);

  if (g_list_length(contacts) != 1) {
    qWarning("List is empty or several contacts have the same UID");
    error = QContactManager::DoesNotExistError;
    return new QContact;
  }
  
  g_list_free(contacts);

  // Convert eContact to QContact
  contact = convert(eContact);
  error = QContactManager::NoError;
  return contact;
}

bool QContactABook::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
  Q_UNUSED(error);
  
  bool ok = false;
  OssoABookRoster* roster = reinterpret_cast<OssoABookRoster*>(m_abookAgregator);
  EBook *book = osso_abook_roster_get_book(roster);
  const char *id = m_localIds[contactId];
  GError *gError = NULL;
  
  QCM5_DEBUG << "Deleting contact id:" << id;
  
  ok = e_book_remove_contact(book, id, &gError);
  WARNING_IF_ERROR(gError);
  
  return ok;
}

EBookQuery* QContactABook::convert(const QContactFilter& filter) const
{
  EBookQuery* query = NULL;
  
  switch(filter.type()){
      case QContactFilter::DefaultFilter:
	{
	  QCM5_DEBUG << "QContactFilter::DefaultFilter";
	  query = e_book_query_any_field_contains(""); //Match all contacts
	}
	break;
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
        }
        break;
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
	}
        break;
      case QContactFilter::ContactDetailRangeFilter:
	{
	  //Current version of ebook doesn't support LT/LE/GT/GL Query tests
	  qWarning() << "ContactDetailRangeFilter is not supported";
	  return NULL;
	}
        break;
      case QContactFilter::ChangeLogFilter:
	QCM5_DEBUG << "ChangeLogFilter"; //TODO 
	break;
      case QContactFilter::ActionFilter:
	QCM5_DEBUG << "ActionFilter"; //TODO
	break;
      case QContactFilter::RelationshipFilter:
	QCM5_DEBUG << "RelationshipFilter"; //TODO
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
	}
	break;
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
        }
	break;
      case QContactFilter::InvalidFilter:
	QCM5_DEBUG << "InvalidFilter";
	query = e_book_query_from_string("(is \"id\" \"-1\")");
	break;
  }
 
  //Debugging
  const char *queryString = e_book_query_to_string(query);
  QCM5_DEBUG << "QUERY" << queryString;
  FREE(queryString);
  
  return query;
} 

QContact* QContactABook::convert(EContact *eContact) const
{  
/*  
  QContactFamily
  QContactGeolocation
  QContactSyncTarget
  QContactTimestamp
*/

  QContact *contact = new QContact();
  QList<QContactDetail*> detailList;
  
  /* Id */
  contact->setId(createContactId(eContact));
  
  /* DisplayLabel */
  //detailList <<  createDisplayLabelDetail(eContact);
  
  /* Address */
  QList<QContactAddress*> addressList = createAddressDetail(eContact);
  QContactAddress* address;
  foreach(address, addressList)
    detailList << address;
  
  /* Anniversary -NOT SUPPORTED IN MAEMO5- */
  
  /* Avatar */
  detailList << createAvatarDetail(eContact);

  /* BirthDay */
  detailList << createBirthdayDetail(eContact);
  
  /* Email */
  QList<QContactEmailAddress*> emailList = createEmailDetail(eContact);
  QContactEmailAddress* email;
  foreach(email, emailList)
    detailList << email;
  
  /* Family -NOT SUPPORTED IN MAEMO5- */
  
  /* Gender */
  detailList << createGenderDetail(eContact);
  
  /* Geo -NOT SUPPORTED IN MAEMO5- */
 
  /* Global UID*/
  detailList << createGuidDetail(eContact);
  
  /* Name & NickName*/
  detailList << createNameDetail(eContact);
  detailList << createNicknameDetail(eContact);

  /* Note */
  detailList << createNoteDetail(eContact);
  
  /* Online Account */
  
  QList<QContactOnlineAccount*> onlineAccountList;// = createOnlineAccountDetail(eContact);
  QContactOnlineAccount* onlineAccount;
  foreach(onlineAccount, onlineAccountList)
    detailList << onlineAccount;
  
  /* Organization */
  detailList << createOrganizationDetail(eContact);
  
  /* Phone*/
  QList<QContactPhoneNumber*> phoneNumberList = createPhoneDetail(eContact);
  QContactPhoneNumber* phoneNumber;
  foreach(phoneNumber, phoneNumberList)
    detailList << phoneNumber;
  
  /* TimeStamp */
  detailList << createTimestampDetail(eContact);

  /* Url */
  detailList << createUrlDetail(eContact);
  
  bool ok;
  QContactDetail* detail;
 
  foreach(detail, detailList){
    if (detail->isEmpty()){
      delete detail;
      continue;
    }
    
    ok = contact->saveDetail(detail);
    if (!ok){
      qWarning() << "Detail can't be saved into QContact" << detail->values(); //WARNING values is deprecated
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

QContactId QContactABook::createContactId(EContact *eContact) const
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
#if 0  //Book uri looks empty all the time
  /* Set URI */
  {
    const char* data = CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_BOOK_URI));
    if (data)
      rtn->setManagerUri(data);
    else
      qWarning("eContact E_CONTACT_BOOK_URI is an empty string");
  }
#endif
  return rtn;
}

//FIXME Maemo5 contacts can contain more than one address with the same context
QList<QContactAddress*> QContactABook::createAddressDetail(EContact *eContact) const
{
  QList<QContactAddress*> rtnList;

  //Ordered list of Fields
  QStringList addressFields;
  addressFields << QContactAddress::FieldPostOfficeBox
                << "Estension"
                << QContactAddress::FieldStreet
                << QContactAddress::FieldLocality
                << QContactAddress::FieldRegion 
                << QContactAddress::FieldPostcode 
                << QContactAddress::FieldCountry;
  
  GList* attrList = osso_abook_contact_get_attributes(eContact, EVC_ADR);
    
  while(attrList){
    QContactAddress *address = new QContactAddress;
    QVariantMap map;
  
    EVCardAttribute *attr = static_cast<EVCardAttribute*>(attrList->data);

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
    GList *v = e_vcard_attribute_get_values(attr);
    int i = 0;
    while (v){
      map[addressFields[i]] = QString::fromLatin1(CONST_CHAR(v->data));
      i++;
      v = v->next;
    }
    g_list_free(v);
    
    
    setDetailValues(map, address);
    
    rtnList << address;
    
    attrList = attrList->next;
  }
  
  g_list_free(attrList);
  
  return rtnList;
}

QContactName* QContactABook::createNameDetail(EContact *eContact) const
{
  QContactName* rtn = new QContactName;
  QVariantMap map;

  //Try to get the structure (looks that this is not supported in Maemo5)
  EContactName* eContactName = static_cast<EContactName*> (e_contact_get(eContact, E_CONTACT_NAME));
  if (eContactName){
    map[QContactName::FieldCustomLabel] = eContactName->additional;
    map[QContactName::FieldFirst] = eContactName->given;
    map[QContactName::FieldLast] = eContactName->family;
    //map[QContactName::FieldMiddle] = eContactName->
    map[QContactName::FieldPrefix] = eContactName->prefixes;
    map[QContactName::FieldSuffix] = eContactName->suffixes;
    e_contact_name_free (eContactName);
  } else {
    //Looks that Maemo use just these two fields
    map[QContactName::FieldFirst] = CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_GIVEN_NAME));
    map[QContactName::FieldLast] = CONST_CHAR(e_contact_get_const(eContact, E_CONTACT_FAMILY_NAME));  
  }
  setDetailValues(map, rtn);
  return rtn;
}

QContactNickname* QContactABook::createNicknameDetail(EContact *eContact) const
{
  QContactNickname* rtn = new QContactNickname;
  QVariantMap map;
  map[QContactNickname::FieldNickname] = CONST_CHAR (e_contact_get_const(eContact, E_CONTACT_NICKNAME));
  setDetailValues(map, rtn);
  return rtn;
}

//FIXME Maemo5 eContacts can contain several email account for the same context
QList<QContactEmailAddress*> QContactABook::createEmailDetail(EContact *eContact) const
{
  QList<QContactEmailAddress*> rtnList;
  
  GList* attrList = osso_abook_contact_get_attributes(eContact, EVC_EMAIL);
  while(attrList){
    QContactEmailAddress *email = new QContactEmailAddress;
    QVariantMap map;
  
    EVCardAttribute *attr = static_cast<EVCardAttribute*>(attrList->data);

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
    
    setDetailValues(map, email);
    rtnList << email;
    attrList = attrList->next;
  }
  g_list_free(attrList);
  
  return rtnList;
}

QContactAvatar* QContactABook::createAvatarDetail(EContact *eContact) const
{  
  QContactAvatar* rtn = new QContactAvatar;
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
  map[QContactAvatar::FieldAvatarPixmap] = QPixmap::fromImage(converted);
  g_object_unref(pixbuf);
  setDetailValues(map, rtn);
  
  return rtn;
}

QContactBirthday* QContactABook::createBirthdayDetail(EContact *eContact) const
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

QContactGender* QContactABook::createGenderDetail(EContact *eContact) const
{
  QContactGender* rtn = new QContactGender;
  QVariantMap map;
  const char* gender = CONST_CHAR(osso_abook_contact_get_value(eContact, "X-GENDER"));
  map[QContactGender::FieldGender] = gender;
  FREE(gender);
  setDetailValues(map, rtn);
  return rtn;
}  

//NOTE Using UID as GUID
QContactGuid* QContactABook::createGuidDetail(EContact *eContact) const
{
  QContactGuid* rtn = new QContactGuid;
  QVariantMap map;
  const char* uid = CONST_CHAR(e_contact_get(eContact, E_CONTACT_UID));
  map[QContactGuid::FieldGuid] = uid;
  FREE(uid);
  setDetailValues(map, rtn);
  return rtn;
}

QContactNote* QContactABook::createNoteDetail(EContact *eContact) const
{
  QContactNote* rtn = new QContactNote;
  QVariantMap map;
  const char* note = CONST_CHAR(e_contact_get(eContact, E_CONTACT_NOTE));
  map[QContactNote::FieldNote] = QString::fromLatin1(note);
  FREE(note);
  setDetailValues(map, rtn);
  return rtn;
}

//FIXME Review required. 
QList<QContactOnlineAccount*> QContactABook::createOnlineAccountDetail(EContact *eContact) const
{
  QList<QContactOnlineAccount*> rtnList;
  
  QStringList evcardToSkip; 
  
  // Gets info of online accounts from roster contacts associated to the master one  
  if (!osso_abook_contact_is_roster_contact (A_CONTACT(eContact))) {
    QContactOnlineAccount* rtn = new QContactOnlineAccount;
    
    GList *contacts = osso_abook_contact_get_roster_contacts (A_CONTACT(eContact));
    GList *node;
    for (node = contacts; node != NULL; node = g_list_next (node)) {
      OssoABookContact *rosterContact = A_CONTACT(node->data);
      McProfile* id = static_cast<McProfile*>(osso_abook_contact_get_profile(rosterContact));
      OssoABookPresence *presence = OSSO_ABOOK_PRESENCE (A_CONTACT(eContact));
      TpConnectionPresenceType presenceType = osso_abook_presence_get_presence_type (presence);
      
      QString presenceTypeString;
      switch (presenceType) {
	case TP_CONNECTION_PRESENCE_TYPE_UNSET: presenceTypeString = "unset"; break;
	case TP_CONNECTION_PRESENCE_TYPE_OFFLINE: presenceTypeString = "Offline"; break;
	case TP_CONNECTION_PRESENCE_TYPE_AVAILABLE: presenceTypeString = "Available"; break;
	case TP_CONNECTION_PRESENCE_TYPE_AWAY: presenceTypeString = "Away"; break;
	case TP_CONNECTION_PRESENCE_TYPE_EXTENDED_AWAY: presenceTypeString = "Extended Away"; break;
	case TP_CONNECTION_PRESENCE_TYPE_HIDDEN: presenceTypeString = "Hidden"; break;
	case TP_CONNECTION_PRESENCE_TYPE_BUSY: presenceTypeString = "Busy"; break;
	case TP_CONNECTION_PRESENCE_TYPE_UNKNOWN: presenceTypeString = "Unknown"; break;
	case TP_CONNECTION_PRESENCE_TYPE_ERROR: presenceTypeString = "Error"; break;
	default:
	  qCritical() << "Presence type is not vaild" << presenceType;
      }
      
      QString accountVCard = QString::fromLatin1(mc_profile_get_vcard_field(id));
      if (!evcardToSkip.contains(accountVCard))
	evcardToSkip << accountVCard;
	
      QVariantMap map;
      //map[QContactOnlineAccount::FieldAccountUri] = //I'M NOT ABLE TO GET this yet
      map[QContactOnlineAccount::FieldNickname] = osso_abook_contact_get_display_name(A_CONTACT(eContact));
      map[QContactOnlineAccount::FieldPresence] = presenceTypeString;
      map[QContactOnlineAccount::FieldServiceProvider] = mc_profile_get_unique_name(id);
      map[QContactOnlineAccount::FieldStatusMessage] = QString::fromLatin1(osso_abook_presence_get_presence_status_message (presence));
      //map[QContactOnlineAccount::FieldSubTypes] =
      
      setDetailValues(map, rtn);
      rtnList << rtn;
    }
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
      if (evcardToSkip.contains(attributeName))
	continue;
      
      // We are looking for attributes defined by Maemo guys and that star with "X-" (eg: X-JABBER, X-MSN and so on)
      if (!attributeName.contains(QRegExp("^X-")))
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
          attrIsType = paramName.contains("TYPE");
	
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
	//DON'T FREE ANYTHING!! e_vcard_attribute_param_free(p);

	if (ossoValidIsOk && !type.isEmpty()) {
	  QContactOnlineAccount* rtn = new QContactOnlineAccount;
          QVariantMap map;
	  //map[QContactOnlineAccount::FieldAccountUri] = 
	  map[QContactOnlineAccount::FieldNickname] = QString::fromLatin1(e_vcard_attribute_get_value(attr));
	  //map[QContactOnlineAccount::FieldPresence] = 
	  map[QContactOnlineAccount::FieldServiceProvider] = type;
	  //map[QContactOnlineAccount::FieldStatusMessage] =
	  //map[QContactOnlineAccount::FieldSubTypes] =
          setDetailValues(map, rtn);
	  rtnList << rtn;
	}
      }
      //DON'T FREE ANYTHING!! e_vcard_attribute_free(attr);
    }
  }
  
  return rtnList;
}

QContactOrganization* QContactABook::createOrganizationDetail(EContact *eContact) const
{
  QContactOrganization* rtn = new QContactOrganization;
  QVariantMap map;
  const char* title = CONST_CHAR(e_contact_get(eContact, E_CONTACT_TITLE));
  map[QContactOrganization::FieldTitle] = title;
  FREE(title);
  setDetailValues(map, rtn);
  return rtn;
}

QList<QContactPhoneNumber*> QContactABook::createPhoneDetail(EContact *eContact) const
{
  QList<QContactPhoneNumber*> rtnList;
  
  GList *l = osso_abook_contact_get_attributes(eContact, EVC_TEL);
  
  while (l) {
    QContactPhoneNumber* phoneNumber = new QContactPhoneNumber;
    
    EVCardAttribute *attr = static_cast<EVCardAttribute*>(l->data);
    GList* p = e_vcard_attribute_get_param(attr, "TYPE");
    
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
    phoneNumber->setNumber(phoneNumberStr);
    
    l = l->next;
    
    rtnList << phoneNumber;
  }
  g_list_free(l);
  
  return rtnList;
}

QContactTimestamp* QContactABook::createTimestampDetail(EContact *eContact) const
{
   QContactTimestamp* rtn = new QContactTimestamp;
   QVariantMap map;
   const char* rev = CONST_CHAR(e_contact_get(eContact, E_CONTACT_REV));
   map[QContactTimestamp::FieldModificationTimestamp] = QDateTime::fromString(rev, Qt::ISODate);
   FREE(rev);
   setDetailValues(map, rtn);
   return rtn;
}

QContactUrl* QContactABook::createUrlDetail(EContact *eContact) const
{
   QContactUrl* rtn = new QContactUrl;
   QVariantMap map;
   const char* url = CONST_CHAR(e_contact_get(eContact, E_CONTACT_HOMEPAGE_URL));
   map[QContactUrl::FieldUrl] = url;
   FREE(url);
   setDetailValues(map, rtn);
   return rtn;
}
