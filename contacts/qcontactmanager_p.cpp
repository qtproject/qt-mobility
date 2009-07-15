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

#include "qcontactabstractaction.h"
#include "qcontactabstractactionfactory.h"

#include <QSharedData>
#include <QtPlugin>
#include <QPluginLoader>

#include <QDebug>
#include <QDir>

#include <QApplication>

#include "memory/qcontactmemorybackend_p.h"
#include "invalid/qcontactinvalidbackend_p.h"


/* Shared QContactManager stuff here, default engine stuff below */
QMultiMap<QString, QContactAbstractAction*> QContactManagerData::m_actionImplementations;
QMap<QString, QContactManagerEngineFactory*> QContactManagerData::m_engines;
bool QContactManagerData::m_discovered;

static void qContactsCleanEngines()
{
    QContactManagerData::m_discovered = false;
    QList<QContactManagerEngineFactory*> factories = QContactManagerData::m_engines.values();
    for (int i=0; i < factories.count(); i++) {
        delete factories.at(i);
    }
    QContactManagerData::m_engines.clear();
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
            QContactAbstractActionFactory *g = qobject_cast<QContactAbstractActionFactory*>(staticPlugins.at(i));
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
            } else if (g) {
                QString name = g->name();
                qDebug() << "Static: found an action factory" << g << "with name" << name;

                // option one: we own the action implementations; clients DO NOT delete them...
                QList<QContactAbstractAction*> impls = g->instances();
                for (int j = 0; j < impls.size(); j++) {
                    QContactAbstractAction* impl = impls.at(j);
                    m_actionImplementations.insert(impl->actionName(), impl);
                }

                // option two: we own the factory instances; clients own the action implementation instances!
                // m_actionFactories.append(g);
                // ... then in our "::actions(..)" functions:
                // foreach (const QContactAbstactActionFactory* f, m_actionFactories) {
                //     retn += f.instances();
                // }

            } else {
                qDebug() << "Static: plugin found is of unknown type!";
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
            QContactAbstractActionFactory *g = qobject_cast<QContactAbstractActionFactory*>(qpl.instance());
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
            } else if (g) {
                QString name = g->name();
                qDebug() << "Dynamic: found an action factory" << g << "with name" << name;

                // option one: we own the action implementations; clients DO NOT delete them...
                QList<QContactAbstractAction*> impls = g->instances();
                for (int j = 0; j < impls.size(); j++) {
                    QContactAbstractAction* impl = impls.at(j);
                    m_actionImplementations.insert(impl->actionName(), impl);
                }

                // option two: we own the factory instances; clients own the action implementation instances!
                // m_actionFactories.append(g);
                // ... then in our "::actions(..)" functions:
                // foreach (const QContactAbstactActionFactory* f, m_actionFactories) {
                //     retn += f.instances();
                // }
            } else {
                qDebug() << "Dynamic: plugin found is of unknown type!";
                qDebug() << "    qpl.instance() =" << qpl.instance();
            }
        }

        qDebug() << "Found engines:" << m_engines.keys();
        qDebug() << "Found actions:" << m_actionImplementations.keys();
    }
}

QList<QContactAbstractAction*> QContactManagerData::actions(const QString& actionName, const QString& vendor, int implementationVersion)
{
    QList<QContactAbstractAction*> all;
    QList<QContactAbstractAction*> retn;
    if (actionName.isEmpty()) {
        // return the entire list of implementations (which are from the given vendor and of the given impl. version)
        all = m_actionImplementations.values();
    } else {
        // just get the list of actions of the specified name, then sort by vendor and impl. version.
        all = m_actionImplementations.values(actionName);
    }

    // no vendor given, :. implVersion is meaningless; return all.
    if (vendor.isEmpty()) {
        return all;
    }

    // filter by vendor and (if supplied) impl. version.
    for (int i = 0; i < all.size(); i++) {
        QContactAbstractAction* impl = all.at(i);
        if (impl->vendor() == vendor && (implementationVersion == -1
                || implementationVersion == impl->implementationVersion())) {
            retn.append(impl);
        }
    }

    return retn;
}

