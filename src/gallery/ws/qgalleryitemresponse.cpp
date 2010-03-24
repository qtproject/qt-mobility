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

#include "qgalleryitemresponse_p.h"

#include "qgallerybinding_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>

#include <oledberr.h>

namespace QWindowsSearch
{

QGalleryItemResponse::QGalleryItemResponse(
            IRowsetScroll *rowSet,
            const QGalleryItemRequest &request,
            const QVector<QGalleryQueryBuilder::Column> &columns,
            QObject *parent)
    : QGalleryRowSetResponse(rowSet, parent)
    , m_urlIndex(0)
    , m_typeIndex(0)
    , m_rowOffset(0)
    , m_count(0)
    , m_minimumPagedItems(qMax<int>(PageSize, request.minimumPagedItems()))
    , m_rowSet(rowSet)
    , m_binding(0)
    , m_placeholderRow(new QGalleryItemListRow)
    , m_fields(request.fields())
{
    m_binding = new QGalleryBinding(m_rowSet);
    m_placeholderRow->metaData.resize(m_fields.count());

    if (m_binding->handle()) {    
        for (int i = 0; i < m_fields.count(); ++i) {
            bool matched = false;
            for (int j = 0; j < columns.count() && !matched; ++j) {
                if (m_fields.at(i) == columns.at(j).first) {
                    for (int k = 0; k < m_binding->columnCount() && !matched; ++k) {
                        if (QString::compare(
                                columns.at(j).second,
                                m_binding->columnName(k),
                                Qt::CaseInsensitive) == 0) {
                            m_columnIndexes.append(k);
                            matched = true;
                        }
                    }
                }
            }
            m_keys.append(i);
            if (!matched)
                m_columnIndexes.append(m_binding->columnCount());
        }

        m_urlIndex = m_binding->columnCount();
        m_typeIndex = m_binding->columnCount();

        for (int k = 0; k < m_binding->columnCount(); ++k) {
            if (QString::compare(QLatin1String("System.ItemUrl"), 
                    m_binding->columnName(k),
                    Qt::CaseInsensitive) == 0) {
                m_urlIndex = k;
                break;
            }
        }

        for (int k = 0; k < m_binding->columnCount(); ++k) {
            if (QString::compare(QLatin1String("System.Kind"), m_binding->columnName(k)) == 0) {
                m_typeIndex = k;
                break;
            }
        }
    } else {
        IDBAsynchStatus *status = 0;
        if (SUCCEEDED(m_rowSet->QueryInterface(
                IID_IDBAsynchStatus, reinterpret_cast<void **>(&status)))) {
            status->Abort(DB_NULL_HCHAPTER, DBASYNCHOP_OPEN);
            status->Release();
        }

        finish(QGalleryAbstractRequest::ConnectionError);
    }
}

QGalleryItemResponse::~QGalleryItemResponse()
{
    delete m_binding;
}

QList<int> QGalleryItemResponse::keys() const
{
    return m_keys;
}

QString QGalleryItemResponse::toString(int key) const
{
    return m_fields.value(key);
}

void QGalleryItemResponse::setCursorPosition(int position)
{
    if (position > cursorPosition()) {
        appendRows(qMax(0, position));

        QGalleryAbstractResponse::setCursorPosition(position);
    } else if (position < cursorPosition()) {
        prependRows(qMax(0, position));

        QGalleryAbstractResponse::setCursorPosition(position);
    }
}

int QGalleryItemResponse::minimumPagedItems() const
{
    return m_minimumPagedItems;
}

int QGalleryItemResponse::count() const
{
    return m_count;
}

QString QGalleryItemResponse::id(int index) const
{
    return row(index)->url;
}

QUrl QGalleryItemResponse::url(int index) const
{
    return QUrl(row(index)->url);
}

QString QGalleryItemResponse::type(int index) const
{
    return row(index)->type;
}

QList<QGalleryResource> QGalleryItemResponse::resources(int index) const
{
    return QList<QGalleryResource>() << QGalleryResource(QUrl(row(index)->url));
}

QVariant QGalleryItemResponse::metaData(int index, int key) const
{
    return row(index)->metaData.at(key);
}

void QGalleryItemResponse::setMetaData(int index, int key, const QVariant &value)
{
    Q_UNUSED(index);
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QGalleryItemList::MetaDataFlags QGalleryItemResponse::metaDataFlags(
        int index, int key) const
{
    Q_UNUSED(index);
    Q_UNUSED(key);

    return 0;
}

void QGalleryItemResponse::customEvent(QEvent *event)
{
    if (event->type() == QGalleryEvent::ItemsInserted) {
        QGalleryItemsInsertedEvent *insertEvent
                = static_cast<QGalleryItemsInsertedEvent *>(event);
        int index = m_count;

        m_count += insertEvent->count;

        emit inserted(index, insertEvent->count);

        appendRows(cursorPosition());
    } else {
        QGalleryRowSetResponse::customEvent(event);
    }
}

void QGalleryItemResponse::appendRows(int position)
{
    const int alignedPosition = ~(PageSize - 1) & qMin(
            position, qMax(0, m_count - m_minimumPagedItems));

    const int count = (position + m_minimumPagedItems + PageSize - 1) & ~(PageSize - 1);

    int offset = qMax(alignedPosition, m_rowOffset + m_rows.count());

    if (offset == count)
        return;

    DBBOOKMARK bookmark = DBBMK_FIRST;
    DBCOUNTITEM rowsFetched = -1;
    HROW rows[PageSize];
    HROW *pRows = rows;

    QVector<Row> newRows;
    newRows.reserve(count - offset);

    QByteArray buffer;
    buffer.resize(m_binding->bufferSize());

    for (; offset < count && rowsFetched != 0; offset += rowsFetched) {
        if (SUCCEEDED(m_rowSet->GetRowsAt(
                0,
                DB_NULL_HCHAPTER,
                STD_BOOKMARKLENGTH,
                reinterpret_cast<BYTE *>(&bookmark),
                offset,
                PageSize,
                &rowsFetched,
                &pRows))) {
            for (DBCOUNTITEM i = 0; i < rowsFetched; ++i) {
                if (SUCCEEDED(m_rowSet->GetData(rows[i], m_binding->handle(), buffer.data()))) {
                    Row row = Row(new QGalleryItemListRow);
                    row->workId = 0;
                    row->flags = 0;
                    row->url = m_binding->toString(buffer.data(), m_urlIndex);
                    row->type = m_binding->toString(buffer.data(), m_typeIndex);

                    for (int i = 0; i < m_columnIndexes.count(); ++i) {
                        row->metaData.append(
                                m_binding->toVariant(buffer.data(), m_columnIndexes.at(i)));
                    }
                    newRows.append(row);
                }
            }
            m_rowSet->ReleaseRows(rowsFetched, rows, 0, 0, 0);
        } else {
            rowsFetched = 0;
        }
    }

    if (!newRows.isEmpty()) {
        int changeIndex = 0;

        if (alignedPosition > m_rowOffset + m_rows.count()) {
            changeIndex = alignedPosition;

            m_rows = newRows;

            m_rowOffset = alignedPosition;
        } else {
            changeIndex = m_rowOffset + m_rows.count();

            const int rowOffset = ~(PageSize - 1) & qMax(
                    0, m_rowOffset + m_rows.count() + newRows.count() - m_minimumPagedItems);
            if (rowOffset != m_rowOffset) {
                m_rows = m_rows.mid(rowOffset - m_rowOffset) + newRows;

                m_rowOffset = alignedPosition;
            } else {
                m_rows += newRows;
            }
        }
        const int changeCount = newRows.count();
        newRows.clear();

        emit metaDataChanged(changeIndex, changeCount);
    }
    qDebug("Appended items\n\tOffset: %d\n\tCached Items: %d", m_rowOffset, m_rows.count());
}

void QGalleryItemResponse::prependRows(int position)
{
    const int alignedPosition = ~(PageSize - 1) & qMin(
            position, qMax(0, m_count - m_minimumPagedItems));

    const int count = qMin(
            (position + m_minimumPagedItems + PageSize - 1) & ~(PageSize - 1),
            m_rowOffset);

    if (count == alignedPosition)
        return;

    DBBOOKMARK bookmark = DBBMK_FIRST;
    DBCOUNTITEM rowsFetched = -1;
    HROW rows[PageSize];
    HROW *pRows = rows;

    QVector<Row> newRows;
    newRows.reserve(count - alignedPosition);

    QByteArray buffer;
    buffer.resize(m_binding->bufferSize());

    for (int offset = count - 1; offset >= alignedPosition && rowsFetched != 0; offset -= rowsFetched) {
        if (SUCCEEDED(m_rowSet->GetRowsAt(
                0,
                DB_NULL_HCHAPTER,
                STD_BOOKMARKLENGTH,
                reinterpret_cast<BYTE *>(&bookmark),
                offset,
                -PageSize,
                &rowsFetched,
                &pRows))) {
            for (DBCOUNTITEM i = 0; i < rowsFetched; ++i) {
                if (SUCCEEDED(m_rowSet->GetData(rows[i], m_binding->handle(), buffer.data()))) {
                    Row row = Row(new QGalleryItemListRow);
                    row->workId = 0;
                    row->flags = 0;
                    row->url = m_binding->toString(buffer.data(), m_urlIndex);
                    row->type = m_binding->toString(buffer.data(), m_typeIndex);

                    for (int i = 0; i < m_columnIndexes.count(); ++i) {
                        row->metaData.append(
                                m_binding->toVariant(buffer.data(), m_columnIndexes.at(i)));
                    }
                    newRows.prepend(row);
                }
            }
            m_rowSet->ReleaseRows(rowsFetched, rows, 0, 0, 0);
        } else {
            rowsFetched = 0;
        }
    }

    if (!newRows.isEmpty()) {
        m_rowOffset = count - newRows.count();
        if (count < m_rowOffset) {
            m_rows = newRows;
        } else {
            const int maximumCount = ~(PageSize - 1)
                    & (alignedPosition + m_minimumPagedItems + PageSize - 1);
            if (maximumCount < m_rowOffset + m_rows.count()) {
                m_rows = newRows + m_rows.mid(0, maximumCount - m_rowOffset);
            } else {
                m_rows = newRows +  m_rows;
            }
        }
        const int changeCount = newRows.count();
        newRows.clear();

        emit metaDataChanged(count - changeCount, changeCount);
    }

    qDebug("Prepended items\n\tOffset: %d\n\tCached Items: %d", m_rowOffset, m_rows.count());
}

}
