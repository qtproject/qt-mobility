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

#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactdetaildefinition.h"
#include "qcontactabstractrequest.h"

class QContactManagerInfoPrivate;
class QContactMemoryEngineData : public QSharedData
{
public:
    QContactMemoryEngineData()
    : m_refCount(QAtomicInt(1)),
    m_nextContactId(1),
    m_nextGroupId(1)
    {
    }

    QContactMemoryEngineData(const QContactMemoryEngineData& other)
        : QSharedData(other),
        m_refCount(QAtomicInt(1)),
        m_nextContactId(other.m_nextContactId),
        m_nextGroupId(other.m_nextGroupId)
    {
    }

    ~QContactMemoryEngineData() {}

    QAtomicInt m_refCount;
    QString m_id;

    QMap<QContactAbstractRequest*, QContactAbstractRequestResult*> m_asynchronousRequests;
    QQueue<QPair<QContactAbstractRequest*, QContactAbstractRequest::Operation> > m_asynchronousOperations;

    QList<QContact> m_contacts;                    // list of contacts
    QMap<QUniqueId, QContactGroup> m_groups;       // map of group id to groups.
    QList<QUniqueId> m_contactIds;                 // list of contact Id's
    QList<QString> m_definitionIds;                // list of definition types (id's)
    mutable QMap<QString, QContactDetailDefinition> m_definitions; // list of definitions.
    mutable QSet<QString> m_createOnlyIds; // a list of create only ids.
    QUniqueId m_nextContactId;
    QUniqueId m_nextGroupId;
};

class QTCONTACTS_EXPORT QContactMemoryEngine : public QContactManagerEngine
{
public:
    using QContactManagerEngine::contacts;

    static QContactMemoryEngine *createMemoryEngine(const QMap<QString, QString>& parameters);
    void deref();

    /* Contacts - Accessors and Mutators */
    QList<QUniqueId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QContact contact(const QUniqueId& contactId, QContactManager::Error& error) const;
    bool saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error);
    bool removeContact(const QUniqueId& contactId, QSet<QUniqueId>& removedContacts, QSet<QUniqueId>& changedGroups, QContactManager::Error& error);

    /* Groups - Accessors and Mutators */
    QList<QUniqueId> groups(QContactManager::Error& error) const;
    QContactGroup group(const QUniqueId& groupId, QContactManager::Error& error) const;
    bool saveGroup(QContactGroup* group, QSet<QUniqueId>& groupsAdded, QSet<QUniqueId>& groupsChanged, QSet<QUniqueId>& contactsChanged, QContactManager::Error& error);
    bool removeGroup(const QUniqueId& groupId, QSet<QUniqueId>& groupsRemoved, QSet<QUniqueId>& contactsChanged, QContactManager::Error& error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(QContactManager::Error& error) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error);
    bool removeDetailDefinition(const QString& definitionId, QContactManager::Error& error);

    /* Asynchronous - Request Trampolines */
    void destroyAsynchronousRequest(QContactAbstractRequest* req);
    bool asynchronousRequestWaitForFinished(QContactAbstractRequest* req, int msecs);
    bool asynchronousRequestWaitForProgress(QContactAbstractRequest* req, int msecs);
    void cancelAsynchronousRequest(QContactAbstractRequest* req);
    void startAsynchronousRequest(QContactAbstractRequest* req, QContactAbstractRequest::Operation operation);

    /* Capabilities reporting */
    bool hasFeature(QContactManagerInfo::ManagerFeature feature) const;
    virtual bool filterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;

protected:
    QContactMemoryEngine(const QMap<QString, QString>& parameters);

private slots:
    void performAsynchronousOperation();

private:
    QContactMemoryEngineData* d;
    static QMap<QString, QContactMemoryEngine*> engines;
};

#endif

