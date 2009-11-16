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

#ifndef CNTSYMBIANENGINEPRIVATE_H
#define CNTSYMBIANENGINEPRIVATE_H

#include <QObject>
#include <QSet>

#include <qtcontacts.h>

#include <cntdbobs.h>
#include <cntitem.h>

#include "qtcontactsglobal.h"
#include "qcontactmanager.h"

#include "cnttransformcontact.h"
#include "cntabstractcontactfilter.h"

#define CNT_SYMBIAN_MANAGER_NAME "symbian"

class QContactChangeSet;
class CntAbstractContactFilter;
class CntAbstractContactSorter;
class CntRelationship;
class CntSymbianDatabase;

class CntSymbianEnginePrivate : public QObject
{
	Q_OBJECT

public:
    CntSymbianEnginePrivate(CntSymbianDatabase *database, const QMap<QString, QString>& parameters, QContactManager::Error& error);
    virtual ~CntSymbianEnginePrivate();

public:
    /* Access */
    //Contacts
    QList<QContactLocalId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QList<QContactLocalId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& qtError) const;
    QContact contact(const QContactLocalId& contactId, QContactManager::Error& qtError) const;
    QString managerUri(){ return m_managerUri; }

    //Groups
    QList<QContactLocalId> groups(QContactManager::Error& qtError) const;

    //Filter & sort order
    CntAbstractContactFilter::FilterSupport filterSupported(const QContactFilter& filter) const;
	bool sortOrderSupported(const QList<QContactSortOrder>& sortOrders) const;
    QList<QContactLocalId> sort(
        QList<QContactLocalId> contactIds,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error);
    int count() const;

    /* Add/Update/Remove */
    bool addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    bool removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError);

    QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const;
    bool saveRelationship(QContactChangeSet *changeSet, QContactRelationship* relationship, QContactManager::Error& error);
    QList<QContactManager::Error> saveRelationships(QContactChangeSet *changeSet, QList<QContactRelationship>* relationships, QContactManager::Error& error);
    bool removeRelationship(QContactChangeSet *changeSet, const QContactRelationship& relationship, QContactManager::Error& error);
    QList<QContactManager::Error> removeRelationships(QContactChangeSet *changeSet, const QList<QContactRelationship>& relationships, QContactManager::Error& error);



    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& qtError);
    QContactLocalId selfContactId(QContactManager::Error& qtError) const;

public:
	// From MContactDbObserver
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

private:

	// Leaving functions implementing CNTMODEL interaction.
    QContact contactL(const QContactLocalId &localId) const;
    QList<QContactLocalId> groupsL() const;

    int countL() const;

	int addContactL(QContact &contact);
	void updateContactL(QContact &contact);
    int removeContactL(QContactLocalId id);

private:
    CntSymbianDatabase* m_dataBase;
    QString m_managerUri;

    CntTransformContact       *m_transformContact;
    CntAbstractContactFilter* m_contactFilter;
    CntAbstractContactSorter* m_contactSorter;
    CntRelationship        *m_relationship;
};

#endif
