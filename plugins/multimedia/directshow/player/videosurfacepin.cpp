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
#include "videosurfacepin.h"

#include "mediasamplevideobuffer.h"
#include "videosurfacemediatypeenum.h"
#include "videosurfacefilter.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>
#include <QtMultimedia/qabstractvideosurface.h>

#include <QtDebug>

VideoSurfacePin::VideoSurfacePin(VideoSurfaceFilter *filter, QAbstractVideoSurface *surface)
    : m_ref(1)
    , m_state(State_Stopped)
    , m_startTime(0)
    , m_filter(filter)
    , m_surface(surface)
    , m_peer(0)
    , m_allocator(0)
    , m_clock(0)
    , m_id(QString::fromLatin1("reference"))
    , m_renderEvent(::CreateEvent(0, 0, 0, 0))
    , m_flushEvent(::CreateEvent(0, 0, 0, 0))
    , m_flushing(false)
{
}

VideoSurfacePin::~VideoSurfacePin()
{
    Q_ASSERT(!m_allocator);
    Q_ASSERT(!m_peer);


    ::CloseHandle(m_renderEvent);
    ::CloseHandle(m_flushEvent);

    Q_ASSERT(m_ref == 1);
}

void VideoSurfacePin::setState(FILTER_STATE state, REFERENCE_TIME time)
{
    QMutexLocker locker(&m_mutex);

    m_state = state;

    if (m_state ==  State_Stopped) {
        m_startTime = 0;

        m_surface->stop();

        ::SetEvent(m_flushEvent);
    } else if (m_state == State_Paused) {
        ::ResetEvent(m_flushEvent);
    } else if (m_state == State_Running) {
        m_startTime = time;
        ::SetEvent(m_flushEvent);
    }
}

void VideoSurfacePin::setClock(IReferenceClock *clock)
{
    QMutexLocker locker(&m_mutex);

    m_clock = clock;
}

HRESULT VideoSurfacePin::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IPin) {
        *ppvObject = static_cast<IPin *>(this);
    } else if (riid == IID_IMemInputPin) {
        *ppvObject = static_cast<IMemInputPin *>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG VideoSurfacePin::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG VideoSurfacePin::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

HRESULT VideoSurfacePin::Connect(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt)
{
    // This is an input pin, you shouldn't be calling Connect on it.
    return E_POINTER;
}

HRESULT VideoSurfacePin::ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt)
{
    if (!pConnector) {
        return E_POINTER;
    } else if (!pmt) {
        return E_POINTER;
    } else if (m_peer) {
        return VFW_E_ALREADY_CONNECTED;
    } else if (pmt->majortype != MEDIATYPE_Video) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    } else {
        m_surfaceFormat = VideoSurfaceMediaType::formatFromType(*pmt);
        m_bytesPerLine = VideoSurfaceMediaType::bytesPerLine(m_surfaceFormat);

        if (!m_surface->start(m_surfaceFormat)) {
            return VFW_E_TYPE_NOT_ACCEPTED;
        } else {
            m_peer = pConnector;
            m_peer->AddRef();

            VideoSurfaceMediaType::copy(&m_mediaType, *pmt);

            return S_OK;
        }
    }
}

HRESULT VideoSurfacePin::Disconnect()
{
    if (!m_peer)
        return S_FALSE;

    m_mediaType.clear();

    if (m_allocator) {
        m_allocator->Release();
        m_allocator = 0;
    }

    m_peer->Release();
    m_peer = 0;

    m_surface->stop();

    return S_OK;
}

HRESULT VideoSurfacePin::ConnectedTo(IPin **ppPin)
{
    if (!ppPin) {
        return E_POINTER;
    } else if (!m_peer) {
        return VFW_E_NOT_CONNECTED;
    } else {
        m_peer->AddRef();

        *ppPin = m_peer;

        return S_OK;
    }
}

HRESULT VideoSurfacePin::ConnectionMediaType(AM_MEDIA_TYPE *pmt)
{
    if (!pmt) {
        return E_POINTER;
    } else if (!m_peer) {
        return VFW_E_NOT_CONNECTED;
    } else {
        VideoSurfaceMediaType::copy(pmt, m_mediaType);

        return S_OK;
    }
}

HRESULT VideoSurfacePin::QueryPinInfo(PIN_INFO *pInfo)
{
    if (!pInfo) {
        return E_POINTER;
    } else {
        m_filter->AddRef();

        pInfo->pFilter = m_filter;
        pInfo->dir = PINDIR_INPUT;

        qMemCopy(pInfo->achName, m_id.utf16(), qMin(MAX_FILTER_NAME, (m_id.length() + 1) * 2));

        return S_OK;
    }
}

HRESULT VideoSurfacePin::QueryId(LPWSTR *Id)
{
    if (!Id) {
        return E_POINTER;
    } else {
        int bytes = (m_id.length() + 1) * 2;

        *Id = static_cast<LPWSTR>(::CoTaskMemAlloc(bytes));

        qMemCopy(*Id, m_id.utf16(), bytes);

        return S_OK;
    }
}

HRESULT VideoSurfacePin::QueryAccept(const AM_MEDIA_TYPE *pmt)
{
    return !m_surface->isFormatSupported(VideoSurfaceMediaType::formatFromType(*pmt))
            ? S_OK
            : S_FALSE;
}

HRESULT VideoSurfacePin::EnumMediaTypes(IEnumMediaTypes **ppEnum)
{
    if (!ppEnum) {
        return E_POINTER;
    } else {
        *ppEnum = new VideoSurfaceMediaTypeEnum(m_surface->supportedPixelFormats());

        return S_OK;
    }
}

