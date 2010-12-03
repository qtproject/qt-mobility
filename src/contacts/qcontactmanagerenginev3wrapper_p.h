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

#ifndef QCONTACTMANAGERENGINEV3WRAPPER_P_H
#define QCONTACTMANAGERENGINEV3WRAPPER_P_H

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

#include <QPointer>
#include <QScopedPointer>
#include "qcontactmanagerengine.h"

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

class RequestController;

/* Wrap a V2 engine and provide the V3 api */
class QContactManagerEngineV3Wrapper : public QContactManagerEngineV3
{
    Q_OBJECT
public:
    QContactManagerEngineV3Wrapper(QContactManagerEngineV2* wrappee);
    ~QContactManagerEngineV3Wrapper();

    /* New functions */
    QSharedPointer<QContactObserver> observeContact(QContactLocalId contactId);

    /* All of the following are simply proxied to the wrapped engine */
    virtual QString managerName() const {return m_engine->managerName();}
    virtual QMap<QString, QString> managerParameters() const {return m_engine->managerParameters();}
    virtual int managerVersion() const {return m_engine->managerVersion();}
    virtual QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const {return m_engine->contactIds(filter, sortOrders, error);}
    virtual QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const {return m_engine->contacts(filter, sortOrders, fetchHint, error);}
    virtual QList<QContact> contacts(const QList<QContactLocalId>& localIds, const QContactFetchHint& fetchHint, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error) const {return m_engine->contacts(localIds, fetchHint, errorMap, error);}
    virtual QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const {return m_engine->contact(contactId, fetchHint, error);}
    virtual bool saveContact(QContact* contact, QContactManager::Error* error) {return m_engine->saveContact(contact, error);}
    virtual bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error) {return m_engine->removeContact(contactId, error);}
    virtual bool saveRelationship(QContactRelationship* relationship, QContactManager::Error* error) {return m_engine->saveRelationship(relationship, error);}
    virtual bool removeRelationship(const QContactRelationship& relationship, QContactManager::Error* error) {return m_engine->removeRelationship(relationship, error);}
    virtual bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error) {return m_engine->saveContacts(contacts, errorMap, error);}
    virtual bool saveContacts(QList<QContact>* contacts,  const QStringList& definitionMask, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error) {return m_engine->saveContacts(contacts, definitionMask, errorMap, error);}
    virtual bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error) {return m_engine->removeContacts(contactIds, errorMap, error);}
    virtual QContact compatibleContact(const QContact& original, QContactManager::Error* error) const {return m_engine->compatibleContact(original, error);}
    virtual QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const {return m_engine->synthesizedDisplayLabel(contact, error);}
    virtual bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error) {return m_engine->setSelfContactId(contactId, error);}
    virtual QContactLocalId selfContactId(QContactManager::Error* error) const {return m_engine->selfContactId(error);}
    virtual QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const {return m_engine->relationships(relationshipType, participantId, role, error);}
    virtual bool saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error) {return m_engine->saveRelationships(relationships, errorMap, error);}
    virtual bool removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error) {return m_engine->removeRelationships(relationships, errorMap, error);}
    virtual bool validateContact(const QContact& contact, QContactManager::Error* error) const {return m_engine->validateContact(contact, error);}
    virtual bool validateDefinition(const QContactDetailDefinition& def, QContactManager::Error* error) const {return m_engine->validateDefinition(def, error);}
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const {return m_engine->detailDefinitions(contactType, error);}
    virtual QContactDetailDefinition detailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error) const {return m_engine->detailDefinition(definitionId, contactType, error);}
    virtual bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error* error) {return m_engine->saveDetailDefinition(def, contactType, error);}
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error) {return m_engine->removeDetailDefinition(definitionId, contactType, error);}
    virtual bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const {return m_engine->hasFeature(feature, contactType);}
    virtual bool isRelationshipTypeSupported(const QString& relationshipType, const QString& contactType) const {return m_engine->isRelationshipTypeSupported(relationshipType, contactType);}
    virtual bool isFilterSupported(const QContactFilter& filter) const {return m_engine->isFilterSupported(filter);}
    virtual QList<QVariant::Type> supportedDataTypes() const {return m_engine->supportedDataTypes();}
    virtual QStringList supportedContactTypes() const {return m_engine->supportedContactTypes();}
    virtual bool startRequest(QContactAbstractRequest* req) {return m_engine->startRequest(req);}
    virtual void requestDestroyed(QContactAbstractRequest* req) {m_engine->requestDestroyed(req);}
    virtual bool cancelRequest(QContactAbstractRequest* req) {return m_engine->cancelRequest(req);}
    virtual bool waitForRequestFinished(QContactAbstractRequest* req, int msecs) {return m_engine->waitForRequestFinished(req, msecs);}

#ifdef SIMULATOR_APPLICATION
    QContactManagerEngineV2 *wrappee() {return m_engine;}
#endif

private Q_SLOTS:
    void contactsUpdated(const QList<QContactLocalId>& ids);
    void contactsDeleted(const QList<QContactLocalId>& ids);
    void observerDestroyed(QContactObserver* object);

private:
    QMultiHash<QContactLocalId, QContactObserver*> m_observerForContact;
    QContactManagerEngineV2* m_engine;

    friend class FetchByIdRequestController;
};


QTM_END_NAMESPACE

#endif
