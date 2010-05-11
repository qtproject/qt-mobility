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

#include <src/global/qbuildcfg.h>
#include "qsensorpluginloader_p.h"
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>

#include "qsensorplugin.h"

QTM_BEGIN_NAMESPACE

QSensorPluginLoader::QSensorPluginLoader(const char *iid, const QString &location)
    : m_iid(iid)
{
    m_location = location + QLatin1String("/");
    load();
}

QSensorPluginLoader::~QSensorPluginLoader()
{
    Q_FOREACH (QPluginLoader *loader, m_loaders) {
        bool ok = loader->unload();
        if (!ok) qWarning() << "Cannot unload" << loader->fileName();
        delete loader;
    }
}

void QSensorPluginLoader::load()
{
    if (!m_plugins.isEmpty())
        return;

    QStringList     paths = QCoreApplication::libraryPaths();
    QString val = QLatin1String(qt_mobility_configure_prefix_path_str);
    if(val.length() > 0){
        val += QLatin1String("/plugins");
        paths << val;
    }

    Q_FOREACH (QString const &path, paths) {
        QString pluginPathName(path + m_location);
        QDir pluginDir(pluginPathName);

        if (!pluginDir.exists())
            continue;

        Q_FOREACH (QString pluginLib, pluginDir.entryList(QDir::Files)) {
            QPluginLoader *loader = new QPluginLoader(pluginPathName + pluginLib);

            QObject *o = loader->instance();
            if (o != 0 && o->qt_metacast(m_iid) != 0) {
                QSensorPluginInterface *p = qobject_cast<QSensorPluginInterface*>(o);
                if (p != 0) {
                    m_plugins << p;
                    m_loaders << loader;
                } else {
                    loader->unload();
                    delete loader;
                }

                continue;
            } else {
                qWarning() << "QSensorPluginLoader: Failed to load plugin: " << pluginLib << loader->errorString();
            }
            delete o;
            loader->unload();
            delete loader;
        }
    }
}

QTM_END_NAMESPACE

