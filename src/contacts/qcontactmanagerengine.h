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


#ifndef QCONTACTMANAGERENGINE_H
#define QCONTACTMANAGERENGINE_H

#include <QSharedData>
#include <QMap>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QObject>

#include "qcontact.h"
#include "qcontactdetaildefinition.h"
#include "qcontactmanager.h"
#include "qcontactabstractrequest.h"
#include "qcontactrequests.h"

QTM_BEGIN_NAMESPACE

class QContactFilter;
class QContactSortOrder;

class Q_CONTACTS_EXPORT QContactManagerEngine : public QObject
{
    Q_OBJECT

public:
    QContactManagerEngine() {}
    virtual void deref() = 0;

    /* URI reporting */
    virtual QString managerName() const;                       // e.g. "Symbian"
    virtual QMap<QString, QString> managerParameters() const;  // e.g. "filename=private.db"
    QString managerUri() const;
    virtual int Q_DECL_DEPRECATED implementationVersion() const; // deprecated
    virtual int managerVersion() const; // replaces the above

    /* Filtering */
    virtual QList<QContactLocalId> Q_DECL_DEPRECATED contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;

    /* Contacts - Accessors and Mutators */
    virtual QList<QContactLocalId> Q_DECL_DEPRECATED contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    virtual QContact Q_DECL_DEPRECATED contact(const QContactLocalId& contactId, QContactManager::Error& error) const;
    virtual QList<QContactManager::Error> Q_DECL_DEPRECATED saveContacts(QList<QContact>* contacts, QContactManager::Error& error);  // deprecated - removed week 3
    virtual QList<QContactManager::Error> Q_DECL_DEPRECATED removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error); // deprecated - removed week 3

    virtual QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    virtual QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    virtual QList<QContact> contacts(const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error& error) const;
    virtual QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error& error) const;
    virtual QContact contact(const QContactLocalId& contactId, const QStringList& definitionRestrictions, QContactManager::Error& error) const;

    virtual bool saveContact(QContact* contact, QContactManager::Error& error);
    virtual bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error& error);
    virtual bool removeContact(const QContactLocalId& contactId, QContactManager::Error& error);
    virtual bool removeContacts(QList<QContactLocalId>* contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error& error);

    /* Synthesize the display label of a contact */
    virtual QString Q_DECL_DEPRECATED synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& error) const; // deprecated
    virtual QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error& error) const; // replaces the above
    QContact setContactDisplayLabel(const QString& displayLabel, const QContact& contact) const;

    /* "Self" contact id (MyCard) */
    virtual bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& error);
    virtual QContactLocalId selfContactId(QContactManager::Error& error) const;

    /* Relationships between contacts */
    virtual QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const;
    virtual bool saveRelationship(QContactRelationship* relationship, QContactManager::Error& error);
    virtual QList<QContactManager::Error> saveRelationships(QList<QContactRelationship>* relationships, QContactManager::Error& error);
    virtual bool removeRelationship(const QContactRelationship& relationship, QContactManager::Error& error);
    virtual QList<QContactManager::Error> removeRelationships(const QList<QContactRelationship>& relationships, QContactManager::Error& error);

    /* Validation for saving */
    virtual bool validateContact(const QContact& contact, QContactManager::Error& error) const;
    virtual bool validateDefinition(const QContactDetailDefinition& def, QContactManager::Error& error) const;

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error& error) const;
    virtual QContactDetailDefinition detailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error& error) const;
    virtual bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error& error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error& error);

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QContactAbstractRequest* req);
    virtual bool startRequest(QContactAbstractRequest* req);
    virtual bool cancelRequest(QContactAbstractRequest* req);
    virtual bool Q_DECL_DEPRECATED waitForRequestProgress(QContactAbstractRequest* req, int msecs);
    virtual bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);

    // the following helper functions are all deprecated and will be removed in week 3.
    static void Q_DECL_DEPRECATED updateRequestStatus(QContactAbstractRequest* req, QContactManager::Error error, QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly = false);
    static void Q_DECL_DEPRECATED updateRequest(QContactAbstractRequest* req, const QList<QContactLocalId>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly = false);
    static void Q_DECL_DEPRECATED updateRequest(QContactAbstractRequest* req, const QList<QContact>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly = false);
    static void Q_DECL_DEPRECATED updateRequest(QContactAbstractRequest* req, const QList<QContactDetailDefinition>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status);
    static void Q_DECL_DEPRECATED updateRequest(QContactAbstractRequest* req, const QMap<QString, QContactDetailDefinition>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly = false);
    static void Q_DECL_DEPRECATED updateRequest(QContactAbstractRequest* req, const QList<QContactRelationship>& result, QContactManager::Error error, const QList<QContactManager::Error>& errors, QContactAbstractRequest::Status status, bool appendOnly = false);

    // they are replaced by the following functions:
    static void updateRequestState(QContactAbstractRequest* req, QContactAbstractRequest::State state);
    static void updateContactLocalIdFetchRequest(QContactLocalIdFetchRequest* req, const QList<QContactLocalId>& result, QContactManager::Error error);
    static void updateContactFetchRequest(QContactFetchRequest* req, const QList<QContact>& result, QContactManager::Error error);
    static void updateContactRemoveRequest(QContactRemoveRequest* req, QContactManager::Error error, const QMap<int, QContactManager::Error>& errorMap);
    static void updateContactSaveRequest(QContactSaveRequest* req, const QList<QContact>& result, QContactManager::Error error, const QMap<int, QContactManager::Error>& errorMap);
    static void updateDefinitionSaveRequest(QContactDetailDefinitionSaveRequest* req, const QList<QContactDetailDefinition>& result, QContactManager::Error error, const QMap<int, QContactManager::Error>& errorMap);
    static void updateDefinitionRemoveRequest(QContactDetailDefinitionRemoveRequest* req, QContactManager::Error error, const QMap<int, QContactManager::Error>& errorMap);
    static void updateDefinitionFetchRequest(QContactDetailDefinitionFetchRequest* req, const QMap<QString, QContactDetailDefinition>& result, QContactManager::Error error, const QMap<int, QContactManager::Error>& errorMap);
    static void updateRelationshipSaveRequest(QContactRelationshipSaveRequest* req, const QList<QContactRelationship>& result, QContactManager::Error error, const QMap<int, QContactManager::Error>& errorMap);
    static void updateRelationshipRemoveRequest(QContactRelationshipRemoveRequest* req, QContactManager::Error error, const QMap<int, QContactManager::Error>& errorMap);
    static void updateRelationshipFetchRequest(QContactRelationshipFetchRequest* req, const QList<QContactRelationship>& result, QContactManager::Error error);


    /* Capabilities reporting */
    virtual bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    virtual QStringList supportedRelationshipTypes(const QString& contactType) const;
    virtual bool Q_DECL_DEPRECATED filterSupported(const QContactFilter& filter) const; // deprecated
    virtual bool isFilterSupported(const QContactFilter& filter) const; // replaces the above
    virtual QList<QVariant::Type> supportedDataTypes() const;
    virtual QStringList supportedContactTypes() const;
 
    /* Versions */ 
    static Q_DECL_DEPRECATED int version(); // deprecated, remove in wk1, no replacement.

    /* Reports the built-in definitions from the schema */
    static QMap<QString, QMap<QString, QContactDetailDefinition> > schemaDefinitions();

