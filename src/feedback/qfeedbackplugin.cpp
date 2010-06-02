/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qfeedbackplugin.h"
#include "qfeedbackplugin_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>

QTM_BEGIN_NAMESPACE

static QFeedbackInterface *backend = 0;
static QThemedFeedbackInterface *themedBackend = 0;

static void initBackends()
{
    const QStringList paths = QCoreApplication::libraryPaths();
    for (int i = 0; i < paths.size() && !(backend && themedBackend); ++i) {

        QDir pluginDir(paths.at(i));
        if (!pluginDir.cd(QLatin1String("feedback")))
            continue;


        foreach (const QFileInfo &pluginLib, pluginDir.entryInfoList(QDir::Files)) {
            QPluginLoader loader(pluginLib.absoluteFilePath());
            bool usedPlugin = false;

            if (!backend) {
                backend = qobject_cast<QFeedbackInterface*>(loader.instance());
                usedPlugin = backend != 0;
            }

            if (!themedBackend) {
                themedBackend = qobject_cast<QThemedFeedbackInterface*>(loader.instance());
                usedPlugin = usedPlugin || backend != 0;
            }

            if (!usedPlugin)
                loader.unload();

            if (backend && themedBackend)
                break; //we already found our plugins
        }
    }

    if (!backend)
        backend = new QDummyBackend;
}

QFeedbackDevice QFeedbackInterface::createFeedbackDevice(int id)
{
    QFeedbackDevice dev;
    dev.m_id = id;
    return dev;
}

QFeedbackInterface *QFeedbackInterface::instance()
{
    if (!backend)
        initBackends();
    return backend;

}

QThemedFeedbackInterface *QThemedFeedbackInterface::instance()
{
    if (!backend)
        initBackends();
    return themedBackend;

}

QTM_END_NAMESPACE
