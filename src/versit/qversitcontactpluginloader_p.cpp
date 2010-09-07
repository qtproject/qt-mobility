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

#include <QPluginLoader>

#include "qversitcontactpluginloader_p.h"
#include "qmobilitypluginsearch.h"

QTM_USE_NAMESPACE

/*!
   A less-than function for factory indices (see QVersitContactHandlerFactory::index()).
   Positive values come first (ascendingly), then zero, then negative values (ascendingly).
 */
bool factoryLessThan(QVersitContactHandlerFactory* a, QVersitContactHandlerFactory* b) {
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

QVersitContactPluginLoader* QVersitContactPluginLoader::mInstance = NULL;

/*!
 * \class QVersitContactPluginLoader
 * This is a singleton class that loads Versit plugins for contacts processing
 */

QVersitContactPluginLoader::QVersitContactPluginLoader()
{
}

/*!
 * Returns the singleton instance of the QVersitContactPluginLoader.
 */
QVersitContactPluginLoader* QVersitContactPluginLoader::instance()
{
    if (!mInstance)
        mInstance = new QVersitContactPluginLoader;
    return mInstance;
}

void QVersitContactPluginLoader::loadPlugins() {
    QStringList plugins = mobilityPlugins(QLatin1String("versit"));
    if (plugins != mPluginPaths) {
        mPluginPaths = plugins;

        foreach (const QString& pluginPath, mPluginPaths) {
            QPluginLoader qpl(pluginPath);
            QObject* plugin = qpl.instance();
            QVersitContactHandlerFactory* contactPlugin =
                qobject_cast<QVersitContactHandlerFactory*>(plugin);
            if (contactPlugin && !mLoadedFactories.contains(contactPlugin->name())) {
                mLoadedFactories.insert(contactPlugin->name());
                mContactHandlerFactories.append(contactPlugin);
            }
        }
        qSort(mContactHandlerFactories.begin(), mContactHandlerFactories.end(), factoryLessThan);
    }
}

/*!
 * Creates and returns handlers from the plugin.  If \a profile is the empty string, only handlers
 * with an empty profile list are returned.  If \a profile is nonempty, only handlers with either
 * an empty profile list or a profile list that contains the given \a profile are returned.
 *
 * The caller is responsible for deleting all returned handlers.
 */
QList<QVersitContactHandler*> QVersitContactPluginLoader::createContactHandlers(const QString& profile)
{
    loadPlugins();

    QList<QVersitContactHandler*> handlers;
    foreach (const QVersitContactHandlerFactory* factory, mContactHandlerFactories) {
        if (factory->profiles().isEmpty() ||
                (!profile.isEmpty() && factory->profiles().contains(profile))) {
            QVersitContactHandler* handler = factory->createHandler();
            handlers.append(handler);
        }
    }
    return handlers;
}
