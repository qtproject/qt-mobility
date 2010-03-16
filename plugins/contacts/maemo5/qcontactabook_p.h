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

#ifndef QCONTACTABOOK_P_H
#define QCONTACTABOOK_P_H

#include <QObject>
#include <QMutex>

#include "qtcontacts.h"
#include "qcontactmaemo5debug_p.h"

#include <libosso-abook/osso-abook-init.h>
#include <libosso-abook/osso-abook-types.h>
#include <libosso-abook/osso-abook-waitable.h>
#include <libosso-abook/osso-abook-presence.h>
#include <libosso-abook/osso-abook-avatar.h>
//#include <libosso-abook/osso-abook-account-manager.h>

#include <libmcclient/mc-profile.h>
#include <libmcclient/mc-account.h>

#include <gdk-pixbuf/gdk-pixbuf.h>


//### Remove in PR 1.2
extern "C" {
        typedef void GtkWindow;
	struct _OssoABookContact {
		EContact parent;
	};
	
	OssoABookRoster*    osso_abook_roster_new               (const char *name,
                                                                 EBookView *book_view,
                                                                 const char *vcard_field);
	gboolean            osso_abook_roster_is_running        (OssoABookRoster *roster);
	void                osso_abook_roster_start             (OssoABookRoster *roster);

	gboolean            osso_abook_contact_has_valid_name   (OssoABookContact *contact);
	gboolean            osso_abook_contact_is_roster_contact(OssoABookContact *contact);

	OssoABookRoster*    osso_abook_aggregator_get_default   (GError **error);
	GList*              osso_abook_aggregator_list_master_contacts
                                                                (OssoABookAggregator *aggregator);
	unsigned            osso_abook_aggregator_get_master_contact_count
                                                                (OssoABookAggregator *aggregator);
	GList*              osso_abook_aggregator_lookup        (OssoABookAggregator *aggregator,
                                                                 const char *uid);
	const char*         osso_abook_contact_get_uid          (OssoABookContact *contact); 
	EBook*              osso_abook_roster_get_book          (OssoABookRoster *roster);
	char*               osso_abook_contact_to_string        (OssoABookContact *contact,
                                                                 EVCardFormat format,
                                                                 gboolean inline_avatar);
	char*               osso_abook_contact_get_value        (EContact *contact,
                                                                 const char *attr_name);
        GList*              osso_abook_aggregator_find_contacts (OssoABookAggregator *aggregator,
                                                                 EBookQuery *query);
        GList*              osso_abook_contact_get_values       (EContact *contact,
                                                                 const char *attr_name);
        GList*              osso_abook_contact_get_attributes   (EContact *contact,
                                                                 const char *attr_name);
	GList*              osso_abook_contact_get_roster_contacts
                                                                (OssoABookContact *master_contact);
	McProfile*          osso_abook_contact_get_profile      (OssoABookContact *contact);
	McAccount*          osso_abook_contact_get_account      (OssoABookContact *contact);
	gboolean            osso_abook_contact_delete           (OssoABookContact *contact,
                                                                 EBook *book,
                                                                 GtkWindow *window);
	GList*              osso_abook_aggregator_find_contacts (OssoABookAggregator *aggregator,
                                                                 EBookQuery *query);
	const char*         osso_abook_contact_get_display_name (OssoABookContact *contact);
	GdkPixbuf*          osso_abook_avatar_get_image_rounded (OssoABookAvatar *avatar,
                                                                 int width,
                                                                 int height,
                                                                 gboolean crop,
                                                                 int radius,
                                                                 const guint8 border_color[4]);
	OssoABookContact*   osso_abook_contact_new              (void);
	guint               osso_abook_contact_async_add        (OssoABookContact *contact,
                                                                 EBook *book,
                                                                 EBookIdCallback callback,
                                                                 gpointer user_data);
	guint               osso_abook_contact_async_commit     (OssoABookContact *contact,
                                                                 EBook *book,
                                                                 EBookCallback callback,
                                                                 gpointer user_data);
	gboolean            osso_abook_contact_add_value        (EContact *contact,
                                                                 const char *attr_name,
                                                                 GCompareFunc value_check,
                                                                 const char *value);
	void                osso_abook_contact_set_pixbuf       (OssoABookContact *contact,
                                                                 GdkPixbuf *pixbuf,
                                                                 EBook *book,
                                                                 GtkWindow *window);
	// osso-abook-account-manager.h
	const GList*        osso_abook_account_manager_get_primary_vcard_fields
                                                                 (OssoABookAccountManager *manager);
	OssoABookAccountManager* osso_abook_account_manager_get_default(void);
	const char*         osso_abook_account_manager_get_vcard_field
                                                                (OssoABookAccountManager *manager,
                                                                 const char *account_name);
}

QTM_USE_NAMESPACE

/* QContactIDsHash stores abookContact IDs (strings)*/
class QContactIDsHash{
public:
  QContactIDsHash(){};
  
  /* Append */
  QContactIDsHash& operator<< (const QByteArray& eContactId){ if (find(eContactId))
                                                                return (*this);
							      quint16 key = qChecksum(eContactId, eContactId.size());
                                                              m_localIds[key] = eContactId;
							      QCM5_DEBUG << "Add key:" << key << "eContactId:" << eContactId;
                                                        
							      return (*this);
                                                            };
  const QContactLocalId append(const QByteArray& eContactId){ uint id = find(eContactId);
                                                              if (id)
								return id;
							      id = qChecksum(eContactId, eContactId.size());
                                                              m_localIds[id] = eContactId;
							      QCM5_DEBUG << "Add key:" << id << "eContactId:" << eContactId;
							      return id;
                                                            };
  /* Find */
  const char* operator[] (const QContactLocalId localId) { return m_localIds.value(localId).constData(); };
  const char* find(const QContactLocalId localId) { return m_localIds.value(localId).constData(); };
  const QContactLocalId operator[] (const QByteArray& eContactId) { return m_localIds.key(eContactId, 0); };
  const QContactLocalId find(const QByteArray& eContactId) { return m_localIds.key(eContactId, 0); };
  
