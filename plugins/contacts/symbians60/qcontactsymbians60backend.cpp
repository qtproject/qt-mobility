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
#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

#include <QDebug>


#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

#include "qcontactsymbians60backend_p.h"

QContactSymbianS60Engine::QContactSymbianS60Engine(const QMap<QString, QString>& , QContactManager::Error& error)
    : d(new QContactSymbianS60EngineData)
{
    error = QContactManager::NoError;
}

QContactSymbianS60Engine::QContactSymbianS60Engine(const QContactSymbianS60Engine& other)
    : QContactManagerEngine(), d(other.d)
{

}

QContactSymbianS60Engine& QContactSymbianS60Engine::operator=(const QContactSymbianS60Engine& other)
{
    // assign
    d = other.d;

    return *this;
}

QContactSymbianS60Engine::~QContactSymbianS60Engine()
{
}

void QContactSymbianS60Engine::deref()
{
    if (!d->m_refCount.deref())
        delete this;
}


QList<QUniqueId> QContactSymbianS60Engine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
}

QContact QContactSymbianS60Engine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
}

bool QContactSymbianS60Engine::saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
}


bool QContactSymbianS60Engine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& , QContactManager::Error& error)
{
    return false;
}


QMap<QString, QContactDetailDefinition> QContactSymbianS60Engine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    return QContactManagerEngine::schemaDefinitions();
}

bool QContactSymbianS60Engine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManagerInfo::ReadOnlyDetails:
        case QContactManagerInfo::Synchronous:
            return true;
        default:
            return false;
    }
}

/* Synthesise the display label of a contact */
QString QContactSymbianS60Engine::synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    QContactName name = contact.detail<QContactName>();
    QContactOrganisation org = contact.detail<QContactOrganisation>();

    if (!name.last().isEmpty()) {
        if (!name.first().isEmpty()) {
            return QString(QLatin1String("%1, %2")).arg(name.last()).arg(name.first());
        } else {
            // Just last
            return name.last();
        }
    } else if (!name.first().isEmpty()) {
        return name.first();
    } else if (!org.displayLabel().isEmpty()) {
        return org.displayLabel();
    } else {
        // XXX grargh.
        return QLatin1String("Unnamed");
    }
}


/*!
 * Returns the list of data types supported by the Symbian S60 engine
 */
QList<QVariant::Type> QContactSymbianS60Engine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
   /* st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::Double);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);*/

    return st;
}


/* Factory lives here in the basement */
QContactManagerEngine* ContactSymbianS60Factory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new QContactSymbianS60Engine(parameters, error);
}

QString ContactSymbianS60Factory::managerName() const
{
    return QString("symbians60");
}
Q_EXPORT_PLUGIN2(contacts_symbians60, ContactSymbianS60Factory);
