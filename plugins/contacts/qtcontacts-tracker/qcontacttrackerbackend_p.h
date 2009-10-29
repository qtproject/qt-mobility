/* * This file is part of qtcontacts-tracker *
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#ifndef QCONTACTTRACKERBACKEND_P_H
#define QCONTACTTRACKERBACKEND_P_H

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
#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/QLive>

using namespace SopranoLive;
#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerenginefactory.h"
#include "qtrackercontactasyncrequest.h"

class QContactAbstractRequest;
class QContactChangeSet;

class QContactTrackerEngineData : public QSharedData
{
public:
    QContactTrackerEngineData()
        : QSharedData(), m_refCount(QAtomicInt(1))
    {
    }

    QContactTrackerEngineData(const QContactTrackerEngineData& other)
        : QSharedData(other), m_refCount(QAtomicInt(1)),
        m_lastUsedId(other.m_lastUsedId),
        m_definitions(other.m_definitions)
    {
    }

    void init();

    ~QContactTrackerEngineData() {}

    QAtomicInt m_refCount;
    mutable QContactLocalId m_lastUsedId;
    mutable QMap<QString, QContactDetailDefinition> m_definitions;
    mutable QMap<QContactAbstractRequest*, QTrackerContactAsyncRequest*> m_requests;
};

class QTCONTACTS_EXPORT QContactTrackerEngine : public QContactManagerEngine
{
Q_OBJECT

public:
    QContactTrackerEngine(const QMap<QString, QString>& parameters);
    QContactTrackerEngine(const QContactTrackerEngine& other);
    ~QContactTrackerEngine();
    QContactTrackerEngine& operator=(const QContactTrackerEngine& other);
    QContactManagerEngine* clone();
    void deref();

    /* Filtering */
    QList<QContactLocalId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QContact contact(const QContactLocalId& contactId, QContactManager::Error& error) const;

    bool saveContact(QContact* contact, QContactManager::Error& error);

    QList<QContactManager::Error> saveContacts(QList<QContact>* contacts, QContactManager::Error& error);

    bool removeContact(const QContactLocalId& contactId, QContactManager::Error& error);
    QList<QContactManager::Error> removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(QContactManager::Error& error) const;
    QContactDetailDefinition detailDefinition(const QString& definitionId, QContactManager::Error& error) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error);
    bool removeDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error);

    /* Asynchronous Request Support */
    void requestDestroyed(QContactAbstractRequest* req);
    bool startRequest(QContactAbstractRequest* req);
    bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    bool hasFeature(QContactManagerInfo::ManagerFeature feature) const;
    bool filterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;


private:
    //called from both constructors, connecting to all contact NodeList changes signals
    void connectToSignals();
    RDFVariable contactDetail2Rdf(const RDFVariable& rdfContact, const QString& definitionName, const QString& fieldName) const;
    QContact contact_impl(const QContactLocalId& contactId, QContactManager::Error& error) const;
private:
    QSharedDataPointer<QContactTrackerEngineData> d;
    const QString contactArchiveFile;
    const QString contactArchiveDir;
    friend class ut_qtcontacts_trackerplugin;
};

class Q_DECL_EXPORT ContactTrackerFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error&);
        QString managerName() const;
};

#endif
