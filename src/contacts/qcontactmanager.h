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

QTM_BEGIN_NAMESPACE

class QContactFilter;
class QContactAction;

class QContactManagerData;
class Q_CONTACTS_EXPORT QContactManager : public QObject
{
    Q_OBJECT

public:
#if Q_QDOC // qdoc's parser fails to recognise the default map argument
    QContactManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = 0, QObject* parent = 0);
    QContactManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = 0, QObject* parent = 0);
#else
    QContactManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
    QContactManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
#endif
    QContactManager(QObject* parent);

    static QContactManager* fromUri(const QString& uri, QObject* parent = 0);
    ~QContactManager();                     // dtor

    QString managerName() const;                       // e.g. "Symbian"
    QMap<QString, QString> managerParameters() const;  // e.g. "filename=private.db"
    QString managerUri() const;                        // managerName + managerParameters
    int managerVersion() const;

    static bool parseUri(const QString& uri, QString* managerName, QMap<QString, QString>* params); // replaces the above.
    static QString buildUri(const QString& managerName, const QMap<QString, QString>& params, int implementationVersion = -1);

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
        UnspecifiedError,
        VersionMismatchError,
        LimitReachedError,
        InvalidContactTypeError
    };

    /* Error reporting */
    QContactManager::Error error() const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const;
    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>()) const;
    QList<QContact> contacts(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QStringList& definitionRestrictions = QStringList()) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QStringList& definitionRestrictions = QStringList()) const;

    QContact contact(const QContactLocalId& contactId, const QStringList& definitionRestrictions = QStringList()) const;  // retrieve a contact

    bool saveContact(QContact* contact);                 // note: MODIFIES contact (sets the contactId)
    bool removeContact(const QContactLocalId& contactId);      // remove the contact from the persistent store

    QList<QContactManager::Error> Q_DECL_DEPRECATED saveContacts(QList<QContact>* contacts);       // deprecated batch API - save
    QList<QContactManager::Error> Q_DECL_DEPRECATED removeContacts(QList<QContactLocalId>* contactIds);  // deprecated batch API - remove
    bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap); // batch API - save.
    bool Q_DECL_DEPRECATED removeContacts(QList<QContactLocalId>* contactIds, QMap<int, QContactManager::Error>* errorMap); // batch API - remove. deprecated also.
    bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap); // batch API - remove.

    /* Return a pruned or modified contact which is valid and can be saved in the manager */
    QContact compatibleContact(const QContact& original); // Preliminary function!

    /* Synthesize the display label of a contact */
    QString synthesizedDisplayLabel(const QContact& contact) const; // replaces the above

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& contactId);
    QContactLocalId selfContactId() const;

    /* Relationships */
    QList<QContactRelationship> Q_DECL_DEPRECATED relationships(const QContactId& participantId, QContactRelationshipFilter::Role role = QContactRelationshipFilter::Either) const;
    QList<QContactRelationship> Q_DECL_DEPRECATED relationships(const QString& relationshipType = QString(), const QContactId& participantId = QContactId(), QContactRelationshipFilter::Role role = QContactRelationshipFilter::Either) const;
    QList<QContactRelationship> relationships(const QContactId& participantId, QContactRelationship::Role role /* = QContactRelationship::Either */) const;
    QList<QContactRelationship> relationships(const QString& relationshipType /* = QString() */, const QContactId& participantId /* = QContactId() */, QContactRelationship::Role role /* = QContactRelationship::Either */) const;
    bool saveRelationship(QContactRelationship* relationship);
    QList<QContactManager::Error> Q_DECL_DEPRECATED saveRelationships(QList<QContactRelationship>* relationships); // deprecated
    bool saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap); // replaces the above
    bool removeRelationship(const QContactRelationship& relationship);
    QList<QContactManager::Error> Q_DECL_DEPRECATED removeRelationships(const QList<QContactRelationship>& relationships); // deprecated
    bool removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap); // replaces the above

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType = QContactType::TypeContact) const;
    QContactDetailDefinition detailDefinition(const QString& definitionName, const QString& contactType = QContactType::TypeContact) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType = QContactType::TypeContact);
    bool removeDetailDefinition(const QString& definitionName, const QString& contactType = QContactType::TypeContact);

    /* Functionality reporting */
    enum ManagerFeature {
        Groups = 0,               // backend supports QContactType::TypeGroup type contacts (convenience for clients... should be deprecated)
        ActionPreferences,        // per-contact action preferences
        MutableDefinitions,
        Relationships,
        ArbitraryRelationshipTypes,
        RelationshipOrdering,     // deprecated along with setRelationshipOrder() etc in QContact.
        DetailOrdering,
        SelfContact,
        Anonymous,
        ChangeLogs
    };
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType = QContactType::TypeContact) const;
    QStringList supportedRelationshipTypes(const QString& contactType = QContactType::TypeContact) const;
    QList<QVariant::Type> supportedDataTypes() const;
    bool isFilterSupported(const QContactFilter& filter) const;// replaces the above.
    QStringList supportedContactTypes() const;

    /* return a list of available backends for which a QContactManager can be constructed. */
    static QStringList availableManagers();

Q_SIGNALS:
    void dataChanged();
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void relationshipsAdded(const QList<QContactLocalId>& affectedContactIds);
    void relationshipsRemoved(const QList<QContactLocalId>& affectedContactIds);
    void selfContactIdChanged(const QContactLocalId& oldId, const QContactLocalId& newId); // need both? or just new?

private:
    friend class QContactManagerData;
    void createEngine(const QString& managerName, const QMap<QString, QString>& parameters); 
    Q_DISABLE_COPY(QContactManager)
    // private data pointer
    QContactManagerData* d;
};

QTM_END_NAMESPACE

#endif
