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

#include "qtcontactsglobal.h"
#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"


class CntSymbianEnginePrivate;
class QContactChangeSet;
class CntSymbianDatabase;
class CntTransformContact;

class CntSymbianEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    CntSymbianEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
    CntSymbianEngine(const CntSymbianEngine& other);
    ~CntSymbianEngine();
    CntSymbianEngine& operator=(const CntSymbianEngine& other);
    void deref();

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QList<QContactLocalId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QList<QContactLocalId> contacts(const QString& contactType, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;

    QContact contact(const QContactLocalId& contactId, QContactManager::Error& error) const;
    bool saveContact(QContact* contact, QContactManager::Error& error);
    QList<QContactManager::Error> saveContacts(QList<QContact>* contacts, QContactManager::Error& error);
    bool removeContact(const QContactLocalId& contactId, QContactManager::Error& error);
    QList<QContactManager::Error> removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error);

    /* Relationships between contacts */
    QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const;
    bool saveRelationship(QContactRelationship* relationship, QContactManager::Error& error);
    QList<QContactManager::Error> saveRelationships(QList<QContactRelationship>* relationships, QContactManager::Error& error);
    bool removeRelationship(const QContactRelationship& relationship, QContactManager::Error& error);
    QList<QContactManager::Error> removeRelationships(const QList<QContactRelationship>& relationships, QContactManager::Error& error);

    /* Definitions */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error& error) const;

    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    bool filterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;

    /* Synthesize the display label of a contact */
    QString synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& error) const;

    /* "Self" contact id (MyCard) */
    bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& error);
    QContactLocalId selfContactId(QContactManager::Error& error) const;

    QString managerName() const;

private:
    QList<QContactLocalId> slowFilter(const QContactFilter& filter, const QList<QContactLocalId>& contacts, QContactManager::Error& error) const;
    QList<QContactLocalId> slowSort(const QList<QContactLocalId>& contactIds, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    bool doSaveContact(QContact* contact, QContactChangeSet& changeSet, QContactManager::Error& error);

    /* Fetch contact */
    QContact fetchContact(const QContactLocalId& contactId, QContactManager::Error& qtError) const;
    QContact fetchContactL(const QContactLocalId &localId) const;

    /* Add contact */
    bool addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    int addContactL(QContact &contact);

    /* Update contact */
    bool updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    void updateContactL(QContact &contact);

    /* Remove contact */
    bool removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError);
    int removeContactL(QContactLocalId id);

    void updateDisplayLabel(QContact& contact) const;

private:
    CntSymbianDatabase      *m_dataBase;
    QString                 m_managerUri;
    CntTransformContact     *m_transformContact;
    CntSymbianEnginePrivate *d;
};

class Q_DECL_EXPORT CntSymbianFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
        QString managerName() const;
};

#endif
