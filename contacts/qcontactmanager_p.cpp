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


#include "qcontactmanager.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"

#include "qcontact_p.h"

#include "qcontactaction.h"
#include "qcontactactionfactory.h"

#include <QSharedData>
#include <QtPlugin>
#include <QPluginLoader>

#include <QDebug>
#include <QDir>

#include <QApplication>

#include "memory/qcontactmemorybackend_p.h"
#include "invalid/qcontactinvalidbackend_p.h"


/* Shared QContactManager stuff here, default engine stuff below */
QList<QContactActionFactory*> QContactManagerData::m_actionfactories; // list of all factories
QList<QContactActionFactory::ActionDescriptor> QContactManagerData::m_descriptors;
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

void QContactManagerData::createEngine(const QString& managerName, const QMap<QString, QString>& parameters)
{
    m_managerName = managerName.isEmpty() ? QContactManager::availableManagers().value(0) : managerName;

    if (m_managerName == QLatin1String("memory"))
        m_engine = QContactMemoryEngine::createMemoryEngine(parameters);
    else {
        /* Look for a factory */
        loadFactories();
        QContactManagerEngineFactory *factory = m_engines.value(managerName);
        m_error = QContactManager::NoError;
        if (factory)
            m_engine = factory->engine(parameters, m_error);
        else
            m_engine = 0;

        if (!m_engine) {
            m_managerName = QLatin1String("invalid");
            if (m_error == QContactManager::NoError)
                m_error = QContactManager::DoesNotExistError;
            m_engine = new QContactInvalidEngine(); // XXX share
        }
    }
    m_params = parameters;
    m_uri = QContactManager::buildUri(m_managerName, parameters);
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
            if (f) {
                QString name = f->managerName();
                qDebug() << "Static: found an engine plugin" << f << "with name" << name;
                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    if(!m_engines.contains(name)) {
                        m_engines.insert(name, f);
                    } else {
                        qWarning() << "Duplicate Contacts static plugin with id" << name;
                    }
                } else {
                    qWarning() << "Static contacts plugin with reserved name" << name << "ignored";
                }
            }

            if (g) {
                QString name = g->name();
                qDebug() << "Static: found an action factory" << g << "with name" << name;

                m_actionfactories.append(g);

                QList<QContactActionFactory::ActionDescriptor> actions = g->actionDescriptors();
                QMap<QContactActionFactory::ActionDescriptor, QContactActionFactory*>::iterator it;
                for (int j = 0; j < actions.size(); j++) {
                    const QContactActionFactory::ActionDescriptor& desc = actions.at(j);
                    m_descriptormap.insert(desc, g);
                    m_descriptors.append(desc);
                    m_actionmap.insertMulti(desc.actionName, m_descriptors.count() - 1);
                    m_vendormap.insertMulti(desc.vendorName, m_descriptors.count() - 1);
                }
            }
        }

        QStringList paths;

        paths << QApplication::applicationDirPath() << QApplication::libraryPaths();

        /* XXX a bit of a hack */
#if defined(Q_OS_UNIX)
        if (qgetenv("LD_LIBRARY_PATH").size() > 0)
            paths << QString::fromLocal8Bit(qgetenv("LD_LIBRARY_PATH"));
#elif defined(Q_OS_WIN)
        if (qgetenv("PATH").size() > 0)
            paths << QString::fromLocal8Bit(qgetenv("PATH")).split(QLatin1Char(';'));
#endif

        /* Code from docs.. */
        for (int i=0; i < paths.count(); i++) {
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
            if (pluginsDir.cd(QLatin1String("plugins")) || (pluginsDir.cdUp() && pluginsDir.cd(QLatin1String("plugins")))) {
                const QStringList& files = pluginsDir.entryList(QDir::Files);
                qDebug() << "Looking for plugins in" << pluginsDir.path() << files;
                for (int j=0; j < files.count(); j++) {
                    plugins << pluginsDir.absoluteFilePath(files.at(j));
                }
            }
        }

        /* Now discover some other plugins */
        for (int i=0; i < plugins.count(); i++) {
            QPluginLoader qpl(plugins.at(i));
            QContactManagerEngineFactory *f = qobject_cast<QContactManagerEngineFactory*>(qpl.instance());
            QContactActionFactory *g = qobject_cast<QContactActionFactory*>(qpl.instance());
            if (f) {
                QString name = f->managerName();
                qDebug() << "Dynamic: found an engine plugin" << f << "with name" << name;

                if (name != QLatin1String("memory") && name != QLatin1String("invalid") && !name.isEmpty()) {
                    if(!m_engines.contains(name)) {
                        m_engines.insert(name, f);
                    } else {
                        qWarning() << "Duplicate Contacts plugin with id" << name;
                    }
                } else {
                    qWarning() << "Contacts plugin" << plugins.at(i) << "with reserved name" << name << "ignored";
                }
            }

            if (g) {
                QString name = g->name();
                qDebug() << "Dynamic: found an action factory" << g << "with name" << name;

                m_actionfactories.append(g);

                QList<QContactActionFactory::ActionDescriptor> actions = g->actionDescriptors();
                QMap<QContactActionFactory::ActionDescriptor, QContactActionFactory*>::iterator it;
                for (int j = 0; j < actions.size(); j++) {
                    const QContactActionFactory::ActionDescriptor& desc = actions.at(j);
                    m_descriptormap.insert(desc, g);
                    m_descriptors.append(desc);
                    m_actionmap.insertMulti(desc.actionName, m_descriptors.count() - 1);
                    m_vendormap.insertMulti(desc.vendorName, m_descriptors.count() - 1);
                }
            }
        }

        qDebug() << "Found engines:" << m_engines.keys();
        qDebug() << "Found actions:" << m_actionmap.keys();
    }
}

QList<QContactAction*> QContactManagerData::actions(const QString& actionName, const QString& vendor, int implementationVersion)
{
    QList<QContactAction*> retn;

    loadFactories();

    bool restrict = false;
    QSet<int> subset;
    QList<QContactActionFactory::ActionDescriptor> descriptors;

    // Go through our list of descriptors, looking for a match
    if (!actionName.isEmpty()) {
        subset = m_actionmap.values(actionName).toSet();
        restrict = true;
    }

    if (!vendor.isEmpty()) {
        if (restrict)
            subset &= m_vendormap.values(vendor).toSet();
        else
            subset = m_vendormap.values(vendor).toSet();
        restrict = true;

        /* We still have to check versions, since we don't hash that */
        if (implementationVersion != -1) {
            QMutableSetIterator<int> it(subset);
            while(it.hasNext()) {
                if (m_descriptors.at(it.next()).vendorVersion != implementationVersion)
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

    /* Now loop over the valid descriptors */
    for (int j=0; j < descriptors.size(); j++) {
        const QContactActionFactory::ActionDescriptor& descriptor = descriptors.at(j);
        retn += m_descriptormap.value(descriptor)->instance(descriptor);
    }

    return retn;
}

// trampoline to engine for asynchronous requests
QContactManagerEngine* QContactManagerData::engine(const QContactManager* manager)
{
    return manager->d->m_engine;
}

