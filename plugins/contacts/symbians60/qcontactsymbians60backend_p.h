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

#ifndef QCONTACTSymbianS60BACKEND_P_H
#define QCONTACTSymbianS60BACKEND_P_H

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

#include "qtcontactsglobal.h"
#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"

class QContactSymbianS60EngineData : public QSharedData
{
public:
    QContactSymbianS60EngineData()
        : QSharedData(),
        m_refCount(QAtomicInt(1))
    {
    }

    QContactSymbianS60EngineData(const QContactSymbianS60EngineData& other)
        : QSharedData(other),
        m_refCount(QAtomicInt(1))
    {
    }

    ~QContactSymbianS60EngineData()
    {
    }

    QAtomicInt m_refCount;
};

class QContactSymbianS60Engine : public QContactManagerEngine
{
    Q_OBJECT

public:
    QContactSymbianS60Engine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
    QContactSymbianS60Engine(const QContactSymbianS60Engine& other);
    ~QContactSymbianS60Engine();
    QContactSymbianS60Engine& operator=(const QContactSymbianS60Engine& other);
    void deref();

    /* Contacts - Accessors and Mutators */
    QList<QUniqueId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QContact contact(const QUniqueId& contactId, QContactManager::Error& error) const;
    bool saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error);
    bool removeContact(const QUniqueId& contactId, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error);

    /* Groups - Accessors and Mutators */
    //QList<QUniqueId> groups(QContactManager::Error& error) const;
    //QContactGroup group(const QUniqueId& groupId, QContactManager::Error& error) const;
    //bool saveGroup(QContactGroup* group, QContactManager::Error& error);
    //bool removeGroup(const QUniqueId& groupId, QContactManager::Error& error);

    /* Definitions */
    QMap<QString, QContactDetailDefinition> detailDefinitions(QContactManager::Error& error) const;

    /* Capabilities reporting */
    bool hasFeature(QContactManagerInfo::ManagerFeature feature) const;
    QList<QVariant::Type> supportedDataTypes() const;

    /* Synthesise the display label of a contact */
    virtual QString synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const;

private:
    QSharedDataPointer<QContactSymbianS60EngineData> d;
};

class Q_DECL_EXPORT ContactSymbianS60Factory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
        QString managerName() const;
};

#endif
