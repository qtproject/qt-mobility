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


#ifndef QCONTACTMANAGER_P_H
#define QCONTACTMANAGER_P_H

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

#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>

#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactactionmanager_p.h"

QTM_BEGIN_NAMESPACE

class QContactManagerEngineFactory;

/* Data and stuff that is shared amongst all backends */
class QContactManagerData
{
public:
    QContactManagerData()
        : m_engine(0),
        m_error(QContactManager::NoError)
    {
    }

    ~QContactManagerData()
    {
        delete m_engine;
    }

    void createEngine(const QString& managerName, const QMap<QString, QString>& parameters);
    static QContactManagerEngineV2* engine(const QContactManager* manager);

    QContactManagerEngineV2* m_engine;
    QContactManager::Error m_error;

    /* Manager plugins */
    static QHash<QString, QContactManagerEngineFactory*> m_engines;
    static QList<QContactActionManagerPlugin*> m_actionManagers;
    static bool m_discovered;
    static bool m_discoveredStatic;
    static QStringList m_pluginPaths;
    static void loadFactories();
    static void loadStaticFactories();

private:
    Q_DISABLE_COPY(QContactManagerData)
};


/* Wrap a V1 engine and provide the V2 api */
class QContactManagerEngineV2Wrapper : public QContactManagerEngineV2
{
    Q_OBJECT
public:
    QContactManagerEngineV2Wrapper(QContactManagerEngine* wrappee);
    ~QContactManagerEngineV2Wrapper();

    /* URI reporting */
    virtual QString managerName() const {return m_engine->managerName();}
    virtual QMap<QString, QString> managerParameters() const {return m_engine->managerParameters();}
    virtual int managerVersion() const {return m_engine->managerVersion();}

    /* Filtering */
    virtual QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    virtual QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    virtual QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;

    virtual bool saveContact(QContact* contact, QContactManager::Error* error);
    virtual bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);
    virtual bool saveRelationship(QContactRelationship* relationship, QContactManager::Error* error);
    virtual bool removeRelationship(const QContactRelationship& relationship, QContactManager::Error* error);

    virtual bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    virtual bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Return a pruned or modified contact which is valid and can be saved in the backend */
    virtual QContact compatibleContact(const QContact& original, QContactManager::Error* error) const;

    /* Synthesize the display label of a contact */
    virtual QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;

    /* "Self" contact id (MyCard) */
    virtual bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error);
    virtual QContactLocalId selfContactId(QContactManager::Error* error) const;

    /* Relationships between contacts */
    virtual QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const;
    virtual bool saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    virtual bool removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Validation for saving */
    virtual bool validateContact(const QContact& contact, QContactManager::Error* error) const;
    virtual bool validateDefinition(const QContactDetailDefinition& def, QContactManager::Error* error) const;

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;
    virtual QContactDetailDefinition detailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error) const;
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
    virtual QStringList supportedContactTypes() const;


private:
    QContactManagerEngine* m_engine;
};


QTM_END_NAMESPACE

#endif
