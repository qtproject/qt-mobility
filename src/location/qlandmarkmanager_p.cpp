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

#include "qlandmarkmanager_p.h"
#include "qlandmarkmanagerengine.h"
#include "qlandmarkmanagerenginefactory.h"
#include <QPluginLoader>
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QObject>
#include <QProcess>
#include <QEventLoop>

#include "qmobilitypluginsearch.h"

QTM_USE_NAMESPACE

QLandmarkManagerPrivate::QLandmarkManagerPrivate()
        : engine(0),
        errorCode(QLandmarkManager::NoError)
{
}

void QLandmarkManagerPrivate::createEngine(const QString &managerName, const QMap<QString,QString> &parameters)
{
    Q_Q(QLandmarkManager);

    if (!q->availableManagers().contains(managerName)) {
        errorCode = QLandmarkManager::NotSupportedError;
        errorString = QString("The landmark engine %1 is not supported.").arg(managerName);
        engine = 0;
        return;
    }

    QList<QLandmarkManagerEngineFactory *> factories = QLandmarkManagerPrivate::factories().values(managerName);

    bool ok;
    int implementationVersion = parameters.value(LANDMARKS_IMPLEMENTATION_VERSION_NAME).toInt(&ok);
    if (!ok)
        implementationVersion = -1;

    foreach(QLandmarkManagerEngineFactory *f, factories)
    {
        QList<int> versions = f->supportedImplementationVersions();
            if (implementationVersion == -1 //no given implemenation version required
               || versions.isEmpty() //manager engine factory does not report any versions
               || versions.contains(implementationVersion)) {
                    engine = f->engine(parameters, &errorCode, &errorString);
                    break;
               }
    }

    if (!engine) {
        errorCode = QLandmarkManager::DoesNotExistError;
        errorString = "The requested landmark engine does not exist";
    }

    if (engine) {
        QObject::connect(engine,
                SIGNAL(dataChanged()),
                q,
                SIGNAL(dataChanged()));
        QObject::connect(engine,
                SIGNAL(landmarksAdded(QList<QLandmarkId>)),
                q,
                SIGNAL(landmarksAdded(QList<QLandmarkId>)));
        QObject::connect(engine,
                SIGNAL(landmarksChanged(QList<QLandmarkId>)),
                q,
                SIGNAL(landmarksChanged(QList<QLandmarkId>)));
        QObject::connect(engine,
                SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
                q,
                SIGNAL(landmarksRemoved(QList<QLandmarkId>)));
        QObject::connect(engine,
                SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
                q,
                SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));
        QObject::connect(engine,
                SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
                q,
                SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)));
        QObject::connect(engine,
                SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
                q,
                SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)));
    }
}

 QHash<QString, QLandmarkManagerEngineFactory*>  QLandmarkManagerPrivate::factories(bool reload)
 {
    static QHash<QString, QLandmarkManagerEngineFactory*> factories;
    static bool alreadyDiscovered = false;

    if (reload  == true)
        alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        loadStaticFactories(&factories);
        loadDynamicFactories(&factories);
        alreadyDiscovered = true;
    }
    return factories;
 }

void QLandmarkManagerPrivate::loadDynamicFactories(QHash<QString, QLandmarkManagerEngineFactory*> *factories)
{
    QStringList paths;
    paths << mobilityPlugins(QLatin1String("landmarks"));

    QPluginLoader qpl;
    for (int i=0;i < paths.count(); ++i) {
        qpl.setFileName(paths.at(i));

        QLandmarkManagerEngineFactory *f = qobject_cast<QLandmarkManagerEngineFactory*>(qpl.instance());
        if (f) {
            QString name = f->managerName();

#if !defined QT_NO_DEBUG
            const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
            if (showDebug)
                qDebug() << "Dynamic: found an engine plugin with name" << name;
#endif
            factories->insertMulti(name,f);
        }
    }
}

void QLandmarkManagerPrivate::loadStaticFactories(QHash<QString, QLandmarkManagerEngineFactory*> *factories)
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for(int i=0; i < staticPlugins.count(); ++i) {
        QLandmarkManagerEngineFactory *f = qobject_cast<QLandmarkManagerEngineFactory*>(staticPlugins.at(i));
        if (f){
            QString name = f->managerName();

#if !defined QT_NO_DEBUG
            const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
            if (showDebug)
                qDebug() << "Static: found an engine plugin with name" << name;
#endif
            if(!name.isEmpty()) {
                factories->insertMulti(name, f);
            }
        }

    }
}
