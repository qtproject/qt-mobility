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

using namespace SopranoLive;

#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

namespace ContactContext {
    typedef enum Location {
        Unknown = 0,
        Home,
        Work
    };
}

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

    ~QContactTrackerEngineData() {}

    QAtomicInt m_refCount;
    mutable QUniqueId m_lastUsedId;
    mutable QMap<QString, QContactDetailDefinition> m_definitions;
    Live<nco::Contact> getContactByContext(const QContactDetail& det, const Live<nco::PersonContact>& ncoContact);
    ContactContext::Location locationContext(const QContactDetail& det) const;
};

class QTCONTACTS_EXPORT QContactTrackerEngine : public QContactManagerEngine
{



public:
    QContactTrackerEngine(const QMap<QString, QString>& parameters);
    QContactTrackerEngine(const QContactTrackerEngine& other);
    ~QContactTrackerEngine();
    QContactTrackerEngine& operator=(const QContactTrackerEngine& other);
    QContactManagerEngine* clone();
    void deref();

    /* Contacts - Accessors and Mutators */
    QList<QUniqueId> contacts() const;
    QList<QUniqueId> contactsWithDetail(const QString& definitionId, const QVariant& value) const;
    bool saveContact(QContact* contact, bool batch, QContactManager::Error& error);
    QContact contact(const QUniqueId& contactId, QContactManager::Error& error) const;
    bool removeContact(const QUniqueId& contactId, bool batch = false);
    QList<QContactManager::Error> saveContacts(QList<QContact>* contacts);
    QList<QContactManager::Error> removeContacts(QList<QUniqueId>* contactIds);

    /* Groups - Accessors and Mutators */
    QList<QUniqueId> groups() const;
    QContactGroup group(const QUniqueId& groupId) const;
    bool saveGroup(QContactGroup* group);
    bool removeGroup(const QUniqueId& groupId);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(QContactManager::Error& error) const;
    QContactDetailDefinition detailDefinition(const QString& definitionId, QContactManager::Error& error) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error);
    bool removeDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error);

    /* Capabilities reporting */
    QStringList capabilities() const;
    QStringList fastFilterableDefinitions() const;
    QList<QVariant::Type> supportedDataTypes() const;

private:
    QSharedDataPointer<QContactTrackerEngineData> d;
};

class Q_DECL_EXPORT ContactTrackerFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error&);
        QString managerId() const;
};

#endif
