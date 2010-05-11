/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef QT_SIMULATOR
#include "contactconnection_simulator_p.h"
#endif
#ifdef SIMULATOR_APPLICATION
    class MobilityClient;
#endif

QTM_BEGIN_NAMESPACE

class QContactAbstractRequest;
class QContactManagerEngine;
class QContactMemoryEngineData : public QSharedData
{
public:
    QContactMemoryEngineData()
        : QSharedData(),
        m_refCount(QAtomicInt(1)),
        m_selfContactId(0),
        m_nextContactId(1),
        m_anonymous(false)
    {
    }

    QContactMemoryEngineData(const QContactMemoryEngineData& other)
        : QSharedData(other),
        m_refCount(QAtomicInt(1)),
        m_selfContactId(other.m_selfContactId),
        m_nextContactId(other.m_nextContactId),
        m_anonymous(other.m_anonymous)
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

    void emitSharedSignals(QContactChangeSet* cs)
    {
        foreach(QContactManagerEngine* engine, m_sharedEngines)
            cs->emitSignals(engine);
    }

    QList<QContactManagerEngine*> m_sharedEngines;   // The list of engines that share this data
};

class QContactMemoryEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    static QContactMemoryEngine *createMemoryEngine(const QMap<QString, QString>& parameters);

    ~QContactMemoryEngine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    /*! \reimp */
    int managerVersion() const {return 1;}

    virtual QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    virtual QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    virtual QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;

    virtual bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    virtual bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /*! \reimp */
    virtual QContact compatibleContact(const QContact& original, QContactManager::Error* error) const
    {
        return QContactManagerEngine::compatibleContact(original, error);
    }

    /*! \reimp */
    virtual QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const
    {
        return QContactManagerEngine::synthesizedDisplayLabel(contact, error);
    }

    /* "Self" contact id (MyCard) */
    virtual bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error);
    virtual QContactLocalId selfContactId(QContactManager::Error* error) const;

    /* Relationships between contacts */
    virtual QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const;
    virtual bool saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    virtual bool removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /*! \reimp */
    virtual bool validateContact(const QContact& contact, QContactManager::Error* error) const
    {
        return QContactManagerEngine::validateContact(contact, error);
    }
    /*! \reimp */
    virtual bool validateDefinition(const QContactDetailDefinition& def, QContactManager::Error* error) const
    {
        return QContactManagerEngine::validateDefinition(def, error);
    }

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;
    /*! \reimp */
    virtual QContactDetailDefinition detailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error) const
    {
        return QContactManagerEngine::detailDefinition(definitionId, contactType, error);
    }
    virtual bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error* error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error);

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QContactAbstractRequest* req);
    virtual bool startRequest(QContactAbstractRequest* req);
    virtual bool cancelRequest(QContactAbstractRequest* req);
    virtual bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    virtual bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    virtual bool isRelationshipTypeSupported(const QString& relationshipType, const QString& contactType) const;
    virtual bool isFilterSupported(const QContactFilter& filter) const;
    virtual QList<QVariant::Type> supportedDataTypes() const;
    /*! \reimp */
    virtual QStringList supportedContactTypes() const
    {
        return QContactManagerEngine::supportedContactTypes();
    }

protected:
    QContactMemoryEngine(QContactMemoryEngineData* data);

protected:
    /* Implement "signal coalescing" for batch functions via change set */
    virtual bool saveContact(QContact* theContact, QContactChangeSet& changeSet, QContactManager::Error* error);
    virtual bool removeContact(const QContactLocalId& contactId, QContactChangeSet& changeSet, QContactManager::Error* error);
    virtual bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error* error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactChangeSet& changeSet, QContactManager::Error* error);
    virtual bool saveRelationship(QContactRelationship* relationship, QContactChangeSet& changeSet, QContactManager::Error* error);
    virtual bool removeRelationship(const QContactRelationship& relationship, QContactChangeSet& changeSet, QContactManager::Error* error);

    void performAsynchronousOperation(QContactAbstractRequest* request);

    QContactMemoryEngineData* d;
    static QMap<QString, QContactMemoryEngineData*> engineDatas;

#ifdef SIMULATOR_APPLICATION
    friend class ::MobilityClient;
#endif
#ifdef QT_SIMULATOR
    friend class Simulator::ContactConnection;
#endif
};

QTM_END_NAMESPACE

#endif

