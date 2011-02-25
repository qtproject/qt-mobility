/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QPluginLoader>

#include "qversitorganizerpluginloader_p.h"
#include "qmobilitypluginsearch.h"

QTM_USE_NAMESPACE

/*!
   A less-than function for factory indices (see QVersitOrganizerHandlerFactory::index()).
   Positive values come first (ascendingly), then zero, then negative values (ascendingly).
 */
bool factoryLessThan(QVersitOrganizerHandlerFactory* a, QVersitOrganizerHandlerFactory* b) {
    if ((a->index() > 0 && b->index() > 0)
            || (a->index() < 0 && b->index() < 0))
        // same sign
        return a->index() < b->index();
    else
        // a is zero
        // or b is zero
        // or opposite sign
        return b->index() < a->index();
}

QVersitOrganizerPluginLoader* QVersitOrganizerPluginLoader::mInstance = NULL;

/*!
 * \class QVersitOrganizerPluginLoader
 * \internal
 * This is a singleton class that loads Versit plugins for organizer item processing
 */

QVersitOrganizerPluginLoader::QVersitOrganizerPluginLoader() : mTimeZoneHandler(NULL)
{
}

/*!
 * Returns the singleton instance of the QVersitOrganizerPluginLoader.
 */
QVersitOrganizerPluginLoader* QVersitOrganizerPluginLoader::instance()
{
    if (!mInstance)
        mInstance = new QVersitOrganizerPluginLoader;
    return mInstance;
}

void QVersitOrganizerPluginLoader::loadPlugins() {
    QStringList plugins = mobilityPlugins(QLatin1String("versit"));
    if (plugins != mPluginPaths) {
        mPluginPaths = plugins;

        foreach (const QString& pluginPath, mPluginPaths) {
            QPluginLoader qpl(pluginPath);
            QObject* plugin = qpl.instance();
            QVersitOrganizerHandlerFactory* organizerPlugin =
                qobject_cast<QVersitOrganizerHandlerFactory*>(plugin);
            if (organizerPlugin && !mLoadedFactories.contains(organizerPlugin->name())) {
                mLoadedFactories.insert(organizerPlugin->name());
                mOrganizerHandlerFactories.append(organizerPlugin);
            } else if (!mTimeZoneHandler) {
                QVersitTimeZoneHandler* timeZonePlugin =
                    qobject_cast<QVersitTimeZoneHandler*>(plugin);
                if (timeZonePlugin) {
                    mTimeZoneHandler = timeZonePlugin;
                }
            }
        }
        qSort(mOrganizerHandlerFactories.begin(), mOrganizerHandlerFactories.end(), factoryLessThan);
    }
}

/*!
 * Creates and returns handlers from the plugin.  If \a profile is the empty string, only handlers
 * with an empty profile list are returned.  If \a profile is nonempty, only handlers with either
 * an empty profile list or a profile list that contains the given \a profile are returned.
 *
 * The caller is responsible for deleting all returned handlers.
 */
QList<QVersitOrganizerHandler*> QVersitOrganizerPluginLoader::createOrganizerHandlers(const QString& profile)
{
    loadPlugins();

    QList<QVersitOrganizerHandler*> handlers;
    foreach (const QVersitOrganizerHandlerFactory* factory, mOrganizerHandlerFactories) {
        if (factory->profiles().isEmpty() ||
                (!profile.isEmpty() && factory->profiles().contains(profile))) {
            QVersitOrganizerHandler* handler = factory->createHandler();
            handlers.append(handler);
        }
    }
    return handlers;
}

QVersitTimeZoneHandler* QVersitOrganizerPluginLoader::timeZoneHandler()
{
    loadPlugins();

    return mTimeZoneHandler;
}