signals:
    void dataChanged();
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void relationshipsAdded(const QList<QContactLocalId>& affectedContactIds);
    void relationshipsRemoved(const QList<QContactLocalId>& affectedContactIds);
    void selfContactIdChanged(const QContactLocalId& oldId, const QContactLocalId& newId);

public:
    /* Helper functions */
    static int compareContact(const QContact& a, const QContact& b, const QList<QContactSortOrder>& sortOrders);
    static void addSorted(QList<QContact>* sorted, const QContact& toAdd, const QList<QContactSortOrder>& sortOrders);
    static int compareVariant(const QVariant& first, const QVariant& second, Qt::CaseSensitivity sensitivity);
    static bool testFilter(const QContactFilter& filter, const QContact& contact);
    static bool validateActionFilter(const QContactFilter& filter);
    static QList<QContactLocalId> sortContacts(const QList<QContact>& contacts, const QList<QContactSortOrder>& sortOrders);
    static void setContactRelationships(QContact* contact, const QList<QContactRelationship>& relationships);

protected:
    void setDetailAccessConstraints(QContactDetail* detail, QContactDetail::AccessConstraints) const;

private:
    /* QContactChangeSet is a utility class used to emit the appropriate signals */
    friend class QContactChangeSet;
};

QTM_END_NAMESPACE

#endif
