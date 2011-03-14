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

#include <QDir>
#include <QFile>
#include <QDebug>

#include "databasefilewatcher_p.h"

DatabaseFileWatcher::DatabaseFileWatcher(const QString &databasePath, QObject *parent)
    : QObject(parent),
      m_watcher(0),
      m_databasePath(databasePath)
{
}

QString DatabaseFileWatcher::closestExistingParent(const QString &path)
{
    if (QFile::exists(path))
        return path;

    int lastSep = path.lastIndexOf(QDir::separator());
    if (lastSep < 0)
        return QString();
    return closestExistingParent(path.mid(0, lastSep));
}

void DatabaseFileWatcher::restartDirMonitoring(const QString &previousDirPath)
{
    if (m_watcher->files().contains(m_databasePath))
        return;

    QString existing = closestExistingParent(m_databasePath);
    if (existing.isEmpty()) {
        qWarning() << "QServiceManager: can't find existing directory for path to database" << m_databasePath
            << "serviceAdded() and serviceRemoved() will not be emitted";
        return;
    }
    if (existing == m_databasePath) {
            if (!previousDirPath.isEmpty())
                m_watcher->removePath(previousDirPath);

            setEnabled(true);
    } else {
        if (previousDirPath != existing) {
            if (!previousDirPath.isEmpty())
                m_watcher->removePath(previousDirPath);
            if (!m_watcher->directories().contains(existing))
                m_watcher->addPath(existing);
        }
    }
}

void DatabaseFileWatcher::setEnabled(bool enabled)
{
    if (!m_watcher) {
        m_watcher = new QFileSystemWatcher(this);
        connect(m_watcher, SIGNAL(fileChanged(QString)),
            SLOT(databaseChanged(QString)));
        connect(m_watcher, SIGNAL(directoryChanged(QString)),
            SLOT(databaseDirectoryChanged(QString)));
    }

    if (enabled) {
        if (QFile::exists(m_databasePath)) {
            if (!m_watcher->files().contains(m_databasePath))
                    m_watcher->addPath(m_databasePath);
        } else {
            restartDirMonitoring(QString());
        }
    } else {
        m_watcher->removePath(m_databasePath);
    }
}

void DatabaseFileWatcher::databaseDirectoryChanged(const QString &path)
{
    if (m_databasePath.contains(path)) {
        restartDirMonitoring(path);
    }
}

void DatabaseFileWatcher::databaseChanged(const QString &path)
{
    if (!QFile::exists(m_databasePath)) {
        restartDirMonitoring(QString());;
    }

    emit notifyChange();
    // if database was deleted, the path may have been dropped
    if (!m_watcher->files().contains(path) && QFile::exists(path))
        m_watcher->addPath(path);
}
