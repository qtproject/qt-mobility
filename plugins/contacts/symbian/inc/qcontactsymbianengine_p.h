/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONTACTSYMBIANENGINEDATA_H
#define QCONTACTSYMBIANENGINEDATA_H

#include <QObject>
#include <qtcontacts.h>

#include <cntdb.h>
#include <cntdbobs.h>
#include <cntitem.h>

#include "qtcontactsglobal.h"


#include "transformcontact.h"

//remember to remove this in the code and just return a normal list
//typedef QList<QUniqueId> QUniqueIdIterator;


class QContactSymbianEngineData : public QObject,
							   public MContactDbObserver
{
	Q_OBJECT
	
public:
    QContactSymbianEngineData();
    virtual ~QContactSymbianEngineData();

public:
    /* Access */
	QList<QUniqueId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error);
    QContact contact(const QUniqueId& contactId, QContactManager::Error& qtError) const;
    QList<QUniqueId> contacts(QContactManager::Error& qtError) const;
    int count() const;
    
    /* "My card" */
    QUniqueId personalID() const;
    bool hasPersonalDetails() const; //is this needed, QUniqueId could be 0 if it doesn't exist
    int setPersonalContactId(const QUniqueId &);
    void clearPersonalContactId();

    /* Add/Update/Remove */ 
    bool addContact(QContact& contact, int &id, QContactManager::Error& qtError);
    bool updateContact(QContact& contact, QContactManager::Error& qtError);
    bool removeContact(const QUniqueId &id, QContactManager::Error& qtError);
    bool removeContacts(const QList<QUniqueId> &, QContactManager::Error& qtError);
    
    //function to find contacts, based on the communication method
    //communication type - call (number, voip fields), digits to match
    //communication type - sms (number fields), digits to match
    //communication type - email (email fields)
    QList<QUniqueId> matchCommunicationAddress( const QString &communicationType, const QString &communicationAddress );

    /* Groups functionality */
	QList<QUniqueId> groups(QContactManager::Error& qtError) const;
	QContactGroup group(const QUniqueId& groupId, QContactManager::Error& qtError) const;
	bool saveGroup(QContactGroup& group, QContactManager::Error& qtError);
	bool removeGroup(const QUniqueId& groupId, QContactManager::Error& qtError);
	QUniqueId simPhonebookGroupId() const;
    
public:
	// From MContactDbObserver
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
    
signals:
		// Database change notifications
		void contactAdded(const QUniqueId &contactId);
    void contactRemoved(const QUniqueId &contactId);
    void contactChanged(const QUniqueId &contactId);
    void groupAdded(const QUniqueId &contactId);
    void groupRemoved(const QUniqueId &contactId);
    void groupChanged(const QUniqueId &contactId);

private:
	// Utility functions to convert to and from CContactItem
	void transformError(TInt symbianError, QContactManager::Error& qtError) const;
		
	// Leaving functions implementing CNTMODEL interaction.
	QContact contactL(const QUniqueId &id) const;
	QList<QUniqueId> contactsL() const;
	int countL() const;
	
	int addContactL(QContact &contact);
	void updateContactL(QContact &contact);
    int removeContactL(QUniqueId id);
    int removeContactsL(const QList<QUniqueId> &ids);
	
	QList<QUniqueId>  matchCommunicationAddressL( const QString &communicationType, const QString &communicationAddress );

	//Leaving functions for groups functionality
	QList<QUniqueId> groupsL() const;
	QContactGroup groupL(const QUniqueId& groupId) const;
	void saveGroupL(QContactGroup& group);
	void removeGroupL(const QUniqueId& groupId);
	QUniqueId simPhonebookGroupIdL() const;

	//utility functions
	CContactGroup* fetchCGroup(const QUniqueId& id) const;
	bool isGroup(const QUniqueId& id) const;
	
	
private:
	CContactDatabase* m_contactDatabase;
	TransformContact* m_transformContact;
};

#endif
