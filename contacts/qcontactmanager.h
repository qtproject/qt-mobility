/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONTACTMANAGER_H
#define QCONTACTMANAGER_H

#include <QObject>

#include <QMap>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>

#include <QSharedPointer>


#include "qcontact.h"
#include "qcontactgroup.h"
#include "qcontactmanagerinfo.h"

class QContactManagerData;
class QTCONTACTS_EXPORT QContactManager : public QObject
{
    Q_OBJECT

public:
#if Q_QDOC // qdoc's parser fails to recognise the default map argument
    QContactManager(const QString& managerId = QString(), const QMap<QString, QString>& parameters = 0);
#else
    QContactManager(const QString& managerId = QString(), const QMap<QString, QString>& parameters = (QMap<QString, QString>()));
#endif

    static QContactManager fromUri(const QString& uri);

    ~QContactManager();                     // dtor

    QContactManager(const QContactManager& other);
    QContactManager& operator=(const QContactManager& other);

    QString managerId() const;                         // e.g. "Symbian"
    QMap<QString, QString> managerParameters() const;  // e.g. "filename=private.db"
    QString storeUri() const;                          // managerId + managerParameters

    static bool splitUri(const QString& uri, QString* managerId, QMap<QString, QString>* params);
    static QString buildUri(const QString& managerId, const QMap<QString, QString>& params);

    /* The values of the Error enum are still to be decided! */
    enum Error {
        NoError = 0,
        DoesNotExistError,
        AlreadyExistsError,
        InvalidDetailError,
        LockedError,
        DetailAccessError,
        PermissionsError,
        OutOfMemoryError,
        NotSupportedError,
        BadArgumentError,
        UnspecifiedError
    };

    /* Error reporting - Qt idiom is "error" rather than "lastError" */
    QContactManager::Error error() const;

    /* Contacts - Accessors and Mutators */
    QList<QUniqueId> contacts() const;                   // retrieve contact ids
    QList<QUniqueId> contactsWithDetail(const QString& definitionId, const QVariant& value = QVariant()) const;
    QList<QUniqueId> contactsWithAction(const QString& actionId, const QVariant& value = QVariant()) const;
    QContact contact(const QUniqueId& contactId) const;  // retrieve a contact
    bool saveContact(QContact* contact);                 // note: MODIFIES contact (sets the contactId + storeUri)
    bool removeContact(const QUniqueId& contactId);      // remove the contact from the persistent store
    QList<QContactManager::Error> saveContacts(QList<QContact>* contacts);       // batch API - save
    QList<QContactManager::Error> removeContacts(QList<QUniqueId>* contactIds);  // batch API - remove

    /* Groups - Accessors and Mutators */
    QList<QUniqueId> groups() const;
    QContactGroup group(const QUniqueId& groupId) const;
    bool saveGroup(QContactGroup* group);
    bool removeGroup(const QUniqueId& groupId);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions() const;
    QContactDetailDefinition detailDefinition(const QString& definitionId) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def);
    bool removeDetailDefinition(const QString& definitionId);

    /* Changelog Functions */
    QList<QUniqueId> contactsAddedSince(const QDateTime& timestamp) const;
    QList<QUniqueId> contactsModifiedSince(const QDateTime& timestamp) const;
    QList<QUniqueId> contactsRemovedSince(const QDateTime& timestamp) const;
    QList<QUniqueId> groupsAddedSince(const QDateTime& timestamp) const;
    QList<QUniqueId> groupsModifiedSince(const QDateTime& timestamp) const;
    QList<QUniqueId> groupsRemovedSince(const QDateTime& timestamp) const;

    /* Functionality reporting */
    QContactManagerInfo information() const;

    /* return a list of available backends for which a QContactManager can be constructed. */
    static QStringList availableManagers();

signals:
    void contactsAdded(const QList<QUniqueId>& contactIds);
    void contactsChanged(const QList<QUniqueId>& contactIds);
    void contactsRemoved(const QList<QUniqueId>& contactIds);
    void groupsAdded(const QList<QUniqueId>& groupIds);
    void groupsChanged(const QList<QUniqueId>& groupIds);
    void groupsRemoved(const QList<QUniqueId>& groupIds);

private:
    // private data pointer
    QSharedPointer<QContactManagerData> d;
};

#endif
