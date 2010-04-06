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

#include "qgallerytrackerlistresponse_p.h"

#include <QtDBus/qdbuspendingreply.h>

QGalleryTrackerListResponse::QGalleryTrackerListResponse(
        const QGalleryTrackerSchema &schema, int minimumPagedItems, QObject *parent)
    : QGalleryAbstractResponse(parent)
    , idFunc(schema.idFunc())
    , m_minimumPagedItems(minimumPagedItems)
    , m_count(0)
    , m_insertIndex(0)
    , m_rowOffset(-1)
    , m_cursorOutdated(false)
    , m_call(0)
{
}

QGalleryTrackerListResponse::~QGalleryTrackerListResponse()
{
}

int QGalleryTrackerListResponse::minimumPagedItems() const
{
    return m_minimumPagedItems;
}

QList<int> QGalleryTrackerListResponse::keys() const
{
    return m_keys;
}

void QGalleryTrackerListResponse::setKeys(const QList<int> &keys)
{
    m_keys = keys;
}

int QGalleryTrackerListResponse::count() const
{
    return m_count;
}

QStringList QGalleryTrackerListResponse::row(int index) const
{
    return m_rows.value(index - m_rowOffset);
}

QString QGalleryTrackerListResponse::id(int index) const
{
    return idFunc(m_rows.value(index - m_rowOffset));
}

QUrl QGalleryTrackerListResponse::url(int) const
{
    return QUrl();
}

QVariant QGalleryTrackerListResponse::metaData(int index, int key) const
{
    return m_rows.value(index - m_rowOffset).value(key);
}

void QGalleryTrackerListResponse::setMetaData(int index, int key, const QVariant &value)
{
    Q_UNUSED(index);
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QGalleryItemList::MetaDataFlags QGalleryTrackerListResponse::metaDataFlags(int index, int key) const
{
    Q_UNUSED(index);
    Q_UNUSED(key);

    return 0;
}

void QGalleryTrackerListResponse::cancel()
{
    // Should suppress any further cursor based requests here, but otherwise once a DBus call is
    // away there's no stopping it.
}

bool QGalleryTrackerListResponse::waitForFinished(int)
{
    if (m_call) {
        m_call->waitForFinished();

        callFinished(m_call);

        return true;
    } else {
        return false;
    }
}

void QGalleryTrackerListResponse::setCursorPosition(int position)
{
    if (!m_call) {
        m_cursorOutdated = false;

        const int alignedIndex = ~PageAlignment & qMax(0, position);
        const int alignedCount = ~PageAlignment & qMax(
                0, position + m_minimumPagedItems + PageAlignment);

        if (alignedIndex < m_rowOffset) {           
            const int count = qMin(alignedCount, m_rowOffset) - alignedIndex;

            m_insertIndex = alignedIndex;

            QDBusPendingCall call = queryRows(alignedIndex, count);

            m_call = new QDBusPendingCallWatcher(call);

            connect(m_call, SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this, SLOT(callFinished(QDBusPendingCallWatcher*)));
        } else if (alignedCount > m_rowOffset + m_rows.count()) {
            m_insertIndex = qMax(alignedIndex, m_rowOffset + m_rows.count());

            const int count = alignedCount - m_insertIndex;

            QDBusPendingCall call = queryRows(m_insertIndex, count);

            if (call.isError()) {
                qWarning("DBUS error %s", qPrintable(call.error().message()));
            }

            m_call = new QDBusPendingCallWatcher(call);

            if (m_call->isFinished()) {
                callFinished(m_call);
            } else {
                connect(m_call, SIGNAL(finished(QDBusPendingCallWatcher*)),
                        this, SLOT(callFinished(QDBusPendingCallWatcher*)));
            }
        }
    } else {
        m_cursorOutdated = true;
    }

    QGalleryAbstractResponse::setCursorPosition(position);
}

void QGalleryTrackerListResponse::callFinished(QDBusPendingCallWatcher *watcher)
{
    if (watcher != m_call) {
        // This probably shouldn't ever happen.
        Q_ASSERT(false);

        return;
    }
    m_call = 0;

    watcher->deleteLater();

    QDBusPendingReply<QVector<QStringList> > reply(*watcher);

    if (reply.isError()) {
        qWarning("DBUS error %s", qPrintable(reply.error().message()));
    }

    QVector<QStringList> rows = reply.value();

    if (rows.count() == 0)
        return;

    if (m_insertIndex < m_rowOffset) {
        if (m_insertIndex + rows.count() < m_rowOffset) {
            m_rows = rows;
            m_rowOffset = m_insertIndex;

            metaDataChanged(m_insertIndex, rows.count());
        } else if (m_insertIndex + rows.count() > m_rowOffset) {
            // This really shouldn't happen.
            Q_ASSERT(false);
            const int alignedCount = ~PageAlignment & qMax(
                    0, m_insertIndex + m_minimumPagedItems + PageAlignment);

            m_rows = rows + m_rows.mid(
                    m_insertIndex + rows.count() - m_rowOffset,
                    alignedCount - m_rowOffset);

            m_rowOffset = m_insertIndex;

            metaDataChanged(m_insertIndex, rows.count());
        } else {
            const int alignedCount = ~PageAlignment & qMax(
                    0, m_insertIndex + m_minimumPagedItems + PageAlignment);

            m_rows = rows + m_rows.mid(0, alignedCount - m_rowOffset);
            m_rowOffset = m_insertIndex;

            metaDataChanged(m_insertIndex, rows.count());
        }
    } else {
        if (m_insertIndex > m_rowOffset + m_rows.count()) {
            m_rows = rows;
            m_rowOffset = m_insertIndex;

            appendedItemsUpdate(m_insertIndex, rows.count());
        } else if (m_insertIndex < m_rowOffset + m_rows.count()) {
            // This really shouldn't happen.
            Q_ASSERT(false);
            const int alignedIndex = ~PageAlignment & qMax(0, m_insertIndex - m_minimumPagedItems);

            m_rows = m_rows.mid(
                    alignedIndex - m_rowOffset,
                    m_insertIndex - m_rowOffset - m_rows.count()) + rows;

            m_rowOffset = alignedIndex;

            appendedItemsUpdate(m_insertIndex, rows.count());
        } else {
            const int alignedIndex = ~PageAlignment & qMax(0, m_insertIndex - m_minimumPagedItems);

            m_rows = m_rows.mid(alignedIndex - m_rowOffset) + rows;
            m_rowOffset = alignedIndex;

            appendedItemsUpdate(m_insertIndex, rows.count());
        }
    }


    if (m_cursorOutdated)
        setCursorPosition(cursorPosition());
}

void QGalleryTrackerListResponse::appendedItemsUpdate(int index, int count)
{
    if (m_rowOffset > m_count) {
        const int insertIndex = m_count;

        m_count = m_rowOffset + m_rows.count();

        emit inserted(insertIndex, m_count - insertIndex);
    } else if (m_rowOffset + m_rows.count() > m_count) {
        const int insertIndex = m_count;

        metaDataChanged(index, m_count - index);

        m_count = m_rowOffset + m_rows.count();

        emit inserted(insertIndex, m_count - insertIndex);
    } else {
        metaDataChanged(index, count);
    }
}
