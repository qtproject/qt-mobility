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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONTACTKABCBACKEND_P_H
#define QCONTACTKABCBACKEND_P_H

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

#include <kabc/addressee.h>
#include <kabc/addresseelist.h>
#include <kabc/addressbook.h>
#include <kabc/stdaddressbook.h>

#include <QSharedData>
#include <QFileSystemWatcher>

#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"

class QContactKabcEngineData : public QSharedData
{
public:
    QContactKabcEngineData()
        : QSharedData(),
        m_refCount(QAtomicInt(1)),
        m_settingsWatcher(0),
        m_lastUsedId(0),
        m_standardAddressbook(false),
        ab(0)
    {
    }

    QContactKabcEngineData(const QContactKabcEngineData& other)
        : QSharedData(other),
        m_refCount(QAtomicInt(1)),
        m_resourceFile(other.m_resourceFile),
        m_settingsFile(other.m_settingsFile),
        m_settingsWatcher(0),
        m_definitions(other.m_definitions),
        m_lastUsedId(other.m_lastUsedId),
        m_kabcUidToQContactLocalId(other.m_kabcUidToQContactLocalId),
        m_QContactLocalIdToKabcUid(other.m_QContactLocalIdToKabcUid),
        m_standardAddressbook(other.m_standardAddressbook),
        ab(0)
    {
    }

    ~QContactKabcEngineData()
    {
        if (ab && !m_standardAddressbook)
            delete ab;
        if (m_settingsWatcher)
            delete m_settingsWatcher;
    }

    QAtomicInt m_refCount;
    QString m_resourceFile;
    QString m_settingsFile;
    QFileSystemWatcher* m_settingsWatcher;
    mutable QMap<QString, QContactDetailDefinition> m_definitions;
    mutable QContactLocalId m_lastUsedId;
    mutable QMap<QString, QContactLocalId> m_kabcUidToQContactLocalId;
    mutable QMap<QContactLocalId, QString> m_QContactLocalIdToKabcUid;
    bool m_standardAddressbook;
    KABC::AddressBook *ab;
};

class QTCONTACTS_EXPORT QContactKabcEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    QContactKabcEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
    QContactKabcEngine(const QContactKabcEngine& other);
    ~QContactKabcEngine();
    QContactKabcEngine& operator=(const QContactKabcEngine& other);
    void deref();

    /* Parameter Reporting */
    QMap<QString, QString> parameters() const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QContact contact(const QContactLocalId& contactId, QContactManager::Error& error) const;
    bool saveContact(QContact* contact, QSet<QContactLocalId>& contactsAdded, QSet<QContactLocalId>& contactsChanged, QSet<QContactLocalId>& groupsChanged, QContactManager::Error& error);
    bool removeContact(const QContactLocalId& contactId, QSet<QContactLocalId>& contactsChanged, QSet<QContactLocalId>& groupsChanged, QContactManager::Error& error);

    /* Groups - Accessors and Mutators */
    //QList<QContactLocalId> groups(QContactManager::Error& error) const;
    //QContactGroup group(const QContactLocalId& groupId, QContactManager::Error& error) const;
    //bool saveGroup(QContactGroup* group, QContactManager::Error& error);
    //bool removeGroup(const QContactLocalId& groupId, QContactManager::Error& error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(QContactManager::Error& error) const;
    QContactDetailDefinition detailDefinition(const QString& definitionId, QContactManager::Error& error) const;
    bool saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error);
    bool removeDetailDefinition(const QString& definitionId, QContactManager::Error& error);

    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature) const;
    QList<QVariant::Type> supportedDataTypes() const;

private:
    QContactLocalId getIdOfAddressee(const KABC::Addressee& addressee, QContactManager::Error& error) const;
    QString escaped(const QString& input) const;
    QString convertDetail(const QContactDetail& detail, const QContact& contact) const;
    QContactDetail convertCustomString(const QString& customString) const;
    bool compareDetails(const QContactDetail& fromNative, const QContactDetail& fromCustom) const;
    QString findDuplicate(const QContactDetail& built, const QMap<QString, QContactDetail>& customDetails) const;
    QStringList findActionIdsFromCustomString(const QString& customString) const;
    QContact convertAddressee(const KABC::Addressee& a) const;
    KABC::Addressee convertContact(const QContact& contact) const;

private slots:
    void settingsFileChanged();

private:
    QSharedDataPointer<QContactKabcEngineData> d;
};

class Q_DECL_EXPORT ContactKabcFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
        QString managerName() const;
};

#endif
