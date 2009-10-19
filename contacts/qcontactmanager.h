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


#ifndef QCONTACTMANAGER_H
#define QCONTACTMANAGER_H

#include <QObject>

#include <QMap>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>

#include "qtcontactsglobal.h"
#include "qcontact.h"
#include "qcontactid.h"
#include "qcontactrelationship.h"
#include "qcontactsortorder.h"

class QContactFilter;
class QContactAction;

class QContactManagerData;
class QTCONTACTS_EXPORT QContactManager : public QObject
{
    Q_OBJECT

public:
#if Q_QDOC // qdoc's parser fails to recognise the default map argument
    QContactManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = 0, QObject* parent = 0);
#else
    QContactManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
#endif

    static QContactManager* fromUri(const QString& uri, QObject* parent = 0);
    ~QContactManager();                     // dtor

    QString managerName() const;                       // e.g. "Symbian"
    QMap<QString, QString> managerParameters() const;  // e.g. "filename=private.db"
    QString managerUri() const;                        // managerName + managerParameters

    static bool splitUri(const QString& uri, QString* managerName, QMap<QString, QString>* params);
    static QString buildUri(const QString& managerName, const QMap<QString, QString>& params);

    /* The values of the Error enum are still to be decided! */
    enum Error {
        NoError = 0,
        DoesNotExistError,
        AlreadyExistsError,
        InvalidDetailError,
        InvalidRelationshipError,
        LockedError,
        DetailAccessError,
        PermissionsError,
        OutOfMemoryError,
        NotSupportedError,
        BadArgumentError,
        UnspecifiedError
    };

    /* Error reporting */
    QContactManager::Error error() const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contacts(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const;    // retrieve contact ids
    QList<QContactLocalId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const; // retrieve ids of contacts matching the filter
    QList<QContactLocalId> contacts(const QString& contactType, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const; // retrieve contacts of the given type

    QContact contact(const QContactLocalId& contactId) const;  // retrieve a contact

    bool saveContact(QContact* contact);                 // note: MODIFIES contact (sets the contactId)
    bool removeContact(const QContactLocalId& contactId);      // remove the contact from the persistent store
    QList<QContactManager::Error> saveContacts(QList<QContact>* contacts);       // batch API - save
    QList<QContactManager::Error> removeContacts(QList<QContactLocalId>* contactIds);  // batch API - remove

    /* Synthesize the display label of a contact */
    QString synthesizeDisplayLabel(const QContact& contact) const;

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& contactId);
    QContactLocalId selfContactId() const;

    /* Relationships */
    QList<QContactRelationship> relationships(const QContactId& participantId, QContactRelationshipFilter::Role role = QContactRelationshipFilter::Either) const;
    QList<QContactRelationship> relationships(const QString& relationshipType = QString(), const QContactId& participantId = QContactId(), QContactRelationshipFilter::Role role = QContactRelationshipFilter::Either) const;
    bool saveRelationship(QContactRelationship* relationship);
    QList<QContactManager::Error> saveRelationships(QList<QContactRelationship>* relationships);
    bool removeRelationship(const QContactRelationship& relationship);
    QList<QContactManager::Error> removeRelationships(const QList<QContactRelationship>& relationships);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions() const;
    QContactDetailDefinition detailDefinition(const QString& definitionName) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def);
    bool removeDetailDefinition(const QString& definitionName);

    /* Functionality reporting */
    enum ManagerFeature {
        Groups = 0,
        ActionPreferences,
        MutableDefinitions,
        Relationships,
        ArbitraryRelationshipTypes,
        SelfContact,
        Anonymous,
        ChangeLogs
    };
    bool hasFeature(QContactManager::ManagerFeature feature) const;
    bool filterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;
    QStringList supportedRelationshipTypes() const;

    /* return a list of available backends for which a QContactManager can be constructed. */
    static QStringList availableManagers();

signals:
    void dataChanged();
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void relationshipsAdded(const QList<QContactLocalId>& affectedContactIds);
    void relationshipsRemoved(const QList<QContactLocalId>& affectedContactIds);

private:
    friend class QContactManagerData;
    Q_DISABLE_COPY(QContactManager)
    // private data pointer
    QContactManagerData* d;
};

#endif
