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

#include "gconflayer_linux_p.h"
#include <QVariant>
#include <GConfItem>

#include <QDebug>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(GConfLayer, gConfLayer);
QVALUESPACE_AUTO_INSTALL_LAYER(GConfLayer);

GConfLayer::GConfLayer()
{
}

GConfLayer::~GConfLayer()
{
    QMutableHashIterator<QString, GConfHandle *> i(m_handles);
    while (i.hasNext()) {
        i.next();

        removeHandle(Handle(i.value()));
    }
}

QString GConfLayer::name()
{
    return QLatin1String("GConf Layer");
}

QUuid GConfLayer::id()
{
    return QVALUESPACE_GCONF_LAYER;
}

unsigned int GConfLayer::order()
{
    return 0;
}

QValueSpace::LayerOptions GConfLayer::layerOptions() const
{
    return QValueSpace::PermanentLayer |
        QValueSpace::WritableLayer;
}

GConfLayer *GConfLayer::instance()
{
    return gConfLayer();
}

bool GConfLayer::startup(Type /*type*/)
{
    return true;
}

bool GConfLayer::value(Handle handle, QVariant *data)
{
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    return value(InvalidHandle, sh->path, data);
}

bool GConfLayer::value(Handle handle, const QString &subPath, QVariant *data)
{
    if (handle != InvalidHandle && !gConfHandle(handle))
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);
    if (handle != InvalidHandle)
        while (path.startsWith(QLatin1Char('/')))
            path = path.mid(1);
    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        handle = item(handle, path);
        createdHandle = true;
    }

    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);

    qDebug() << "Read value from" << fullPath;
    GConfItem gconfItem(fullPath);
    *data = gconfItem.value();
    qDebug() << "*data = " << *data;

    if (createdHandle)
        removeHandle(handle);

    return data->isValid();
}

QSet<QString> GConfLayer::children(Handle handle)
{
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return QSet<QString>();

    qDebug() << "Get children from" << sh->path;
    GConfItem gconfItem(sh->path);

    QSet<QString> ret;
    foreach (const QString child, gconfItem.listEntries() + gconfItem.listDirs()) {
        const int index = child.lastIndexOf(QLatin1Char('/'), -1);
        ret += child.mid(index + 1);
        qDebug() << "found" << child.mid(index + 1);
    }

    return ret;
}

QAbstractValueSpaceLayer::Handle GConfLayer::item(Handle parent, const QString &path)
{
    QString fullPath;

    // Fail on invalid path.
    if (path.isEmpty() || path.contains(QLatin1String("//")))
        return InvalidHandle;

    if (parent == InvalidHandle) {
        fullPath = path;
    } else {
        GConfHandle *sh = gConfHandle(parent);
        if (!sh)
            return InvalidHandle;

        if (path == QLatin1String("/")) {
            fullPath = sh->path;
        } else if (sh->path.endsWith(QLatin1Char('/')) && path.startsWith(QLatin1Char('/')))
            fullPath = sh->path + path.mid(1);
        else if (!sh->path.endsWith(QLatin1Char('/')) && !path.startsWith(QLatin1Char('/')))
            fullPath = sh->path + QLatin1Char('/') + path;
        else
            fullPath = sh->path + path;
    }

    if (m_handles.contains(fullPath)) {
        GConfHandle *sh = m_handles.value(fullPath);
        ++sh->refCount;
        return Handle(sh);
    }

    // Create a new handle for path
    GConfHandle *sh = new GConfHandle(fullPath);
    m_handles.insert(fullPath, sh);

    return Handle(sh);
}

void GConfLayer::setProperty(Handle handle, Properties properties)
{
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return;

    if (properties & QAbstractValueSpaceLayer::Publish) {
        qDebug() << "TODO: Start monitoring (child) items from" << sh->path;
    } else {
        qDebug() << "TODO: Stop monitoring (child) items from" << sh->path;
    }

}

void GConfLayer::removeHandle(Handle handle)
{
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return;

    if (--sh->refCount)
        return;

    m_handles.remove(sh->path);

    delete sh;
}

bool GConfLayer::setValue(QValueSpacePublisher */*creator*/,
                                    Handle handle,
                                    const QString &subPath,
                                    const QVariant &data)
{
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);

    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        if (path.isEmpty())
            path.append(QLatin1Char('/'));

        sh = gConfHandle(item(Handle(sh), path));
        createdHandle = true;
    }

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);


    qDebug() << "Write value" << data << "to" << fullPath;
    GConfItem gconfItem(fullPath);
    gconfItem.set(data);

    if (createdHandle)
        removeHandle(Handle(sh));
    return true;    //TODO: How to check whether set was ok?
}

void GConfLayer::sync()
{
    //Not needed
}

bool GConfLayer::removeSubTree(QValueSpacePublisher * /*creator*/, Handle /*handle*/)
{
    //Not needed
    return false;
}

bool GConfLayer::removeValue(QValueSpacePublisher */*creator*/,
    Handle handle,
    const QString &subPath)
{
    GConfHandle *sh = gConfHandle(handle);
    if (!sh)
        return false;

    QString path(subPath);
    while (path.endsWith(QLatin1Char('/')))
        path.chop(1);

    int index = path.lastIndexOf(QLatin1Char('/'), -1);

    bool createdHandle = false;

    QString value;
    if (index == -1) {
        value = path;
    } else {
        // want a value that is in a sub path under handle
        value = path.mid(index + 1);
        path.truncate(index);

        if (path.isEmpty())
            path.append(QLatin1Char('/'));

        sh = gConfHandle(item(Handle(sh), path));
        createdHandle = true;
    }

    QString fullPath(sh->path);
    if (fullPath != QLatin1String("/"))
        fullPath.append(QLatin1Char('/'));

    fullPath.append(value);

    qDebug() << "TODO: Remove value from" << fullPath;
    GConfItem gconfItem(fullPath);
    gconfItem.unset();

    if (createdHandle)
        removeHandle(Handle(sh));

    return true;    //TODO: How to check whether unset was ok?
}

void GConfLayer::addWatch(QValueSpacePublisher *, Handle)
{
    //Not needed
}

void GConfLayer::removeWatches(QValueSpacePublisher *, Handle)
{
    //Not needed
}

bool GConfLayer::supportsInterestNotification() const
{
    return false;
}

bool GConfLayer::notifyInterest(Handle, bool)
{
    //Not needed
    return false;
}

#include "moc_gconflayer_linux_p.cpp"

QTM_END_NAMESPACE
