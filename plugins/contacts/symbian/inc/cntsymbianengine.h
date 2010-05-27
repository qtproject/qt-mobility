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

#ifndef CNTSYMBIANBACKEND_P_H
#define CNTSYMBIANBACKEND_P_H

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
#include <QObject>
#include <QQueue>

#include <qmobilityglobal.h>
#include "qtcontactsglobal.h"
#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"

QTM_BEGIN_NAMESPACE
class QContactChangeSet;
class QContactAbstractRequest;
QTM_END_NAMESPACE
class CntSymbianDatabase;
class CntSymbianSrvConnection;
class CntTransformContact;
class CntSymbianFilter;
class CntAbstractContactSorter;
class CntRelationship;
class CntDisplayLabel;

QTM_USE_NAMESPACE

#define CNT_SYMBIAN_MANAGER_NAME "symbian"

class CntSymbianEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    CntSymbianEngine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
    ~CntSymbianEngine();
    
    /* URI reporting */
    QString managerName() const;

    /* XXX TODO - implement these correctly */
    int managerVersion() const { return 1;}
    QContact compatibleContact(const QContact& contact, QContactManager::Error* error) const {return QContactManagerEngine::compatibleContact(contact, error);}

    /* Functions that are optional in the base API */
    bool saveRelationship(QContactRelationship* relationship, QContactManager::Error* error);
    bool removeRelationship(const QContactRelationship& relationship, QContactManager::Error* error);
    bool saveContact(QContact* contact, QContactManager::Error* error);
    bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;

    bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Synthesize the display label of a contact */
    QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;
    
    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error);
    QContactLocalId selfContactId(QContactManager::Error* error) const;
    
    /* Relationships between contacts */
    QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const;
    bool saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    bool removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;

    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    bool isRelationshipTypeSupported(const QString& relationshipType, const QString& contactType) const;
    bool isFilterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;

private:
    QList<QContactLocalId> slowFilter(const QContactFilter& filter, const QList<QContactLocalId>& contacts, QContactManager::Error* error) const;
    QList<QContactLocalId> slowSort(const QList<QContactLocalId>& contactIds, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    bool doSaveContact(QContact* contact, QContactChangeSet& changeSet, QContactManager::Error* error);

    QContact fetchContactL(const QContactLocalId &localId, const QStringList& detailDefinitionsHint) const;

    /* Add contact */
    bool addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error* qtError);
    int addContactL(QContact &contact);

    /* Update contact */
    bool updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error* qtError);
    void updateContactL(QContact &contact);

    /* Remove contact */
    bool removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error* qtError);
    void removeContactL(QContactLocalId id);

    void updateDisplayLabel(QContact& contact) const;

    /* Asynchronous Request Support - synchronous versions until thread worker is stable */
    void requestDestroyed(QContactAbstractRequest* req);
    bool startRequest(QContactAbstractRequest* req);
    bool cancelRequest(QContactAbstractRequest* req);
    bool waitForRequestProgress(QContactAbstractRequest* req, int msecs);
    bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);

private slots:
    void performAsynchronousOperation();

private:
    CntSymbianDatabase *m_dataBase;
    CntSymbianSrvConnection *m_srvConnection;
    QString m_managerUri;
    CntTransformContact *m_transformContact;
    CntSymbianFilter *m_contactFilter;
#ifndef SYMBIAN_BACKEND_USE_SQLITE    
    CntAbstractContactSorter *m_contactSorter;
#endif    
    CntRelationship *m_relationship;
    CntDisplayLabel *m_displayLabel;

    QQueue<QContactAbstractRequest*> m_asynchronousOperations; // async requests to be performed.
#ifdef PBK_UNIT_TEST
    friend class TestSymbianEngine;
    friend class TestCntRelationship;
#endif  //PBK_UNIT_TEST
};
#ifndef PBK_UNIT_TEST
class CntSymbianFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
        QString managerName() const;
};
#endif  //PBK_UNIT_TEST
#endif  //CNTSYMBIANBACKEND_P_H
