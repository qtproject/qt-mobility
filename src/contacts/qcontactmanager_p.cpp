/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"
#include "qcontactmanagerenginev2wrapper_p.h"

#include "qcontact_p.h"

#include "qcontactaction.h"
#include "qcontactactiondescriptor.h"
#ifdef QT_SIMULATOR
#include "qcontactsimulatorbackend_p.h"
#endif

#include <QSharedData>
#include <QtPlugin>
#include <QPluginLoader>

#include <QDebug>
#include <QDir>
#include <QFile>

#include <QApplication>


#if defined(Q_OS_SYMBIAN)
# include <f32file.h>
#endif

#include "qcontactmemorybackend_p.h"
#include "qcontactinvalidbackend_p.h"
#include "qmobilitypluginsearch.h"

QTM_BEGIN_NAMESPACE

/* Shared QContactManager stuff here, default engine stuff below */
QHash<QString, QContactManagerEngineFactory*> QContactManagerData::m_engines;
QSet<QContactManager*> QContactManagerData::m_aliveEngines;
QList<QContactActionManagerPlugin*> QContactManagerData::m_actionManagers;

bool QContactManagerData::m_discoveredStatic;
QStringList QContactManagerData::m_pluginPaths;

static void qContactsCleanEngines()
{
    // This is complicated by needing to remove any engines before we unload factories
    foreach(QContactManager* manager, QContactManagerData::m_aliveEngines) {
        // We don't delete the managers here, we just kill their engines
        // and replace it with an invalid engine (for safety :/)
        QContactManagerData* d = QContactManagerData::managerData(manager);
        delete d->m_engine;
        d->m_engine = new QContactInvalidEngine();
    }

    QList<QContactManagerEngineFactory*> factories = QContactManagerData::m_engines.values();

    for (int i=0; i < factories.count(); i++) {
        delete factories.at(i);
    }
    QContactManagerData::m_engines.clear();
    QContactManagerData::m_actionManagers.clear();
    QContactManagerData::m_aliveEngines.clear();
}

static int parameterValue(const QMap<QString, QString>& parameters, const char* key, int defaultValue)
{
    if (parameters.contains(QString::fromAscii(key))) {
        bool ok;
        int version = parameters.value(QString::fromAscii(key)).toInt(&ok);
        
        if (ok)
            return version;
    }
    return defaultValue;
}

void QContactManagerData::createEngine(const QString& managerName, const QMap<QString, QString>& parameters)
{
    m_engine = 0;

    QString builtManagerName = managerName.isEmpty() ? QContactManager::availableManagers().value(0) : managerName;
    if (builtManagerName == QLatin1String("memory")) {
        m_engine = new QContactManagerEngineV2Wrapper(QContactMemoryEngine::createMemoryEngine(parameters));
#ifdef QT_SIMULATOR
    } else if (builtManagerName == QLatin1String("simulator")) {
        m_engine = new QContactManagerEngineV2Wrapper(QContactSimulatorEngine::createSimulatorEngine(parameters));
#endif
    } else {
        int implementationVersion = parameterValue(parameters, QTCONTACTS_IMPLEMENTATION_VERSION_NAME, -1);

        bool found = false;
        bool loadedDynamic = false;

        /* First check static factories */
        loadStaticFactories();

        /* See if we got a fast hit */
        QList<QContactManagerEngineFactory*> factories = m_engines.values(builtManagerName);
        m_error = QContactManager::NoError;

        while(!found) {
            foreach (QContactManagerEngineFactory* f, factories) {
                QList<int> versions = f->supportedImplementationVersions();
                if (implementationVersion == -1 ||//no given implementation version required
                        versions.isEmpty() || //the manager engine factory does not report any version
                        versions.contains(implementationVersion)) {
                    QContactManagerEngine* engine = f->engine(parameters, &m_error);
                    // if it's a V2, use it
                    m_engine = qobject_cast<QContactManagerEngineV2*>(engine);
                    if (!m_engine && engine) {
                        // Nope, v1, so wrap it
                        m_engine = new QContactManagerEngineV2Wrapper(engine);
                    }
                    found = true;
                    break;
                }
            }
            
            // Break if found or if this is the second time through
            if (loadedDynamic || found)
                break;

            // otherwise load dynamic factories and reloop
            loadFactories();
            factories = m_engines.values(builtManagerName);
            loadedDynamic = true;
        }

        // XXX remove this
        // the engine factory could lie to us, so check the real implementation version
        if (m_engine && (implementationVersion != -1 && m_engine->managerVersion() != implementationVersion)) {
            m_error = QContactManager::VersionMismatchError;
            m_engine = 0;
        }

        if (!m_engine) {
            if (m_error == QContactManager::NoError)
                m_error = QContactManager::DoesNotExistError;
            m_engine = new QContactInvalidEngine();
        }
    }
}


