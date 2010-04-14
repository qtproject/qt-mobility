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

#include "qgallerytrackercountresponse_p.h"

#include "qgallerytrackerschema_p.h"

#include <QtDBus/qdbuspendingreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

QGalleryTrackerCountResponse::QGalleryTrackerCountResponse(
        const QDBusConnection &connection,
        const QGalleryTrackerSchema &schema,
        const QString &query,
        QObject *parent)
    : QGalleryAbstractResponse(parent)
    , m_count(0)
    , m_workingCount(0)
    , m_currentOffset(0)
    , m_call(0)
    , m_dbusInterface(
            QLatin1String("org.freedesktop.Tracker"),
            QLatin1String("/org/freedesktop/Tracker/Metadata"),
            QLatin1String("org.freedesktop.Tracker.Metadata"),
            connection)
    , m_query(query)
    , m_service(schema.service())
    , m_identityFields(schema.identityFields())

{
    if (!m_identityFields.isEmpty())
        m_countField = m_identityFields.takeLast();

    queryCount();
}

QGalleryTrackerCountResponse::~QGalleryTrackerCountResponse()
{
}


QStringList QGalleryTrackerCountResponse::propertyNames() const
{
    return QStringList();
}

int QGalleryTrackerCountResponse::propertyKey(const QString &) const
{
    return -1;
}

QGalleryProperty::Attributes QGalleryTrackerCountResponse::propertyAttributes(int) const
{
    return QGalleryProperty::Attributes();
}

int QGalleryTrackerCountResponse::count() const
{
    return m_count;
}

QString QGalleryTrackerCountResponse::id(int) const
{
    return QString();
}

QUrl QGalleryTrackerCountResponse::url(int) const
{
    return QUrl();
}

QString QGalleryTrackerCountResponse::type(int) const
{
    return QString();
}

QList<QGalleryResource> QGalleryTrackerCountResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QVariant QGalleryTrackerCountResponse::metaData(int, int) const
{
    return QVariant();
}

void QGalleryTrackerCountResponse::setMetaData(int, int, const QVariant &)
{
}

void QGalleryTrackerCountResponse::cancel()
{

}

bool QGalleryTrackerCountResponse::waitForFinished(int)
{
    if (m_call) {
        m_call->waitForFinished();

        callFinished(m_call);

        return true;
    } else {
        return false;
    }
}

void QGalleryTrackerCountResponse::callFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher != m_call) {
        // This shouldn't ever happen.
        Q_ASSERT(false);

        return;
    }
    m_call = 0;

    watcher->deleteLater();

    int oldCount = m_count;

    if (watcher->isError()) {
        finish(QGalleryAbstractRequest::ConnectionError);

        return;
    } else if (m_identityFields.isEmpty()) {
        QDBusPendingReply<int> reply(*watcher);

        m_count = reply.value();
    } else {
        QDBusPendingReply<QVector<QStringList> > reply(*watcher);

        const QVector<QStringList> counts = reply.value();

        for (QVector<QStringList>::const_iterator it = counts.begin(), end = counts.end();
                it != end;
                ++it) {
            m_workingCount += it->value(1).toInt();
        }

        m_currentOffset += counts.count();

        if (counts.count() == MaximumFetchSize) {
            if (m_count > m_workingCount)
                m_count = m_workingCount;

            queryCount();
        } else {
            m_count = m_workingCount;
        }
    }

    if (m_count > oldCount)
        emit inserted(oldCount, m_count - oldCount);
    else if (m_count < oldCount)
        emit removed(m_count, oldCount - m_count);

    if (!m_call)
        finish(QGalleryAbstractRequest::Succeeded);
}

void QGalleryTrackerCountResponse::queryCount()
{
    if (m_countField.isEmpty()) {
        m_call = new QDBusPendingCallWatcher(m_dbusInterface.asyncCall(
                QLatin1String("GetCount"), m_service, QLatin1String("*"), m_query), this);
    } else if (m_identityFields.isEmpty()) {
        m_call = new QDBusPendingCallWatcher(m_dbusInterface.asyncCall(
                QLatin1String("GetCount"), m_service, m_countField, m_query), this);
    } else {
        m_call = new QDBusPendingCallWatcher(m_dbusInterface.asyncCall(
                QLatin1String("GetUniqueValuesWithCount"),
                m_service,
                m_identityFields,
                m_query,
                m_countField,
                false,
                m_currentOffset,
                MaximumFetchSize), this);
    }

    if (m_call->isFinished()) {
        callFinished(m_call);
    } else {
        connect(m_call, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(callFinished(QDBusPendingCallWatcher*)));
    }
}

#include "moc_qgallerytrackercountresponse_p.cpp"

QTM_END_NAMESPACE
