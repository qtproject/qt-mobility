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


#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"

#include "qcontact_p.h"

#include "qcontactaction.h"
#include "qcontactactiondescriptor.h"
#include "qcontactactionfactory.h"

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

QTM_BEGIN_NAMESPACE

/* Shared QContactManager stuff here, default engine stuff below */
QList<QContactActionFactory*> QContactManagerData::m_actionfactories; // list of all factories
QList<QContactActionDescriptor> QContactManagerData::m_descriptors;
QHash<QString, QContactManagerEngineFactory*> QContactManagerData::m_engines;
QContactManagerData::DescriptorHash QContactManagerData::m_descriptormap;
QHash<QString, int> QContactManagerData::m_vendormap;
QHash<QString, int> QContactManagerData::m_actionmap;

bool QContactManagerData::m_discovered;
bool QContactManagerData::m_discoveredStatic;
QStringList QContactManagerData::m_pluginPaths;

static void qContactsCleanEngines()
{
    QContactManagerData::m_discovered = false;
    QList<QContactManagerEngineFactory*> factories = QContactManagerData::m_engines.values();
    QList<QContactActionFactory*> actionfactories = QContactManagerData::m_actionfactories;

    for (int i=0; i < factories.count(); i++) {
        delete factories.at(i);
    }
    for(int i=0; i < actionfactories.count(); i++) {
        delete actionfactories.at(i);
    }
    QContactManagerData::m_engines.clear();
    QContactManagerData::m_actionfactories.clear();
    QContactManagerData::m_descriptors.clear();
    QContactManagerData::m_descriptormap.clear();
    QContactManagerData::m_actionmap.clear();
    QContactManagerData::m_vendormap.clear();
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
        m_engine = QContactMemoryEngine::createMemoryEngine(parameters);
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
            QContactActionFactory *g = qobject_cast<QContactActionFactory*>(staticPlugins.at(i));
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

            if (g) {
                QString name = g->name();
#if !defined QT_NO_DEBUG
                if (showDebug)
                    qDebug() << "Static: found an action factory" << g << "with name" << name;
#endif
                if (m_actionfactories.contains(g)) {
                    qWarning() << "Static contacts plugin" << name << "has the same name as currently loaded plugin; ignored";
                } else {
                    m_actionfactories.append(g);

                    QList<QContactActionDescriptor> actions = g->actionDescriptors();
                    QMap<QContactActionDescriptor, QContactActionFactory*>::iterator it;
                    for (int j = 0; j < actions.size(); j++) {
                        QContactActionDescriptor desc = actions.at(j);
                        m_descriptormap.insert(desc, g);
                        m_descriptors.append(desc);
                        m_actionmap.insertMulti(desc.actionName(), m_descriptors.count() - 1);
                        m_vendormap.insertMulti(desc.vendorName(), m_descriptors.count() - 1);
                    }
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

    if (!m_discovered || QApplication::libraryPaths() != m_pluginPaths) {
        m_discovered = true;
        m_pluginPaths = QApplication::libraryPaths();

        /* Discover a bunch o plugins */
        QStringList plugins;

        QStringList paths;
        QSet<QString> processed;

        paths << QApplication::applicationDirPath() << QApplication::libraryPaths();
#if !defined QT_NO_DEBUG
        if (showDebug)
            qDebug() << "Plugin paths:" << paths;
#endif

        /* Enumerate our plugin paths */
        for (int i=0; i < paths.count(); i++) {
            if (processed.contains(paths.at(i)))
                continue;
            processed.insert(paths.at(i));
            QDir pluginsDir(paths.at(i));
            if (!pluginsDir.exists())
                continue;

#if defined(Q_OS_WIN)
            if (pluginsDir.dirName().toLower() == QLatin1String("debug") || pluginsDir.dirName().toLower() == QLatin1String("release"))
                pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
            if (pluginsDir.dirName() == QLatin1String("MacOS")) {
                pluginsDir.cdUp();
                pluginsDir.cdUp();
                pluginsDir.cdUp();
            }
#endif

#if defined(Q_OS_SYMBIAN)
            // In Symbian, going cdUp() in a c:/private/<uid3>/ will result in *platsec* error at fileserver (requires AllFiles capability)
            // Also, trying to cd() to a nonexistent directory causes *platsec* error. This does not cause functional harm, but should
            // nevertheless be changed to use native Symbian methods to avoid unnecessary platsec warnings (as per qpluginloader.cpp).
            RFs rfs;
            qt_symbian_throwIfError(rfs.Connect());
            bool pluginPathFound = false;
            QStringList directories;
            directories << QString("plugins/contacts") << QString("contacts") << QString("../plugins/contacts");
            foreach (const QString& dirName, directories) {
                QString testDirPath = pluginsDir.path() + '/' + dirName;
                testDirPath = QDir::cleanPath(testDirPath);
                // Use native Symbian code to check for directory existence, because checking
                // for files from under non-existent protected dir like E:/private/<uid> using
                // QDir::exists causes platform security violations on most apps.
                QString nativePath = QDir::toNativeSeparators(testDirPath);
                TPtrC ptr = TPtrC16(static_cast<const TUint16*>(nativePath.utf16()), nativePath.length());
                TUint attributes;
                TInt err = rfs.Att(ptr, attributes);
                if (err == KErrNone) {
                    // yes, the directory exists.
                    pluginsDir.cd(testDirPath);
                    pluginPathFound = true;
                    break;
                }
            }
            rfs.Close();
            if (pluginPathFound) {
#else
            if (pluginsDir.cd(QLatin1String("plugins/contacts")) || pluginsDir.cd(QLatin1String("contacts")) || (pluginsDir.cdUp() && pluginsDir.cd(QLatin1String("plugins/contacts")))) {
#endif
                const QStringList& files = pluginsDir.entryList(QDir::Files);
#if !defined QT_NO_DEBUG
                if (showDebug)
                    qDebug() << "Looking for contacts plugins in" << pluginsDir.path() << files;
#endif
                for (int j=0; j < files.count(); j++) {
                    plugins << pluginsDir.absoluteFilePath(files.at(j));
                }
            }
        }

        /* Now discover the dynamic plugins */
        for (int i=0; i < plugins.count(); i++) {
            QPluginLoader qpl(plugins.at(i));
            QContactManagerEngineFactory *f = qobject_cast<QContactManagerEngineFactory*>(qpl.instance());
            QContactActionFactory *g = qobject_cast<QContactActionFactory*>(qpl.instance());

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

            if (g) {
                QString name = g->name();
#if !defined QT_NO_DEBUG
                if (showDebug)
                    qDebug() << "Dynamic: found a contact action factory" << g << "with name" << name;
#endif
                // we also need to ensure that we haven't already loaded this factory.
                if (m_actionfactories.contains(g)) {
                    qWarning() << "Contacts plugin" << plugins.at(i) << "has the same name as currently loaded plugin" << name << "; ignored";
                } else {
                    m_actionfactories.append(g);

                    QList<QContactActionDescriptor> actions = g->actionDescriptors();
                    QMap<QContactActionDescriptor, QContactActionFactory*>::iterator it;
                    for (int j = 0; j < actions.size(); j++) {
                        const QContactActionDescriptor& desc = actions.at(j);
                        m_descriptormap.insert(desc, g);
                        m_descriptors.append(desc);
                        m_actionmap.insertMulti(desc.actionName(), m_descriptors.count() - 1);
                        m_vendormap.insertMulti(desc.vendorName(), m_descriptors.count() - 1);
                    }
                }
            }

            /* Debugging */
#if !defined QT_NO_DEBUG
            if (showDebug && !f && !g) {
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
            qDebug() << "Found actions:" << m_actionmap.keys();
        }
#endif
    }
}

QList<QContactActionDescriptor> QContactManagerData::actionDescriptors(const QString& actionName, const QString& vendorName, int implementationVersion)
{
    loadFactories();

    bool restrict = false;
    QSet<int> subset;
    QList<QContactActionDescriptor> descriptors;

    // Go through our list of descriptors, looking for a match
    if (!actionName.isEmpty()) {
        subset = m_actionmap.values(actionName).toSet();
        restrict = true;
    }

    if (!vendorName.isEmpty()) {
        if (restrict)
            subset &= m_vendormap.values(vendorName).toSet();
        else
            subset = m_vendormap.values(vendorName).toSet();
        restrict = true;

        /* We still have to check versions, since we don't hash that */
        if (implementationVersion != -1) {
            QMutableSetIterator<int> it(subset);
            while(it.hasNext()) {
                if (m_descriptors.at(it.next()).implementationVersion() != implementationVersion)
                    it.remove();
            }
        }
    }

    if (restrict) {
        QSetIterator<int> it(subset);
        while(it.hasNext()) {
            descriptors << m_descriptors.at(it.next());
        }
    } else {
        /* No restrictions, just iterate over all descriptors and return all actions (!) */
        descriptors = m_descriptors;
    }

    return descriptors;
}

QContactAction* QContactManagerData::action(const QContactActionDescriptor& actionDescriptor)
{
    loadFactories();
    QContactActionFactory* actionFactory = m_descriptormap.value(actionDescriptor, 0);
    if (actionFactory)
        return actionFactory->instance(actionDescriptor);
    return 0;
}

// trampoline for private classes
QContactManagerEngine* QContactManagerData::engine(const QContactManager* manager)
{
    if (manager)
        return manager->d->m_engine;
    return 0;
}

QTM_END_NAMESPACE

