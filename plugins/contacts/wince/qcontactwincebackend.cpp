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
#include "qcontactwincebackend_p.h"

#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

#include <QDebug>

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

QContactWinCEEngine::QContactWinCEEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
    : d(new QContactWinCEEngineData)
{
    error = QContactManager::NoError;

    if (SUCCEEDED(CoInitializeEx( NULL, 0))) {
        qDebug() << "Initialized COM";
        if (SUCCEEDED(CoCreateInstance(CLSID_Application, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IPOutlookApp2,
                                       reinterpret_cast<void **>(&d->m_app)))) {
            qDebug() << "Created IPOutlookApp2";
            if(FAILED(d->m_app->Logon(NULL)))
            {
                qDebug() << "Failed to log on";
                d->m_app->Release();
                d->m_app = 0;
            } else {
                qDebug() << "logged on ok";
            }
        }
    }
}

QContactWinCEEngine::QContactWinCEEngine(const QContactWinCEEngine& other)
    : QContactManagerEngine(), d(other.d)
{

}

QContactWinCEEngine& QContactWinCEEngine::operator=(const QContactWinCEEngine& other)
{
    // assign
    d = other.d;

    return *this;
}

QContactWinCEEngine::~QContactWinCEEngine()
{
}

void QContactWinCEEngine::deref()
{
    if (!d->m_refCount.deref())
        delete this;
}


QList<QUniqueId> QContactWinCEEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QList<QUniqueId> allCIds;
    error = QContactManager::NoError;

    // return the list sorted according to sortOrders
    QContactManager::Error sortError;
    QList<QContact> sorted;
    QList<QUniqueId> sortedIds;
    for (int i = 0; i < allCIds.size(); i++)
        QContactManagerEngine::addSorted(&sorted, contact(allCIds.at(i), sortError), sortOrders);
    for (int i = 0; i < sorted.size(); i++)
        sortedIds.append(sorted.at(i).id());

    return sortedIds;
}

QContact QContactWinCEEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    error = QContactManager::NoError;


    error = QContactManager::DoesNotExistError;
    return QContact();
}

bool QContactWinCEEngine::saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    if (contact == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        error = QContactManager::InvalidDetailError;
        return false;
    }


    // success!
    error = QContactManager::NoError;
    return true;
}

bool QContactWinCEEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    error = QContactManager::DoesNotExistError;
    return false;
}


QMap<QString, QContactDetailDefinition> QContactWinCEEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    return QMap<QString, QContactDetailDefinition>();
}

QContactDetailDefinition QContactWinCEEngine::detailDefinition(const QString& definitionName, QContactManager::Error& error) const
{
    error = QContactManager::DoesNotExistError;
    return QContactDetailDefinition();
}

bool QContactWinCEEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    if (!validateDefinition(def, error)) {
        return false;
    }
    error = QContactManager::NoError;
    return true;
}

bool QContactWinCEEngine::removeDetailDefinition(const QString& definitionId, QContactManager::Error& error)
{
    if (definitionId.isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    error = QContactManager::DoesNotExistError;
    return false;
}

/*! \reimp */
bool QContactWinCEEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManagerInfo::Batch:
        case QContactManagerInfo::ActionPreferences:
        case QContactManagerInfo::ReadOnlyDetails:
        case QContactManagerInfo::CreateOnlyDetails:
        case QContactManagerInfo::MutableDefinitions:
        case QContactManagerInfo::Synchronous:
            return true;
        default:
            return false;
    }
}

/*!
 * Returns the list of data types supported by the WinCE engine
 */
QList<QVariant::Type> QContactWinCEEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}


/* Factory lives here in the basement */
QContactManagerEngine* ContactWinceFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new QContactWinCEEngine(parameters, error);
}

QString ContactWinceFactory::managerName() const
{
    return QString("wince");
}
Q_EXPORT_PLUGIN2(contacts_wince, ContactWinceFactory);
