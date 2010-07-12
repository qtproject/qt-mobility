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

#include "qversitpluginloader_p.h"
#include "qmobilitypluginsearch.h"

QTM_USE_NAMESPACE

QVersitPluginLoader* QVersitPluginLoader::mInstance = NULL;

QVersitPluginLoader::QVersitPluginLoader()
{
}

QVersitPluginLoader* QVersitPluginLoader::instance()
{
    if (!mInstance)
        mInstance = new QVersitPluginLoader;
    return mInstance;
}

void QVersitPluginLoader::loadPlugins() {
    QStringList plugins = mobilityPlugins(QLatin1String("versit"));
    if (plugins != mPluginPaths) {
        mPluginPaths = plugins;

        foreach (const QString& pluginPath, mPluginPaths) {
            QPluginLoader qpl(pluginPath);
            QVersitContactHandlerFactory* handler =
                qobject_cast<QVersitContactHandlerFactory*>(qpl.instance());
            if (handler && !mFactories.contains(handler->name())) {
                mFactories.insert(handler->name(), handler);
            }
        }
    }
}

QList<QVersitContactHandler*> QVersitPluginLoader::createHandlers()
{
    loadPlugins();

    QList<QVersitContactHandler*> handlers;
    foreach (const QVersitContactHandlerFactory* factory, mFactories) {
        QVersitContactHandler* handler = factory->createHandler();
        handlers.append(handler);
    }
    return handlers;
}
