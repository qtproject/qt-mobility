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
#include "qcontactmanager.h"

#include "transformcontact.h"
#include "qabstractcontactfilter.h"

//remember to remove this in the code and just return a normal list
//typedef QList<QUniqueId> QUniqueIdIterator;

class QContactChangeSet;
class QAbstractContactFilter;
class QAbstractContactSorter;

class QContactSymbianEngineData : public QObject,
							   public MContactDbObserver
{
	Q_OBJECT

public:
    QContactSymbianEngineData(QContactManager::Error& error);
    virtual ~QContactSymbianEngineData();

public:
    /* Access */
	QList<QUniqueId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
	QAbstractContactFilter::FilterSupport filterSupported(const QContactFilter& filter) const;
	bool sortOrderSupported(const QList<QContactSortOrder>& sortOrders) const;
    QContact contact(const QUniqueId& contactId, QContactManager::Error& qtError) const;
    QList<QUniqueId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& qtError) const;
    int count() const;

    /* "My card" */
    QUniqueId personalID() const;
    bool hasPersonalDetails() const; //is this needed, QUniqueId could be 0 if it doesn't exist
    int setPersonalContactId(const QUniqueId &);
    void clearPersonalContactId();

    /* Add/Update/Remove */
    bool addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool removeContact(const QUniqueId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError);

    /* Groups functionality */
	QList<QUniqueId> groups(QContactManager::Error& qtError) const;
	QContactGroup group(const QUniqueId& groupId, QContactManager::Error& qtError) const;
	bool addGroup(QContactGroup& group, QContactChangeSet& changeSet, QContactManager::Error& qtError);
	bool updateGroup(QContactGroup& group, QContactChangeSet& changeSet, QContactManager::Error& qtError);
	bool removeGroup(const QUniqueId& groupId, QContactChangeSet& changeSet, QContactManager::Error& qtError);
	QUniqueId simPhonebookGroupId() const;

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QUniqueId& contactId, QContactManager::Error& qtError);
    QUniqueId selfContactId(QContactManager::Error& qtError) const;

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
    
public:
    // Utility function to convert symbian error to QContactManager error
    static void transformError(TInt symbianError, QContactManager::Error& qtError);

private:

	// Leaving functions implementing CNTMODEL interaction.
	QContact contactL(const QUniqueId &id) const;
	int countL() const;

	int addContactL(QContact &contact);
	void updateContactL(QContact &contact);
    int removeContactL(QUniqueId id);

	//Leaving functions for groups functionality
	QList<QUniqueId> groupsL() const;
	QContactGroup groupL(const QUniqueId& groupId) const;
	void addGroupL(QContactGroup& group);
	void updateGroupL(QContactGroup& group);
	void removeGroupL(const QUniqueId& groupId);
	QUniqueId simPhonebookGroupIdL() const;
	QList<QUniqueId> memberOfGroupsL(const TContactItemId contactId) const;
	void updateMemberOfGroupsL(QContact contact);

	//utility functions
	CContactGroup* fetchCGroup(const QUniqueId& id) const;
	bool isGroup(const QUniqueId& id) const;

private:
	CContactDatabase* m_contactDatabase;
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	CContactChangeNotifier* m_contactChangeNotifier;
#endif
	TransformContact* m_transformContact;
    QList<QUniqueId> m_contactsAddedEmitted;
    QList<QUniqueId> m_contactsChangedEmitted;
    QList<QUniqueId> m_contactsRemovedEmitted;
    QAbstractContactFilter* m_contactFilter;
    QAbstractContactSorter* m_contactSorter;
};

#endif
