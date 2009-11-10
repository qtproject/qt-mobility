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

#include <QApplication>

#include "qcontactmemorybackend_p.h"
#include "qcontactinvalidbackend_p.h"


/* Shared QContactManager stuff here, default engine stuff below */
QList<QContactActionFactory*> QContactManagerData::m_actionfactories; // list of all factories
QList<QContactActionDescriptor> QContactManagerData::m_descriptors;
QHash<QString, QContactManagerEngineFactory*> QContactManagerData::m_engines;
QContactManagerData::DescriptorHash QContactManagerData::m_descriptormap;
QHash<QString, int> QContactManagerData::m_vendormap;
QHash<QString, int> QContactManagerData::m_actionmap;

bool QContactManagerData::m_discovered;

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
    int apiVersion = parameterValue(parameters, QTCONTACTS_VERSION_NAME, QContactManager::version());
    m_engine = 0;

    if (apiVersion != QContactManager::version()) {
        m_error = QContactManager::VersionMismatchError;
        m_engine = new QContactInvalidEngine(); // XXX share
        return;
    }

    QString builtManagerName = managerName.isEmpty() ? QContactManager::availableManagers().value(0) : managerName;
    if (builtManagerName == QLatin1String("memory"))
        m_engine = QContactMemoryEngine::createMemoryEngine(parameters);
    else {
        int implementationVersion = parameterValue(parameters, QTCONTACTS_IMPLEMENTATION_VERSION_NAME, -1);
        /* Look for a factory */
        loadFactories();
        QList<QContactManagerEngineFactory*> factories = m_engines.values(builtManagerName);
        m_error = QContactManager::NoError;

        
        foreach (QContactManagerEngineFactory* f, factories) {
            QList<int> versions = f->supportedImplementationVersions();
            if (f && f->version() == apiVersion) {
                if (implementationVersion == -1 ||//no given implementation version required
                    versions.isEmpty() || //the manager engine factory does not report any version
                    versions.contains(implementationVersion)) {
                    m_engine = f->engine(parameters, m_error);
                    break;
                }
            }
        }

        // the engine factory could lie to us, so check the real implementation version
        if (m_engine->version() != apiVersion || (implementationVersion != -1 && m_engine->implementationVersion() != implementationVersion)) {
            m_error = QContactManager::VersionMismatchError;
            m_engine = 0;
        }

        if (!m_engine) {
            if (m_error == QContactManager::NoError)
                m_error = QContactManager::DoesNotExistError;
            m_engine = new QContactInvalidEngine(); // XXX share
        }
    }
}

/* Plugin loader */
void QContactManagerData::loadFactories()
{
    if (!m_discovered) {
        m_discovered = true;

        /* Clean stuff up at the end */
        qAddPostRoutine(qContactsCleanEngines);

        /* Discover a bunch o plugins */
        QStringList plugins;

        /* First the static ones */
        QObjectList staticPlugins = QPluginLoader::staticInstances();
        for (int i=0; i < staticPlugins.count(); i++ ){
            QContactManagerEngineFactory *f = qobject_cast<QContactManagerEngineFactory*>(staticPlugins.at(i));
            QContactActionFactory *g = qobject_cast<QContactActionFactory*>(staticPlugins.at(i));
            if (f && f->version() == QContactManager::version()) {
                QString name = f->managerName();
                qDebug() << "Static: found an engine plugin" << f << "with name" << name;
                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    m_engines.insertMulti(name, f);
                } else {
                    qWarning() << "Static contacts plugin with reserved name" << name << "ignored";
                }
            }

            if (g) {
                QString name = g->name();
                qDebug() << "Static: found an action factory" << g << "with name" << name;

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

        QStringList paths;
        QSet<QString> processed;

        paths << QApplication::applicationDirPath() << QApplication::libraryPaths();

        qDebug() << "Plugin paths:" << paths;
        /* Enumerate our plugin paths */
        for (int i=0; i < paths.count(); i++) {
            if (processed.contains(paths.at(i)))
                continue;
            processed.insert(paths.at(i));
            QDir pluginsDir(paths.at(i));
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
            if (pluginsDir.cd(QLatin1String("plugins/contacts")) || (pluginsDir.cdUp() && pluginsDir.cd(QLatin1String("plugins/contacts")))) {
                const QStringList& files = pluginsDir.entryList(QDir::Files);
                qDebug() << "Looking for plugins in" << pluginsDir.path() << files;
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
                qDebug() << "Dynamic: found an engine plugin" << f << "with name" << name;

                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    m_engines.insertMulti(name, f);
                } else {
                    qWarning() << "Contacts plugin" << plugins.at(i) << "with reserved name" << name << "ignored";
                }
            }

            if (g) {
                QString name = g->name();
                qDebug() << "Dynamic: found an action factory" << g << "with name" << name;

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

            /* Debugging */
            if (!f && !g) {
                qDebug() << "Unknown plugin:" << qpl.errorString() << " [qobject:" << qpl.instance() << "]";
            }
        }

        
        QStringList engineNames;
        foreach (QContactManagerEngineFactory* f, m_engines.values()) {
            QStringList versions;
            foreach (int v, f->supportedImplementationVersions()) {
                versions << QString::fromAscii("%1.%2").arg(f->version()).arg(v);
            }
            engineNames << QString::fromAscii("%1[%2]").arg(f->managerName()).arg(versions.join(QString::fromAscii(",")));
        }
        qDebug() << "Found engines:" << engineNames;
        qDebug() << "Found actions:" << m_actionmap.keys();
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



