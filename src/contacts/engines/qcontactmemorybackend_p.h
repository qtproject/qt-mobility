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


#ifndef QCONTACTMEMORYBACKEND_P_H
#define QCONTACTMEMORYBACKEND_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QSharedData>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QDateTime>
#include <QString>
#include <QObject>

#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactdetaildefinition.h"
#include "qcontactabstractrequest.h"
#include "qcontactchangeset.h"

QTM_BEGIN_NAMESPACE

class QContactAbstractRequest;
class QContactMemoryEngineData : public QSharedData
{
public:
    QContactMemoryEngineData()
        : QSharedData(),
        m_refCount(QAtomicInt(1)),
        m_selfContactId(0),
        m_nextContactId(1),
        m_anonymous(false),
        m_engineVersion(0)
    {
    }

    QContactMemoryEngineData(const QContactMemoryEngineData& other)
        : QSharedData(other),
        m_refCount(QAtomicInt(1)),
        m_selfContactId(other.m_selfContactId),
        m_nextContactId(other.m_nextContactId),
        m_anonymous(other.m_anonymous),
        m_engineVersion(0)
    {
    }

    ~QContactMemoryEngineData()
    {
    }

    QAtomicInt m_refCount;
    QString m_id;                                  // the id parameter value

    QContactLocalId m_selfContactId;               // the "MyCard" contact id
    QList<QContact> m_contacts;                    // list of contacts
    QList<QContactLocalId> m_contactIds;           // list of contact Id's
    QList<QContactRelationship> m_relationships;   // list of contact relationships
    QMap<QContactLocalId, QList<QContactRelationship> > m_orderedRelationships; // map of ordered lists of contact relationships
    QList<QString> m_definitionIds;                // list of definition types (id's)
    mutable QMap<QString, QMap<QString, QContactDetailDefinition> > m_definitions; // map of contact type to map of definition name to definitions.
    QContactLocalId m_nextContactId;
    bool m_anonymous;                              // Is this backend ever shared?
    QString m_engineName;                          // name of this engine as supplied by factory (memory)
    int m_engineVersion;                           // version of this engine as supplied by factory

    QQueue<QContactAbstractRequest*> m_asynchronousOperations; // async requests to be performed.
};

class Q_CONTACTS_EXPORT QContactMemoryEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    using QContactManagerEngine::contacts;

    static QContactMemoryEngine *createMemoryEngine(const QMap<QString, QString>& parameters);
    void deref();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& error);
    QContactLocalId selfContactId(QContactManager::Error& error) const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contactIds(const QList<QContactSortOrder> &sortOrders, QContactManager::Error &error) const;
    QList<QContact> contacts(const QList<QContactSortOrder> &sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error &error) const;
    QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error& error) const;
    bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error& error);
    bool saveContact(QContact* contact, QContactManager::Error& error);
    bool removeContacts(QList<QContactLocalId>* contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error& error);
    bool removeContact(const QContactLocalId& contactId, QContactManager::Error& error);

    /* Relationships - Accessors and Mutators */
    QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const;
    bool saveRelationship(QContactRelationship* relationship, QContactManager::Error& error);
    QList<QContactManager::Error> saveRelationships(QList<QContactRelationship>* relationships, QContactManager::Error& error);
    bool removeRelationship(const QContactRelationship& relationship, QContactManager::Error& error);
    QList<QContactManager::Error> removeRelationships(const QList<QContactRelationship>& relationships, QContactManager::Error& error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error& error) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error& error);
    bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error& error);

    /* Asynchronous Request Support */
    void requestDestroyed(QContactAbstractRequest* req);
    bool startRequest(QContactAbstractRequest* req);
    bool cancelRequest(QContactAbstractRequest* req);
    bool waitForRequestProgress(QContactAbstractRequest* req, int msecs);
    bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    QStringList supportedRelationshipTypes(const QString& contactType) const;
    bool filterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;

    /* Version Reporting */
    int implementationVersion() const;

protected:
    QContactMemoryEngine(const QMap<QString, QString>& parameters);

private slots:
    void performAsynchronousOperation();

private:
    /* Implement "signal coalescing" for batch functions via change set */
    bool saveContact(QContact* theContact, QContactChangeSet& changeSet, QContactManager::Error& error);
    bool removeContact(const QContactLocalId& contactId, QContactChangeSet& changeSet, QContactManager::Error& error);
    bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error& error);
    bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error& error);
    bool saveRelationship(QContactRelationship* relationship, QContactChangeSet& changeSet, QContactManager::Error& error);
    bool removeRelationship(const QContactRelationship& relationship, QContactChangeSet& changeSet, QContactManager::Error& error);

    QContactMemoryEngineData* d;
    static QMap<QString, QContactMemoryEngine*> engines;
};

QTM_END_NAMESPACE

#endif

