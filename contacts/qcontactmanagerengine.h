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


#ifndef QCONTACTMANAGERENGINE_H
#define QCONTACTMANAGERENGINE_H

#include <QSharedData>
#include <QMap>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QVariant>

#include "qcontact.h"
#include "qcontactdetaildefinition.h"
#include "qcontactgroup.h"
#include "qcontactmanager.h"
#include "qcontactmanagerinfo.h"

class QContactFilter;
class QContactSortOrder;

class QTCONTACTS_EXPORT QContactManagerEngine : public QObject
{
    Q_OBJECT

public:
    QContactManagerEngine() {}
    virtual void deref() = 0;

    /* Filtering */
    virtual QList<QUniqueId> contacts(const QContactFilter& filter, const QContactSortOrder& sortOrder, QContactManager::Error& error) const;

    /* Contacts - Accessors and Mutators */
    virtual QList<QUniqueId> contacts(const QContactSortOrder& sortOrder, QContactManager::Error& error) const;
    virtual QContact contact(const QUniqueId& contactId, QContactManager::Error& error) const;

    virtual bool saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error);
    virtual QList<QContactManager::Error> saveContacts(QList<QContact>* contacts, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error);

    virtual bool removeContact(const QUniqueId& contactId, bool batch, QContactManager::Error& error);
    virtual QList<QContactManager::Error> removeContacts(QList<QUniqueId>* contactIds, QContactManager::Error& error);

    /* Synthesise the display label of a contact */
    virtual QString synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const;

    /* Validation for saving */
    virtual bool validateContact(const QContact& contact, QContactManager::Error& error) const;
    virtual bool validateDefinition(const QContactDetailDefinition& def, QContactManager::Error& error) const;

    /* Groups - Accessors and Mutators */
    virtual QList<QUniqueId> groups(QContactManager::Error& error) const;
    virtual QContactGroup group(const QUniqueId& groupId, QContactManager::Error& error) const;
    virtual bool saveGroup(QContactGroup* group, QContactManager::Error& error);
    virtual bool removeGroup(const QUniqueId& groupId, QContactManager::Error& error);

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(QContactManager::Error& error) const;
    virtual QContactDetailDefinition detailDefinition(const QString& definitionId, QContactManager::Error& error) const;
    virtual bool saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error);
    virtual bool removeDetailDefinition(const QString& definitionId, QContactManager::Error& error);

    /* Changelog Functions */
    virtual QList<QUniqueId> contactsAddedSince(const QDateTime& timestamp, QContactManager::Error& error) const;
    virtual QList<QUniqueId> contactsModifiedSince(const QDateTime& timestamp, QContactManager::Error& error) const;
    virtual QList<QUniqueId> contactsRemovedSince(const QDateTime& timestamp, QContactManager::Error& error) const;

    virtual QList<QUniqueId> groupsAddedSince(const QDateTime& timestamp, QContactManager::Error& error) const;
    virtual QList<QUniqueId> groupsModifiedSince(const QDateTime& timestamp, QContactManager::Error& error) const;
    virtual QList<QUniqueId> groupsRemovedSince(const QDateTime& timestamp, QContactManager::Error& error) const;

    /* Capabilities reporting */
    virtual bool hasFeature(QContactManagerInfo::ManagerFeature feature) const;
    virtual bool filterSupported(const QContactFilter& filter) const;
    virtual QList<QVariant::Type> supportedDataTypes() const;

    /* Reports the built-in definitions from the schema */
    static QMap<QString, QContactDetailDefinition> schemaDefinitions();

signals:
    void contactsAdded(const QList<QUniqueId>& contactIds);
    void contactsChanged(const QList<QUniqueId>& contactIds);
    void contactsRemoved(const QList<QUniqueId>& contactIds);
    void groupsAdded(const QList<QUniqueId>& groupIds);
    void groupsChanged(const QList<QUniqueId>& groupIds);
    void groupsRemoved(const QList<QUniqueId>& groupIds);

public:
    /* Helper functions */
    static void addSorted(QList<QContact>* sorted, const QContact& toAdd, const QContactSortOrder& sortOrder);
    static int compareVariant(const QVariant& first, const QVariant& second, Qt::CaseSensitivity sensitivity);
    static bool testFilter(const QContactFilter& filter, const QContact& contact);
};

#endif
