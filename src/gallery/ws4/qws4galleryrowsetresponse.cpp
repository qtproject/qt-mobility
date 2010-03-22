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

#include "qws4galleryrowsetresponse_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>

#include <oledberr.h>

QWS4GalleryRowSetResponse::QWS4GalleryRowSetResponse(IRowsetScroll *rowSet, QObject *parent)
    : QGalleryAbstractResponse(parent)
    , m_ref(1)
    , m_asynchNotifyCookie(0)
    , m_rowsetNotifyCookie(0)
    , m_rowSet(rowSet)
{
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
    }
}

QWS4GalleryRowSetResponse::~QWS4GalleryRowSetResponse()
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
        if (m_rowsetNotifyCookie != 0 && SUCCEEDED(
                container->FindConnectionPoint(IID_IRowsetNotify, &connection))) {
            connection->Unadvise(m_rowsetNotifyCookie);
            connection->Release();
        }
    }

    m_rowSet->Release();

    Q_ASSERT(m_ref == 1);
}

void QWS4GalleryRowSetResponse::cancel()
{
    IDBAsynchStatus *status = 0;
    if (SUCCEEDED(m_rowSet->QueryInterface(
            IID_IDBAsynchStatus, reinterpret_cast<void **>(&status)))) {
        status->Abort(DB_NULL_HCHAPTER, DBASYNCHOP_OPEN);
        status->Release();
    }
}

bool QWS4GalleryRowSetResponse::waitForFinished(int msecs)
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

// IUnknown
HRESULT QWS4GalleryRowSetResponse::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IDBAsynchNotify) {
        *ppvObject = static_cast<IDBAsynchNotify *>(this);
    } else if  (riid == IID_IRowsetNotify) {
        *ppvObject = static_cast<IRowsetNotify *>(this);
    } else {
        *ppvObject = 0;

        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG QWS4GalleryRowSetResponse::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QWS4GalleryRowSetResponse::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

// IDBAsynchNotify
HRESULT QWS4GalleryRowSetResponse::OnLowResource(DB_DWRESERVE)
{
    return E_NOTIMPL;
}

HRESULT QWS4GalleryRowSetResponse::OnProgress(
        HCHAPTER hChapter,
        DBASYNCHOP,
        DBCOUNTITEM ulProgress,
        DBCOUNTITEM ulProgressMax,
        DBASYNCHPHASE,
        LPOLESTR)
{
    HROW rows[PageSize];
    HROW *pRows = rows;

    int count = 0;

    for (DBCOUNTITEM rowsFetched = -1; rowsFetched != 0; count += rowsFetched) {
        if (SUCCEEDED(m_rowSet->GetNextRows(hChapter, 0, PageSize, &rowsFetched, &pRows)))
            m_rowSet->ReleaseRows(rowsFetched, rows, 0, 0, 0);
        else
            rowsFetched = 0;
    }

    if (count > 0)
        QCoreApplication::postEvent(this, new QWS4GalleryItemsInsertedEvent(count));

    QCoreApplication::postEvent(this, new QWS4GalleryProgressEvent(ulProgress, ulProgressMax));

    return S_OK;
}

HRESULT QWS4GalleryRowSetResponse::OnStop(HCHAPTER, DBASYNCHOP, HRESULT hrStatus, LPOLESTR)
{
    QCoreApplication::postEvent(this, new QWS4GalleryStopEvent(hrStatus));

    return S_OK;
}

// IRowsetNotify
HRESULT QWS4GalleryRowSetResponse::OnFieldChange(
        IRowset *, HROW, DBORDINAL, DBORDINAL[], DBREASON, DBEVENTPHASE, BOOL)
{
    return DB_S_UNWANTEDREASON;
}

HRESULT QWS4GalleryRowSetResponse::OnRowChange(
        IRowset *, DBCOUNTITEM, const HROW[], DBREASON, DBEVENTPHASE, BOOL)
{
    return DB_S_UNWANTEDREASON;
}

HRESULT QWS4GalleryRowSetResponse::OnRowsetChange(IRowset *, DBREASON, DBEVENTPHASE, BOOL)
{
    return DB_S_UNWANTEDREASON;
}

void QWS4GalleryRowSetResponse::customEvent(QEvent *event)
{
    if (event->type() == QWS4GalleryEvent::Progress) {
        QWS4GalleryProgressEvent *progressEvent = static_cast<QWS4GalleryProgressEvent *>(event);

        emit progressChanged(progressEvent->currentProgress, progressEvent->maximumProgress);
    } else if (event->type() == QWS4GalleryEvent::Stop) {
        QWS4GalleryStopEvent *stopEvent = static_cast<QWS4GalleryStopEvent *>(event);

        if (SUCCEEDED(stopEvent->result)) {
            finish(QGalleryAbstractRequest::Succeeded);
        } else {
            finish(QGalleryAbstractRequest::ConnectionError);
        }
    }
}