  /* Remove */ //TEST Remove functions not used nor tested yet
  bool remove(const QContactLocalId localId){ bool removed = (m_localIds.remove(localId) == 1) ? true : false;
                                              QCM5_DEBUG << "Remove QContactLocalId:" << localId << ((removed) ? "OK" : "NO");
                                              return removed;
                                            };
  bool remove(const QByteArray& eContactId){ const QContactLocalId hashKey = m_localIds.key(eContactId, 0);
                                             bool removed = remove(hashKey);
					     QCM5_DEBUG << "Remove QContactLocalId:" << hashKey << ((removed) ? "OK" : "NO");
                                             return removed;
                                           };
  
  /* Take */
  const QContactLocalId take(const QByteArray& eContactId){ const QContactLocalId hashKey = m_localIds.key(eContactId, 0);
                                                            remove(hashKey);
                                                            return hashKey; };
private:
  QHash<QContactLocalId, QByteArray> m_localIds; //[int/QContactLocalId Maemo5LocalId, QByteArray eContactID]
};

//Contains Data shared with contact changes/added/removed callbacks
struct cbSharedData;

class QContactABook : public QObject
{
  Q_OBJECT
  
public:  
  QContactABook(QObject* parent = 0);
  ~QContactABook();
  
  QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
  //QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
  QContact* getQContact(const QContactLocalId& contactId, QContactManager::Error& error) const;
  bool removeContact(const QContactLocalId& contactId, QContactManager::Error& error);
  bool saveContact(QContact* contact, QContactManager::Error& error);

Q_SIGNALS:
  void savingJobDone();

public:
  // Members used by callbacks
  void _contactsAdded(const QList<QContactLocalId> & contactIds ){ /*TODO*/ };
  void _contactsRemoved(const QList<QContactLocalId> & contactIds ){ /*TODO*/ };
  void _contactsChanged(const QList<QContactLocalId> & contactIds ){ /*TODO*/ };
  void _savingJobFinished(){ emit savingJobDone(); };
  
private:
  void initAddressBook();
  void initLocalIdHash();
  
  bool setDetailValues(const QVariantMap& data, QContactDetail* detail) const;
  
  OssoABookContact* getAContact(const QContactLocalId& contactId) const;
  
  /* Searching */
  EBookQuery* convert(const QContactFilter& filter) const;
  
  /* Reading - eContact/abookContact to QContact methods */
  QContact* convert(EContact *eContact) const;
  
  QContactId getContactId(EContact *eContact) const;
  QList<QContactAddress*> getAddressDetail(EContact *eContact) const;
  QContactName* getNameDetail(EContact *eContact) const;
  QContactNickname* getNicknameDetail(EContact *eContact) const;
  QList<QContactEmailAddress*> getEmailDetail(EContact *eContact) const;
  QContactAvatar* getAvatarDetail(EContact *eContact) const;
  QContactBirthday* getBirthdayDetail(EContact *eContact) const;
  QContactGender* getGenderDetail(EContact *eContact) const;
  QContactGuid* getGuidDetail(EContact *eContact) const;
  QContactNote* getNoteDetail(EContact *eContact) const;
  QList<QContactOnlineAccount*> getOnlineAccountDetail(EContact *eContact) const;
  QContactOrganization* getOrganizationDetail(EContact *eContact) const;
  QList<QContactPhoneNumber*> getPhoneDetail(EContact *eContact) const; 
  QContactTimestamp* getTimestampDetail(EContact *eContact) const; 
  QContactUrl* getUrlDetail(EContact *eContact) const;
  
  /* Saving - QContact to abookContact */
  OssoABookContact* convert(const QContact *contact) const;
  
  /* Save QDetails in OssoABookContact attributes */
  void setAddressDetail(const OssoABookContact* aContact, const QContactAddress& detail) const;
  void setAvatarDetail(const OssoABookContact* aContact, const QContactAvatar& detail) const;
  void setBirthdayDetail(const OssoABookContact* aContact, const QContactBirthday& detail) const;
  void setEmailDetail(const OssoABookContact* aContact, const QContactEmailAddress& detail) const;
  void setGenderDetail(const OssoABookContact* aContact, const QContactGender& detail) const;
  void setNameDetail(const OssoABookContact* aContact, const QContactName& detail) const;
  void setNicknameDetail(const OssoABookContact* aContact, const QContactNickname& detail) const;
  void setNoteDetail(const OssoABookContact* aContact, const QContactNote& detail) const;
  void setOnlineAccountDetail(const OssoABookContact* aContact, const QContactOnlineAccount& detail) const;
  void setOrganizationDetail(const OssoABookContact* aContact, const QContactOrganization& detail) const;
  void setPhoneDetail(const OssoABookContact* aContact, const QContactPhoneNumber& detail) const;
  void setUrlDetail(const OssoABookContact* aContact, const QContactUrl& detail) const;
  
  /* Internal Vars */
  OssoABookAggregator *m_abookAgregator;
  mutable QContactIDsHash m_localIds; //Converts QLocalId <=> eContactId
  QMutex m_saveContactMutex;
  cbSharedData *cbSD;
};

#endif