void QContactManagerData::loadStaticFactories()
{
    if (!m_discoveredStatic) {
#if !defined QT_NO_DEBUG
        const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

        m_discoveredStatic = true;

        /* Clean stuff up at the end */
        qAddPostRoutine(qContactsCleanEngines);

        /* Loop over all the static plugins */
        QObjectList staticPlugins = QPluginLoader::staticInstances();
        for (int i=0; i < staticPlugins.count(); i++ ){
            QContactManagerEngineFactory *f = qobject_cast<QContactManagerEngineFactory*>(staticPlugins.at(i));
            if (f) {
                QString name = f->managerName();
#if !defined QT_NO_DEBUG
                if (showDebug)
                    qDebug() << "Static: found an engine plugin" << f << "with name" << name;
#endif
                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    // we also need to ensure that we haven't already loaded this factory.
                    if (m_engines.keys().contains(name)) {
                        qWarning() << "Static contacts plugin" << name << "has the same name as a currently loaded plugin; ignored";
                    } else {
                        m_engines.insertMulti(name, f);
                    }
                } else {
                    qWarning() << "Static contacts plugin with reserved name" << name << "ignored";
                }
            }
        }
    }
}


/* Plugin loader */
void QContactManagerData::loadFactories()
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    // Always do this..
    loadStaticFactories();

    // But only load dynamic plugins when the paths change
    QStringList paths = QCoreApplication::libraryPaths();
#ifdef QTM_PLUGIN_PATH
    paths << QLatin1String(QTM_PLUGIN_PATH);
#endif

    if (paths != m_pluginPaths) {
        m_pluginPaths = paths;

        QStringList plugins = mobilityPlugins(QLatin1String("contacts"));

        /* Now discover the dynamic plugins */
        for (int i=0; i < plugins.count(); i++) {
            QPluginLoader qpl(plugins.at(i));

#if !defined QT_NO_DEBUG
            if (showDebug)
                qDebug() << "Loading plugin" << plugins.at(i);
#endif

            QContactManagerEngineFactory *f = qobject_cast<QContactManagerEngineFactory*>(qpl.instance());
            QContactActionManagerPlugin *m = qobject_cast<QContactActionManagerPlugin*>(qpl.instance());

            if (f) {
                QString name = f->managerName();
#if !defined QT_NO_DEBUG
                if (showDebug)
                    qDebug() << "Dynamic: found a contact engine plugin" << f << "with name" << name;
#endif
                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    // we also need to ensure that we haven't already loaded this factory.
                    if (m_engines.keys().contains(name)) {
                        qWarning() << "Contacts plugin" << plugins.at(i) << "has the same name as currently loaded plugin" << name << "; ignored";
                    } else {
                        m_engines.insertMulti(name, f);
                    }
                } else {
                    qWarning() << "Contacts plugin" << plugins.at(i) << "with reserved name" << name << "ignored";
                }
            }

            if (m) {
                m_actionManagers.append(m);
            }

            /* Debugging */
#if !defined QT_NO_DEBUG
            if (showDebug && !f && !m) {
                qDebug() << "Unknown plugin:" << qpl.errorString();
                if (qpl.instance()) {
                    qDebug() << "[qobject:" << qpl.instance() << "]";
                }
            }
#endif
        }
        
        QStringList engineNames;
        foreach (QContactManagerEngineFactory* f, m_engines.values()) {
            QStringList versions;
            foreach (int v, f->supportedImplementationVersions()) {
                versions << QString::fromAscii("%1").arg(v);
            }
            engineNames << QString::fromAscii("%1[%2]").arg(f->managerName()).arg(versions.join(QString::fromAscii(",")));
        }
#if !defined QT_NO_DEBUG
        if (showDebug) {
            qDebug() << "Found engines:" << engineNames;
            qDebug() << "Found action engines:" << m_actionManagers;
        }
#endif
    }
}

// trampoline for private classes
QContactManagerEngineV2* QContactManagerData::engine(const QContactManager* manager)
{
    if (manager)
        return manager->d->m_engine;
    return 0;
}

QTM_END_NAMESPACE