HRESULT VideoSurfacePin::QueryInternalConnections(IPin **apPin, ULONG *nPin)
{
    Q_UNUSED(apPin);
    Q_UNUSED(nPin);

    return E_NOTIMPL;
}

HRESULT VideoSurfacePin::EndOfStream()
{
    QMutexLocker locker(&m_mutex);

    m_flushing = true;

    return S_OK;
}

HRESULT VideoSurfacePin::BeginFlush()
{
    QMutexLocker locker(&m_mutex);

    m_pendingFrame = QVideoFrame();
    m_surface->present(QVideoFrame());

    ::SetEvent(m_flushEvent);

    return S_OK;
}

HRESULT VideoSurfacePin::EndFlush()
{
    QMutexLocker locker(&m_mutex);

    m_flushing = false;
    
    ::ResetEvent(m_flushEvent);

    return S_OK;
}

HRESULT VideoSurfacePin::NewSegment(
        REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
    return S_OK;
}

HRESULT VideoSurfacePin::QueryDirection(PIN_DIRECTION *pPinDir)
{
    if (!pPinDir) {
        return E_POINTER;
    } else {
        *pPinDir = PINDIR_INPUT;

        return S_OK;
    }
}

HRESULT VideoSurfacePin::GetAllocator(IMemAllocator **ppAllocator)
{
    QMutexLocker locker(&m_mutex);

    if (!ppAllocator) {
        return E_POINTER;
    } else if (!m_allocator) {
        return VFW_E_NO_ALLOCATOR;
    } else {
        *ppAllocator = m_allocator;

        return S_OK;
    }
}

HRESULT VideoSurfacePin::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
{
    Q_UNUSED(bReadOnly);

    QMutexLocker locker(&m_mutex);

    HRESULT hr;
    ALLOCATOR_PROPERTIES properties;

    if (!pAllocator) {
        return E_POINTER;
    } else if ((hr = pAllocator->GetProperties(&properties)) != S_OK) {
        return hr;
    } else {
        if (properties.cBuffers == 1) {
            ALLOCATOR_PROPERTIES actual;

            properties.cBuffers = 2;
            if ((hr = pAllocator->SetProperties(&properties, &actual)) != S_OK)
                return hr;
        }
        if (m_allocator)
            m_allocator->Release();

        m_allocator = pAllocator;
        m_allocator->AddRef();

        return S_OK;
    }
}

HRESULT VideoSurfacePin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps)
{
    if (!pProps)
        return E_POINTER;

    pProps->cBuffers = 2;

    return S_OK;
}

HRESULT VideoSurfacePin::Receive(IMediaSample *pSample)
{
    if (!pSample)
        return E_POINTER;

    bool running = false;
    bool paused = false;

    {
        QMutexLocker locker(&m_mutex);

        if (m_flushing) {
            return S_FALSE;
        } else if (m_state == State_Stopped) {
            return VFW_E_WRONG_STATE;
        } else {
            // Should be checking frame format.

            m_pendingFrame = QVideoFrame(
                    new MediaSampleVideoBuffer(pSample, m_bytesPerLine),
                    m_surfaceFormat.frameSize(),
                    m_surfaceFormat.pixelFormat());

            REFERENCE_TIME start = 0;
            REFERENCE_TIME end = 0;
            if (m_clock && m_state == State_Running && pSample->GetTime(&start, &end) == S_OK) {
                DWORD_PTR advise;
                m_clock->AdviseTime(m_startTime, start, reinterpret_cast<HEVENT>(m_renderEvent), &advise);
                running = true;
            } else if (m_state == State_Paused) {
                paused = true;
            }
        }
    }

    if (running) {
        HANDLE handles[] = { m_flushEvent, m_renderEvent };

        DWORD index = ::WaitForMultipleObjects(2, handles, false, INFINITE);

        QMutexLocker locker(&m_mutex);

        if (index == WAIT_OBJECT_0) {
            if (m_flushing) {
                return S_FALSE;
            } else if (m_state == State_Stopped) {
                return VFW_E_WRONG_STATE;
            } else {
                ::ResetEvent(m_flushEvent);
            }
        } else {
            paused = m_state == State_Paused;
        }
    }

    QCoreApplication::postEvent(this, new QEvent(QEvent::User));

    if (paused) {
        ::WaitForSingleObject(m_flushEvent, INFINITE);

        QMutexLocker locker(&m_mutex);

        if (m_state != State_Stopped && !m_flushing)
            ::ResetEvent(m_flushEvent);
    }

    return S_OK;
}

HRESULT VideoSurfacePin::ReceiveMultiple(
        IMediaSample **pSamples, long nSamples, long *nSamplesProcessed)
{
    if (!pSamples || !nSamplesProcessed)
        return E_POINTER;

    for (*nSamplesProcessed = 0; *nSamplesProcessed < nSamples; ++(*nSamplesProcessed)) {
        HRESULT hr = Receive(pSamples[*nSamplesProcessed]);

        if (hr != S_OK)
            return hr;
    }
    return S_OK;
}

HRESULT VideoSurfacePin::ReceiveCanBlock()
{
    return S_OK;
}

void VideoSurfacePin::customEvent(QEvent *event)
{
    if (event->type() == QEvent::User) {
        QMutexLocker locker(&m_mutex);

        m_surface->present(m_pendingFrame);
        
        m_pendingFrame = QVideoFrame();
    }
}
