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

#include "qws4gallerydocumentresponse_p.h"

#include "qws4gallerybinding_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>

#include <oledberr.h>

QWS4GalleryDocumentResponse::QWS4GalleryDocumentResponse(
            IRowset *rowSet,
            const QGalleryDocumentRequest &request,
            const QVector<QWS4GalleryQueryBuilder::Column> &columns,
            QObject *parent)
    : QGalleryAbstractResponse(parent)
    , m_ref(1)
    , m_urlIndex(0)
    , m_typeIndex(0)
    , m_asynchNotifyCookie(0)
    , m_rowsetNotifyCookie(0)
    , m_rowsetEventsCookie(0)
    , m_rowSet(rowSet)
    , m_binding(0)
    , m_fields(request.fields())
{
    m_binding = new QWS4GalleryBinding(m_rowSet);

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

        IConnectionPointContainer *container = 0;
        if (SUCCEEDED(m_rowSet->QueryInterface(
                IID_IConnectionPointContainer, reinterpret_cast<void **>(&container)))) {
            IConnectionPoint *connection = 0;
            if (SUCCEEDED(container->FindConnectionPoint(IID_IDBAsynchNotify, &connection))) {
                connection->Advise(static_cast<IDBAsynchNotify *>(this), &m_asynchNotifyCookie);
                connection->Release();
            }
            if (SUCCEEDED(container->FindConnectionPoint(IID_IRowsetNotify, &connection))) {
                connection->Advise(static_cast<IRowsetNotify *>(this), &m_rowsetNotifyCookie);
                connection->Release();
            }
    #ifdef __IRowsetEvents_INTERFACE_DEFINED__
            if (request->isLive() && SUCCEEDED(
                    container->FindConnectionPoint(IID_IRowsetEvents, &connection))) {
                connection->Advise(static_cast<IRowsetEvents *>(this), &m_rowsetEventsCookie);
                connection->Release();
            }
            container->Release();
    #endif
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

QWS4GalleryDocumentResponse::~QWS4GalleryDocumentResponse()
{
    IConnectionPointContainer *container = 0;
    if (SUCCEEDED(m_rowSet->QueryInterface(
            IID_IConnectionPointContainer, reinterpret_cast<void **>(&container)))) {
        IConnectionPoint *connection = 0;
        if (m_asynchNotifyCookie != 0 && SUCCEEDED(
                container->FindConnectionPoint(IID_IDBAsynchNotify, &connection))) {
            connection->Unadvise(m_asynchNotifyCookie);
            connection->Release();
        }
        if (m_rowsetNotifyCookie && SUCCEEDED(
                container->FindConnectionPoint(IID_IRowsetNotify, &connection))) {
            connection->Unadvise(m_rowsetNotifyCookie);
            connection->Release();
        }
#ifdef __IRowsetEvents_INTERFACE_DEFINED__
        if (m_rowsetEventsCookie && SUCCEEDED(
                container->FindConnectionPoint(IID_IRowsetEvents, &connection))) {
            connection->Unadvise(m_rowsetEventsCookie);
            connection->Release();
        }
        container->Release();
#endif
    }

    delete m_binding;
    m_rowSet->Release();

    Q_ASSERT(m_ref == 1);
}

QList<int> QWS4GalleryDocumentResponse::keys() const
{
    return m_keys;
}

QString QWS4GalleryDocumentResponse::toString(int key) const
{
    return m_fields.value(key);
}

int QWS4GalleryDocumentResponse::count() const
{
    return m_rows.count();
}

QString QWS4GalleryDocumentResponse::id(int index) const
{
    return m_rows.at(index)->url;
}

QUrl QWS4GalleryDocumentResponse::url(int index) const
{
    return QUrl(m_rows.at(index)->url);
}

QString QWS4GalleryDocumentResponse::type(int index) const
{
    return m_rows.at(index)->type;
}

QList<QGalleryResource> QWS4GalleryDocumentResponse::resources(int index) const
{
    return QList<QGalleryResource>() << QGalleryResource(QUrl(m_rows.at(index)->url));
}

QVariant QWS4GalleryDocumentResponse::metaData(int index, int key) const
{
    return m_rows.at(index)->metaData.at(key);
}

void QWS4GalleryDocumentResponse::setMetaData(int index, int key, const QVariant &value)
{
    Q_UNUSED(index);
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QGalleryDocumentList::MetaDataFlags QWS4GalleryDocumentResponse::metaDataFlags(
        int index, int key) const
{
    Q_UNUSED(index);
    Q_UNUSED(key);

    return 0;
}

void QWS4GalleryDocumentResponse::cancel()
{
#ifdef __IRowsetEvents_INTERFACE_DEFINED__
    if (m_rowsetEventsCookie != 0) {
        IConnectionPointContainer *container = 0;
        if (SUCCEEDED(m_rowSet->QueryInterface(
                IID_IConnectionPointContainer, reinterpret_cast<void **>(&container)))) {
            IConnectionPoint *connection = 0;
            if (SUCCEEDED(container->FindConnectionPoint(IID_IRowsetEvents, &connection))) {
                connection->Unadvise(m_rowsetEventsCookie);
                connection->Release();

                m_rowsetEventsCookie = 0;
            }
            container->Release();
        }
    }
#endif

    IDBAsynchStatus *status = 0;
    if (SUCCEEDED(m_rowSet->QueryInterface(
            IID_IDBAsynchStatus, reinterpret_cast<void **>(&status)))) {
        status->Abort(DB_NULL_HCHAPTER, DBASYNCHOP_OPEN);
        status->Release();
    }
}

bool QWS4GalleryDocumentResponse::waitForFinished(int msecs)
{
    IDBAsynchStatus *status = 0;
    if (SUCCEEDED(m_rowSet->QueryInterface(
            IID_IDBAsynchStatus, reinterpret_cast<void **>(&status)))) {
        QMutexLocker locker(&m_asynchMutex);
        DBASYNCHPHASE phase = 0;

        status->GetStatus(DB_NULL_HCHAPTER, DBASYNCHOP_OPEN, 0, 0, &phase, 0);
        status->Release();

        if (phase == DBASYNCHPHASE_COMPLETE || phase == DBASYNCHPHASE_CANCELED) {
            return true;
        } else {
            if (m_asynchWait.wait(&m_asynchMutex, msecs)) {
                // there might be more to do here.
                return true;
            } else {
                return false;
            }
        }
    } else {
        return true;
    }
}

HRESULT QWS4GalleryDocumentResponse::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IDBAsynchNotify) {
        *ppvObject = static_cast<IDBAsynchNotify *>(this);
#ifdef __IRowsetEvents_INTERFACE_DEFINED__
    } else if (riid == IID_IRowsetEvents) {
        *ppvObject = static_cast<IRowsetEvents *>(this);
#endif
    } else {
        *ppvObject = 0;

        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG QWS4GalleryDocumentResponse::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QWS4GalleryDocumentResponse::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

// IDBAsynchNotify
HRESULT QWS4GalleryDocumentResponse::OnLowResource(DB_DWRESERVE)
{
    return E_NOTIMPL;
}

HRESULT QWS4GalleryDocumentResponse::OnProgress(
        HCHAPTER hChapter,
        DBASYNCHOP eOperation,
        DBCOUNTITEM ulProgress,
        DBCOUNTITEM ulProgressMax,
        DBASYNCHPHASE eAsynchPhase,
        LPOLESTR pwszStatusText)
{
    DBCOUNTITEM count = 0;
    HROW rows[20];
    HROW *pRows = rows;


    if (m_rowSet->GetNextRows(DB_NULL_HCHAPTER, 0, 20, &count, &pRows) == S_OK) {
        readRows(rows, count);

        m_rowSet->ReleaseRows(count, rows, 0, 0, 0);
    }

    return S_OK;
}

// IRowSetNotify
HRESULT QWS4GalleryDocumentResponse::OnStop(
        HCHAPTER hChapter, DBASYNCHOP eOperation, HRESULT hrStatus, LPOLESTR pwszStatusText)
{
    return S_OK;
}

HRESULT QWS4GalleryDocumentResponse::OnFieldChange(
        IRowset *pRowset,
        HROW hRow,
        DBORDINAL cColumns,
        DBORDINAL rgColumns[],
        DBREASON eReason,
        DBEVENTPHASE ePhase,
        BOOL fCantDeny)
{
    return DB_S_UNWANTEDREASON;
}

HRESULT QWS4GalleryDocumentResponse::OnRowChange(
        IRowset *pRowset,
        DBCOUNTITEM cRows,
        const HROW rghRows[],
        DBREASON eReason,
        DBEVENTPHASE ePhase,
        BOOL fCantDeny)
{
    if (eReason == DBREASON_ROW_INSERT || eReason == DBREASON_ROW_ASYNCHINSERT) {
        readRows(rghRows, cRows);

        return S_OK;
    } else {
        return DB_S_UNWANTEDREASON;
    }
}

HRESULT QWS4GalleryDocumentResponse::OnRowsetChange(
        IRowset *pRowset, DBREASON eReason, DBEVENTPHASE ePhase, BOOL fCantDeny)
{
    return DB_S_UNWANTEDREASON;
}

// IRowsetEvents
// Not usable prior to Windows 7, so just placeholder implementations for now.
#ifdef __IRowsetEvents_INTERFACE_DEFINED__
HRESULT QWS4GalleryDocumentResponse::OnNewItem(
        REFPROPVARIANT itemID, ROWSETEVENT_ITEMSTATE newItemState)
{
    Q_UNUSED(itemID);
    Q_UNUSED(newItemState);
}

HRESULT QWS4GalleryDocumentResponse::OnChangedItem(
        REFPROPVARIANT itemID,
        ROWSETEVENT_ITEMSTATE rowsetItemState,
        ROWSETEVENT_ITEMSTATE changedItemState)
{
    Q_UNUSED(itemID);
    Q_UNUSED(rowsetItemState);
    Q_UNUSED(changeItemState);

    qDebug(Q_FUNC_INFO);

    return S_OK;
}

HRESULT QWS4GalleryDocumentResponse::OnDeletedItem(
        REFPROPVARIANT itemID, ROWSETEVENT_ITEMSTATE deletedItemState)
{
    Q_UNUSED(itemID);
    Q_UNUSED(deletedItemState);

    qDebug(Q_FUNC_INFO);

    return S_OK;
}

HRESULT QWS4GalleryDocumentResponse::OnRowsetEvent(
    ROWSETEVENT_TYPE eventType, REFPROPVARIANT eventData)
{
    Q_UNUSED(eventType);
    Q_UNUSED(eventData);

    qDebug(Q_FUNC_INFO);

    return S_OK;
}
#endif

void QWS4GalleryDocumentResponse::customEvent(QEvent *event)
{
    if (event->type() == QEvent::User) {
        int index = m_rows.count();

        {
            QMutexLocker locker(&m_asynchMutex);
            m_rows += m_pendingRows;
            m_pendingRows.clear();
        }

        if (index != m_rows.count())
            emit inserted(index, m_rows.count() - index);
    } else {
        QGalleryAbstractResponse::customEvent(event);
    }
}

void QWS4GalleryDocumentResponse::readRows(const HROW rows[], DBCOUNTITEM count)
{
    QVector<Row> newRows;
    newRows.reserve(count);

    QByteArray buffer;
    buffer.resize(m_binding->bufferSize());

    for (DBCOUNTITEM i = 0; i < count; ++i) {
        if (SUCCEEDED(m_rowSet->GetData(rows[i], m_binding->handle(), buffer.data()))) {
            Row row = Row(new QWS4GalleryDocumentListRow);
            row->workId = 0;
            row->flags = 0;
            row->url = m_binding->toString(buffer.data(), m_urlIndex);
            row->type = m_binding->toString(buffer.data(), m_typeIndex);

            for (int i = 0; i < m_columnIndexes.count(); ++i)
                row->metaData.append(m_binding->toVariant(buffer.data(), m_columnIndexes.at(i)));

            newRows.append(row);
        }
    }

    QMutexLocker locker(&m_asynchMutex);

    if (m_pendingRows.isEmpty())
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));

    m_pendingRows += newRows;
}
