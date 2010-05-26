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

#include "qgallerytrackermetadataedit_p.h"

#include <QtDBus/qdbuspendingcall.h>

QTM_BEGIN_NAMESPACE

QGalleryTrackerMetaDataEdit::QGalleryTrackerMetaDataEdit(
        const QGalleryDBusInterfacePointer &metaDataInterface,
        const QString &uri,
        const QString &service,
        QObject *parent)
    : QObject(parent)
    , m_watcher(0)
    , m_index(-1)
    , m_metaDataInterface(metaDataInterface)
    , m_uri(uri)
    , m_service(service)
{
}

QGalleryTrackerMetaDataEdit::~QGalleryTrackerMetaDataEdit()
{
}

void QGalleryTrackerMetaDataEdit::commit()
{
    if (m_values.isEmpty()) {
        emit finished(this);
    } else {
        m_watcher = new QDBusPendingCallWatcher(m_metaDataInterface->asyncCall(
                QLatin1String("Set"),
                m_service,
                m_uri,
                QStringList(m_values.keys()),
                QStringList(m_values.values())), this);

        if (m_watcher->isFinished()) {
            watcherFinished(m_watcher);
        } else {
            connect(m_watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this, SLOT(watcherFinished(QDBusPendingCallWatcher*)));
        }
    }
}

void QGalleryTrackerMetaDataEdit::watcherFinished(QDBusPendingCallWatcher *watcher)
{
    Q_ASSERT(watcher == m_watcher);

    m_watcher->deleteLater();
    m_watcher = 0;

    if (watcher->isError()) {
        qWarning("DBUS error %s", qPrintable(watcher->error().message()));

        m_values.clear();
    }

    emit finished(this);
}

#include "moc_qgallerytrackermetadataedit_p.cpp"

QTM_END_NAMESPACE
