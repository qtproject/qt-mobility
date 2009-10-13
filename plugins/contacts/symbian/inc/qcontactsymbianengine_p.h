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
//typedef QList<QContactLocalId> QContactLocalIdIterator;

class QContactChangeSet;

class QContactSymbianEngineData : public QObject,
							   public MContactDbObserver
{
	Q_OBJECT
	
public:
    QContactSymbianEngineData();
    virtual ~QContactSymbianEngineData();

public:
    /* Access */
        QList<QContactLocalId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error);
    QContact contact(const QContactLocalId& contactId, QContactManager::Error& qtError) const;
    QList<QContactLocalId> contacts(QContactManager::Error& qtError) const;
    int count() const;
    
    /* "My card" */
    QContactLocalId personalID() const;
    bool hasPersonalDetails() const; //is this needed, QContactLocalId could be 0 if it doesn't exist
    int setPersonalContactId(const QContactLocalId &);
    void clearPersonalContactId();

    /* Add/Update/Remove */ 
    bool addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    
    //function to find contacts, based on the communication method
    //communication type - call (number, voip fields), digits to match
    //communication type - sms (number fields), digits to match
    //communication type - email (email fields)
    QList<QContactLocalId> matchCommunicationAddress( const QString &communicationType, const QString &communicationAddress );

    /* Groups functionality */
        QList<QContactLocalId> groups(QContactManager::Error& qtError) const;
        QContactGroup group(const QContactLocalId& groupId, QContactManager::Error& qtError) const;
	bool addGroup(QContactGroup& group, QContactChangeSet& changeSet, QContactManager::Error& qtError);
	bool updateGroup(QContactGroup& group, QContactChangeSet& changeSet, QContactManager::Error& qtError);
        bool removeGroup(const QContactLocalId& groupId, QContactChangeSet& changeSet, QContactManager::Error& qtError);
        QContactLocalId simPhonebookGroupId() const;
    
public:
	// From MContactDbObserver
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
    
signals:
    // Database change notifications
    void contactAdded(const QContactLocalId &contactId);
    void contactRemoved(const QContactLocalId &contactId);
    void contactChanged(const QContactLocalId &contactId);
    void groupAdded(const QContactLocalId &contactId);
    void groupRemoved(const QContactLocalId &contactId);
    void groupChanged(const QContactLocalId &contactId);

private:
    // Utility function to convert symbian error to QContactManager error
	void transformError(TInt symbianError, QContactManager::Error& qtError) const;

	// Leaving functions implementing CNTMODEL interaction.
        QContact contactL(const QContactLocalId &id) const;
        QList<QContactLocalId> contactsL() const;
	int countL() const;

	int addContactL(QContact &contact);
	void updateContactL(QContact &contact);
    int removeContactL(QContactLocalId id);

        QList<QContactLocalId>  matchCommunicationAddressL( const QString &communicationType, const QString &communicationAddress );

	//Leaving functions for groups functionality
        QList<QContactLocalId> groupsL() const;
        QContactGroup groupL(const QContactLocalId& groupId) const;
	void addGroupL(QContactGroup& group);
	void updateGroupL(QContactGroup& group);
        void removeGroupL(const QContactLocalId& groupId);
        QContactLocalId simPhonebookGroupIdL() const;
        QList<QContactLocalId> memberOfGroupsL(const TContactItemId contactId) const;
	void updateMemberOfGroupsL(QContact contact);

	//utility functions
        CContactGroup* fetchCGroup(const QContactLocalId& id) const;
        bool isGroup(const QContactLocalId& id) const;

private:
	CContactDatabase* m_contactDatabase;
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__  
	CContactChangeNotifier* m_contactChangeNotifier;
#endif	
	TransformContact* m_transformContact;
    QList<QContactLocalId> m_contactsAddedEmitted;
    QList<QContactLocalId> m_contactsChangedEmitted;
    QList<QContactLocalId> m_contactsRemovedEmitted;
};

#endif
