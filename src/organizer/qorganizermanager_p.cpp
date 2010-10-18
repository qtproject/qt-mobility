/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizermanager.h"
#include "qorganizermanager_p.h"
#include "qorganizermanagerengine.h"
#include "qorganizermanagerenginefactory.h"

#include "qorganizeritem_p.h"

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

#include "qorganizeritemmemorybackend_p.h"
#include "qorganizeriteminvalidbackend_p.h"
#include "qmobilitypluginsearch.h"

QTM_BEGIN_NAMESPACE

/* Shared QOrganizerManager stuff here, default engine stuff below */
QHash<QString, QOrganizerManagerEngineFactory*> QOrganizerManagerData::m_engines;

bool QOrganizerManagerData::m_discovered;
bool QOrganizerManagerData::m_discoveredStatic;
QStringList QOrganizerManagerData::m_pluginPaths;

static void qOrganizerItemsCleanEngines()
{
    QOrganizerManagerData::m_discovered = false;
    QList<QOrganizerManagerEngineFactory*> factories = QOrganizerManagerData::m_engines.values();
    for (int i=0; i < factories.count(); i++) {
        delete factories.at(i);
    }
    QOrganizerManagerData::m_engines.clear();
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

void QOrganizerManagerData::createEngine(const QString& managerName, const QMap<QString, QString>& parameters)
{
    m_engine = 0;

    QString builtManagerName = managerName.isEmpty() ? QOrganizerManager::availableManagers().value(0) : managerName;
    if (builtManagerName == QLatin1String("memory")) {
        m_engine = QOrganizerItemMemoryEngine::createMemoryEngine(parameters);
    } else {
        int implementationVersion = parameterValue(parameters, QTORGANIZER_IMPLEMENTATION_VERSION_NAME, -1);

        bool found = false;
        bool loadedDynamic = false;

        /* First check static factories */
        loadStaticFactories();

        /* See if we got a fast hit */
        QList<QOrganizerManagerEngineFactory*> factories = m_engines.values(builtManagerName);
        m_error = QOrganizerManager::NoError;

        while(!found) {
            foreach (QOrganizerManagerEngineFactory* f, factories) {
                QList<int> versions = f->supportedImplementationVersions();
                if (implementationVersion == -1 ||//no given implementation version required
                        versions.isEmpty() || //the manager engine factory does not report any version
                        versions.contains(implementationVersion)) {
                    m_engine = f->engine(parameters, &m_error);
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
            m_error = QOrganizerManager::VersionMismatchError;
            m_engine = 0;
        }

        if (!m_engine) {
            if (m_error == QOrganizerManager::NoError)
                m_error = QOrganizerManager::DoesNotExistError;
            m_engine = new QOrganizerItemInvalidEngine();
        }
    }
}


void QOrganizerManagerData::loadStaticFactories()
{
    if (!m_discoveredStatic) {
#if !defined QT_NO_DEBUG
        const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

        m_discoveredStatic = true;

        /* Clean stuff up at the end */
        qAddPostRoutine(qOrganizerItemsCleanEngines);

        /* Loop over all the static plugins */
        QObjectList staticPlugins = QPluginLoader::staticInstances();
        for (int i=0; i < staticPlugins.count(); i++ ){
            QOrganizerManagerEngineFactory *f = qobject_cast<QOrganizerManagerEngineFactory*>(staticPlugins.at(i));
            if (f) {
                QString name = f->managerName();
#if !defined QT_NO_DEBUG
                if (showDebug)
                    qDebug() << "Static: found an engine plugin" << f << "with name" << name;
#endif
                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    // we also need to ensure that we haven't already loaded this factory.
                    if (m_engines.keys().contains(name)) {
                        qWarning() << "Static organizeritems plugin" << name << "has the same name as a currently loaded plugin; ignored";
                    } else {
                        m_engines.insertMulti(name, f);
                    }
                } else {
                    qWarning() << "Static organizeritems plugin with reserved name" << name << "ignored";
                }
            }
        }
    }
}


/* Plugin loader */
void QOrganizerManagerData::loadFactories()
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    // Always do this..
    loadStaticFactories();

    QStringList plugins;
    plugins = mobilityPlugins(QLatin1String("organizer"));

    if (!m_discovered || plugins != m_pluginPaths) {
        m_discovered = true;
        m_pluginPaths = plugins;

        /* Now discover the dynamic plugins */
        for (int i=0; i < m_pluginPaths.count(); i++) {
            QPluginLoader qpl(m_pluginPaths.at(i));
            QOrganizerManagerEngineFactory *f = qobject_cast<QOrganizerManagerEngineFactory*>(qpl.instance());
            if (f) {
                QString name = f->managerName();
#if !defined QT_NO_DEBUG
                if (showDebug)
                    qDebug() << "Dynamic: found a organizer engine plugin" << f << "with name" << name;
#endif
                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    // we also need to ensure that we haven't already loaded this factory.
                    if (m_engines.keys().contains(name)) {
                        qWarning() << "Organizer plugin" << m_pluginPaths.at(i) << "has the same name as currently loaded plugin" << name << "; ignored";
                    } else {
                        m_engines.insertMulti(name, f);
                    }
                } else {
                    qWarning() << "Organizer plugin" << m_pluginPaths.at(i) << "with reserved name" << name << "ignored";
                }
            }

            /* Debugging */
#if !defined QT_NO_DEBUG
            if (showDebug && !f) {
                qDebug() << "Unknown plugin:" << qpl.errorString();
                if (qpl.instance()) {
                    qDebug() << "[qobject:" << qpl.instance() << "]";
                }
            }
#endif
        }

        QStringList engineNames;
        foreach (QOrganizerManagerEngineFactory* f, m_engines.values()) {
            QStringList versions;
            foreach (int v, f->supportedImplementationVersions()) {
                versions << QString::fromAscii("%1").arg(v);
            }
            engineNames << QString::fromAscii("%1[%2]").arg(f->managerName()).arg(versions.join(QString::fromAscii(",")));
        }
#if !defined QT_NO_DEBUG
        if (showDebug) {
            qDebug() << "Found engines:" << engineNames;
        }
#endif
    }
}

/* Caller takes ownership of the id */
QOrganizerItemEngineId* QOrganizerManagerData::createEngineItemId(const QString& managerName, const QMap<QString, QString>& parameters, const QString& engineIdString)
{
    if (managerName == QLatin1String("memory"))
        return new QOrganizerItemMemoryEngineId(engineIdString);

    loadFactories();
    QOrganizerManagerEngineFactory *engineFactory = m_engines.value(managerName);
    return engineFactory ? engineFactory->createItemEngineId(parameters, engineIdString) : NULL;
}

/* Caller takes ownership of the id */
QOrganizerCollectionEngineId* QOrganizerManagerData::createEngineCollectionId(const QString& managerName, const QMap<QString, QString>& parameters, const QString& engineIdString)
{
    if (managerName == QLatin1String("memory"))
        return new QOrganizerCollectionMemoryEngineId(engineIdString);

    loadFactories();
    QOrganizerManagerEngineFactory *engineFactory = m_engines.value(managerName);
    return engineFactory ? engineFactory->createCollectionEngineId(parameters, engineIdString) : NULL;
}

// trampoline for private classes
QOrganizerManagerEngine* QOrganizerManagerData::engine(const QOrganizerManager* manager)
{
    if (manager)
        return manager->d->m_engine;
    return 0;
}

QTM_END_NAMESPACE

