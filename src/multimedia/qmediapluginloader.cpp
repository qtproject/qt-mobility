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

#include "qmediapluginloader_p.h"
#include <QtCore/qcoreapplication.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>

#include "qmediaserviceproviderplugin.h"
#include "qmobilitypluginsearch.h"

QT_BEGIN_NAMESPACE

typedef QMap<QString,QObjectList> ObjectListMap;
Q_GLOBAL_STATIC(ObjectListMap, staticMediaPlugins);

QMediaPluginLoader::QMediaPluginLoader(const char *iid, const QString &location, Qt::CaseSensitivity):
    m_iid(iid)
{
    m_location = location + "/";
    load();
}

QStringList QMediaPluginLoader::keys() const
{
    return m_instances.keys();
}

QObject* QMediaPluginLoader::instance(QString const &key)
{
    return m_instances.value(key);
}

QList<QObject*> QMediaPluginLoader::instances(QString const &key)
{
    return m_instances.values(key);
}

//to be used for testing purposes only
void QMediaPluginLoader::setStaticPlugins(const QString &location, const QObjectList& objects)
{
    staticMediaPlugins()->insert(location+"/", objects);
}

void QMediaPluginLoader::load()
{
    if (!m_instances.isEmpty())
        return;

    if (staticMediaPlugins() && staticMediaPlugins()->contains(m_location)) {
        foreach(QObject *o, staticMediaPlugins()->value(m_location)) {
            if (o != 0 && o->qt_metacast(m_iid) != 0) {
                QFactoryInterface* p = qobject_cast<QFactoryInterface*>(o);
                if (p != 0) {
                    foreach (QString const &key, p->keys())
                        m_instances.insertMulti(key, o);
                }
            }
        }
    } else {
        QStringList plugins = QTM_PREPEND_NAMESPACE(mobilityPlugins)(m_location);
        for (int i=0; i < plugins.count(); i++) {
            QPluginLoader   loader(plugins.at(i));
            QObject *o = loader.instance();
            if (o != 0 && o->qt_metacast(m_iid) != 0) {
                QFactoryInterface* p = qobject_cast<QFactoryInterface*>(o);
                if (p != 0) {
                    foreach (QString const &key, p->keys())
                        m_instances.insertMulti(key, o);
                }

                continue;
            } else {
                qWarning() << "QMediaPluginLoader: Failed to load plugin: " << plugins.at(i) << loader.errorString();
            }
            delete o;
            loader.unload();
        }
    }
}
QT_END_NAMESPACE

