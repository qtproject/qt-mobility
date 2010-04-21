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


#ifndef QORGANIZERITEMMANAGER_H
#define QORGANIZERITEMMANAGER_H

#include <QObject>

#include <QMap>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>

#include "qtorganizeritemsglobal.h"
#include "qorganizeritem.h"
#include "qorganizeritemid.h"
#include "qorganizeritemrelationship.h"
#include "qorganizeritemsortorder.h"
#include "qorganizeritemfetchhint.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemFilter;
class QOrganizerItemAction;

class QOrganizerItemManagerData;
class Q_CONTACTS_EXPORT QOrganizerItemManager : public QObject
{
    Q_OBJECT

public:
#if Q_QDOC // qdoc's parser fails to recognise the default map argument
    explicit QOrganizerItemManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = 0, QObject* parent = 0);
    QOrganizerItemManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = 0, QObject* parent = 0);
#else
    explicit QOrganizerItemManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
    QOrganizerItemManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
#endif
    explicit QOrganizerItemManager(QObject* parent);

    static QOrganizerItemManager* fromUri(const QString& uri, QObject* parent = 0);
    ~QOrganizerItemManager();                     // dtor

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
    QOrganizerItemManager::Error error() const;

    /* Contacts - Accessors and Mutators */
    QList<QOrganizerItemLocalId> contactIds(const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>()) const;
    QList<QOrganizerItemLocalId> contactIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>()) const;

    // these three functions replace the three deprecated functions above.
    QList<QOrganizerItem> contacts(const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>(), const QOrganizerItemFetchHint& fetchHint = QOrganizerItemFetchHint()) const;
    QList<QOrganizerItem> contacts(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>(), const QOrganizerItemFetchHint& fetchHint = QOrganizerItemFetchHint()) const;
    QOrganizerItem contact(const QOrganizerItemLocalId& contactId, const QOrganizerItemFetchHint& fetchHint = QOrganizerItemFetchHint()) const;  // retrieve a contact

    bool saveContact(QOrganizerItem* contact);                 // note: MODIFIES contact (sets the contactId)
    bool removeContact(const QOrganizerItemLocalId& contactId);      // remove the contact from the persistent store

    bool saveContacts(QList<QOrganizerItem>* contacts, QMap<int, QOrganizerItemManager::Error>* errorMap); // batch API - save.
    bool removeContacts(const QList<QOrganizerItemLocalId>& contactIds, QMap<int, QOrganizerItemManager::Error>* errorMap); // batch API - remove.

    /* Return a pruned or modified contact which is valid and can be saved in the manager */
    QOrganizerItem compatibleContact(const QOrganizerItem& original); // Preliminary function!

    /* Synthesize the display label of a contact */
    QString synthesizedContactDisplayLabel(const QOrganizerItem& contact) const;
    void synthesizeContactDisplayLabel(QOrganizerItem* contact) const;

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QOrganizerItemLocalId& contactId);
    QOrganizerItemLocalId selfContactId() const;

    /* Relationships */
    QList<QOrganizerItemRelationship> relationships(const QOrganizerItemId& participantId, QOrganizerItemRelationship::Role role = QOrganizerItemRelationship::Either) const;
    QList<QOrganizerItemRelationship> relationships(const QString& relationshipType = QString(), const QOrganizerItemId& participantId = QOrganizerItemId(), QOrganizerItemRelationship::Role role = QOrganizerItemRelationship::Either ) const;
    bool saveRelationship(QOrganizerItemRelationship* relationship);
    bool saveRelationships(QList<QOrganizerItemRelationship>* relationships, QMap<int, QOrganizerItemManager::Error>* errorMap);
    bool removeRelationship(const QOrganizerItemRelationship& relationship);
    bool removeRelationships(const QList<QOrganizerItemRelationship>& relationships, QMap<int, QOrganizerItemManager::Error>* errorMap);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& contactType = QOrganizerItemType::TypeContact) const;
    QOrganizerItemDetailDefinition detailDefinition(const QString& definitionName, const QString& contactType = QOrganizerItemType::TypeContact) const;
    bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& contactType = QOrganizerItemType::TypeContact);
    bool removeDetailDefinition(const QString& definitionName, const QString& contactType = QOrganizerItemType::TypeContact);

    /* Functionality reporting */
    enum ManagerFeature {
        Groups = 0,               // backend supports QOrganizerItemType::TypeGroup type contacts (convenience for clients... should be deprecated)
        ActionPreferences,        // per-contact action preferences
        MutableDefinitions,
        Relationships,
        ArbitraryRelationshipTypes,
        RelationshipOrdering,     // deprecated along with setRelationshipOrder() etc in QOrganizerItem.
        DetailOrdering,
        SelfContact,
        Anonymous,
        ChangeLogs
    };
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& contactType = QOrganizerItemType::TypeContact) const;
    bool isRelationshipTypeSupported(const QString& relationshipType, const QString& contactType = QOrganizerItemType::TypeContact) const;
    QList<QVariant::Type> supportedDataTypes() const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QStringList supportedContactTypes() const;

    /* return a list of available backends for which a QOrganizerItemManager can be constructed. */
    static QStringList availableManagers();

Q_SIGNALS:
    void dataChanged();
    void contactsAdded(const QList<QOrganizerItemLocalId>& contactIds);
    void contactsChanged(const QList<QOrganizerItemLocalId>& contactIds);
    void contactsRemoved(const QList<QOrganizerItemLocalId>& contactIds);
    void relationshipsAdded(const QList<QOrganizerItemLocalId>& affectedContactIds);
    void relationshipsRemoved(const QList<QOrganizerItemLocalId>& affectedContactIds);
    void selfContactIdChanged(const QOrganizerItemLocalId& oldId, const QOrganizerItemLocalId& newId); // need both? or just new?

private:
    friend class QOrganizerItemManagerData;
    void createEngine(const QString& managerName, const QMap<QString, QString>& parameters); 
    Q_DISABLE_COPY(QOrganizerItemManager)
    // private data pointer
    QOrganizerItemManagerData* d;
};

QTM_END_NAMESPACE

#endif
