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

#include "qgalleryrowsetresponse_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>

#include <oledberr.h>

namespace QWindowsSearch
{

QGalleryRowSetResponse::QGalleryRowSetResponse(IRowsetScroll *rowSet, QObject *parent)
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

QGalleryRowSetResponse::~QGalleryRowSetResponse()
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

void QGalleryRowSetResponse::cancel()
{
    IDBAsynchStatus *status = 0;
    if (SUCCEEDED(m_rowSet->QueryInterface(
            IID_IDBAsynchStatus, reinterpret_cast<void **>(&status)))) {
        status->Abort(DB_NULL_HCHAPTER, DBASYNCHOP_OPEN);
        status->Release();
    }
}

bool QGalleryRowSetResponse::waitForFinished(int msecs)
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
HRESULT QGalleryRowSetResponse::QueryInterface(REFIID riid, void **ppvObject)
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

ULONG QGalleryRowSetResponse::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QGalleryRowSetResponse::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

// IDBAsynchNotify
HRESULT QGalleryRowSetResponse::OnLowResource(DB_DWRESERVE)
{
    return E_NOTIMPL;
}

HRESULT QGalleryRowSetResponse::OnProgress(
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
        QCoreApplication::postEvent(this, new QGalleryItemsInsertedEvent(count));

    QCoreApplication::postEvent(this, new QGalleryProgressEvent(ulProgress, ulProgressMax));

    return S_OK;
}

HRESULT QGalleryRowSetResponse::OnStop(HCHAPTER, DBASYNCHOP, HRESULT hrStatus, LPOLESTR)
{
    QCoreApplication::postEvent(this, new QGalleryStopEvent(hrStatus));

    return S_OK;
}

// IRowsetNotify
HRESULT QGalleryRowSetResponse::OnFieldChange(
        IRowset *, HROW, DBORDINAL, DBORDINAL[], DBREASON, DBEVENTPHASE, BOOL)
{
    return DB_S_UNWANTEDREASON;
}

HRESULT QGalleryRowSetResponse::OnRowChange(
        IRowset *, DBCOUNTITEM, const HROW[], DBREASON, DBEVENTPHASE, BOOL)
{
    return DB_S_UNWANTEDREASON;
}

HRESULT QGalleryRowSetResponse::OnRowsetChange(IRowset *, DBREASON, DBEVENTPHASE, BOOL)
{
    return DB_S_UNWANTEDREASON;
}

void QGalleryRowSetResponse::customEvent(QEvent *event)
{
    if (event->type() == QGalleryEvent::Progress) {
        QGalleryProgressEvent *progressEvent = static_cast<QGalleryProgressEvent *>(event);

        emit progressChanged(progressEvent->currentProgress, progressEvent->maximumProgress);
    } else if (event->type() == QGalleryEvent::Stop) {
        QGalleryStopEvent *stopEvent = static_cast<QGalleryStopEvent *>(event);

        if (SUCCEEDED(stopEvent->result)) {
            finish(QGalleryAbstractRequest::Succeeded);
        } else {
            finish(QGalleryAbstractRequest::ConnectionError);
        }
    }
}

}
