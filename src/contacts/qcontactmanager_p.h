/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTMANAGER_P_H
#define QCONTACTMANAGER_P_H

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

#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>

#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactactionmanager_p.h"
#include "qcontactobserver.h"

QTM_USE_NAMESPACE
QTM_BEGIN_NAMESPACE

class QContactManagerEngineFactory;

/* Data and stuff that is shared amongst all backends */
class QContactManagerData
{
public:
    QContactManagerData()
        : m_engine(0),
        m_lastError(QContactManager::NoError)
    {
    }

    ~QContactManagerData()
    {
        delete m_engine;
    }


    void createEngine(const QString& managerName, const QMap<QString, QString>& parameters);
    static QContactManagerData* get(const QContactManager* manager);
    static QContactManagerEngineV2* engine(const QContactManager* manager);

    QContactManagerEngineV2* m_engine;
    QContactManager::Error m_lastError;
    QMap<int, QContactManager::Error> m_lastErrorMap;

    /* Manager plugins */
    static QHash<QString, QContactManagerEngineFactory*> m_engines;
    static QSet<QContactManager*> m_aliveEngines;
    static QContactManagerData* managerData(QContactManager* manager) {return manager->d;}
    static QContactManagerData* managerData(const QContactManager* manager) {return manager->d;} // laziness to avoid const_cast
    static QList<QContactActionManagerPlugin*> m_actionManagers;
    static bool m_discoveredStatic;
    static QStringList m_pluginPaths;
    static void loadFactories();
    static void loadStaticFactories();

    // Observer stuff
    static void registerObserver(QContactManager *m, QContactObserver* observer);
    static void unregisterObserver(QContactManager *m, QContactObserver* observer);
    void _q_contactsUpdated(const QList<QContactLocalId>& ids);
    void _q_contactsDeleted(const QList<QContactLocalId>& ids);

    QMultiHash<QContactLocalId, QContactObserver*> m_observerForContact;

    // Lazy connection stuff - reference counted set
    QHash<QByteArray, int> m_connectedSignals;

private:
    Q_DISABLE_COPY(QContactManagerData)
};

/*
    Helper to hold the error state of a synchronous operation - when destructed, updates the
    manager's last error variables to the result of this operation.  This means that during
    callbacks the error state can't be modified behind the engines back. and it's more conceptually
    correct.
 */
class QContactManagerSyncOpErrorHolder
{
public:
    QContactManagerSyncOpErrorHolder(const QContactManager* m, QMap<int, QContactManager::Error> *pUserError = 0)
        : error(QContactManager::NoError),
        data(QContactManagerData::managerData(m)),
        userError(pUserError)
    {
    }

    ~QContactManagerSyncOpErrorHolder()
    {
        data->m_lastError = error;
        data->m_lastErrorMap = errorMap;
        if (userError)
            *userError = errorMap;
    }

    QContactManager::Error error;
    QContactManagerData* data;
    QMap<int, QContactManager::Error> errorMap;
    QMap<int, QContactManager::Error> *userError;
};

QTM_END_NAMESPACE

#endif
