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
#include <QtCore/qthread.h>
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
    , m_startResult(S_OK)
    , m_mediaTypeToken(0)
    , m_id(QString::fromLatin1("reference"))
    , m_flushEvent(::CreateEvent(0, 0, 0, 0))
    , m_sampleScheduler(this)
    , m_flushing(false)
{
    connect(surface, SIGNAL(supportedFormatsChanged()), this, SLOT(supportedFormatsChanged()));
    connect(&m_sampleScheduler, SIGNAL(sampleReady()), this, SLOT(sampleReady()));
}

VideoSurfacePin::~VideoSurfacePin()
{
    Q_ASSERT(!m_allocator);
    Q_ASSERT(!m_peer);

    ::CloseHandle(m_flushEvent);

    Q_ASSERT(m_ref == 1);
}

void VideoSurfacePin::setState(FILTER_STATE state, REFERENCE_TIME time)
{
    QMutexLocker locker(&m_mutex);

    m_state = state;

    if (m_state ==  State_Stopped) {
        m_sampleScheduler.stop();

        m_surface->stop();

        ::SetEvent(m_flushEvent);
    } else if (m_state == State_Paused) {
        m_sampleScheduler.pause();
    } else if (m_state == State_Running) {
        m_sampleScheduler.run(time);
    }
}

void VideoSurfacePin::setClock(IReferenceClock *clock)
{
    QMutexLocker locker(&m_mutex);

    m_sampleScheduler.setClock(clock);
}

HRESULT VideoSurfacePin::QueryInterface(REFIID riid, void **ppvObject)
{
    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == IID_IUnknown
            || riid == IID_IPin) {
        *ppvObject = static_cast<IPin *>(this);
    } else if (riid == IID_IMemInputPin) {
        *ppvObject = static_cast<IMemInputPin *>(&m_sampleScheduler);
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
    } else {
        HRESULT hr;
        QMutexLocker locker(&m_mutex);

        if (m_peer) {
            hr = VFW_E_ALREADY_CONNECTED;
        } else if (pmt->majortype != MEDIATYPE_Video) {
            hr = VFW_E_TYPE_NOT_ACCEPTED;
        } else {
            m_surfaceFormat = VideoSurfaceMediaType::formatFromType(*pmt);
            m_bytesPerLine = VideoSurfaceMediaType::bytesPerLine(m_surfaceFormat);

            if (thread() == QThread::currentThread()) {
                hr = start();
            } else {
                QCoreApplication::postEvent(this, new QEvent(QEvent::Type(StartSurface)));

                ::WaitForSingleObject(m_flushEvent, INFINITE);
                ::ResetEvent(m_flushEvent);

                hr = m_startResult;
            }
        }
        if (hr == S_OK) {
           m_peer = pConnector;
           m_peer->AddRef();

           VideoSurfaceMediaType::copy(&m_mediaType, *pmt);
        }
        return hr;
    }
}

HRESULT VideoSurfacePin::start()
{
    if (!m_surface->start(m_surfaceFormat)) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    } else {
        return S_OK;
    }
}

HRESULT VideoSurfacePin::Disconnect()
{
    QMutexLocker locker(&m_mutex);

    if (!m_peer)
        return S_FALSE;

    if (thread() == QThread::currentThread()) {
        stop();
    } else {
        QCoreApplication::postEvent(this, new QEvent(QEvent::Type(StopSurface)));

        ::WaitForSingleObject(m_flushEvent, INFINITE);

        ::ResetEvent(m_flushEvent);
    }

    m_mediaType.clear();

    if (m_allocator) {
        m_allocator->Release();
        m_allocator = 0;
    }

    m_peer->Release();
    m_peer = 0;

    return S_OK;
}

void VideoSurfacePin::stop()
{
    m_surface->stop();
}

HRESULT VideoSurfacePin::ConnectedTo(IPin **ppPin)
{
    if (!ppPin) {
        return E_POINTER;
    } else {
        QMutexLocker locker(&m_mutex);

        if (!m_peer) {
            return VFW_E_NOT_CONNECTED;
        } else {
            m_peer->AddRef();

            *ppPin = m_peer;

            return S_OK;
        }
    }
}

HRESULT VideoSurfacePin::ConnectionMediaType(AM_MEDIA_TYPE *pmt)
{
    if (!pmt) {
        return E_POINTER;
    } else {
        QMutexLocker locker(&m_mutex);

        if (!m_peer) {
            return VFW_E_NOT_CONNECTED;
        } else {
            VideoSurfaceMediaType::copy(pmt, m_mediaType);

            return S_OK;
        }
    }
}

HRESULT VideoSurfacePin::QueryPinInfo(PIN_INFO *pInfo)
{
    if (!pInfo) {
        return E_POINTER;
    } else {
        // Don't lock, the filter and id are static.

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
        // Don't lock, the id is static.

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
        QMutexLocker locker(&m_mutex);

        *ppEnum = new VideoSurfaceMediaTypeEnum(this, m_mediaTypeToken);

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

    m_sampleScheduler.setFlushing(true);

    if (thread() == QThread::currentThread()) {
        flush();
    } else {
        QCoreApplication::postEvent(this, new QEvent(QEvent::Type(FlushSurface)));

        ::WaitForSingleObject(m_flushEvent, INFINITE);
    }

    return S_OK;
}

HRESULT VideoSurfacePin::EndFlush()
{
    QMutexLocker locker(&m_mutex);

    m_flushing = false;

    ::ResetEvent(m_flushEvent);

    m_sampleScheduler.setFlushing(false);

    return S_OK;
}

void VideoSurfacePin::flush()
{
    m_surface->present(QVideoFrame());

    ::SetEvent(m_flushEvent);
}

HRESULT VideoSurfacePin::NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
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

int VideoSurfacePin::currentMediaTypeToken()
{
    QMutexLocker locker(&m_mutex);

    return m_mediaTypeToken;
}

HRESULT VideoSurfacePin::nextMediaType(
        int token, int *index, ULONG count, AM_MEDIA_TYPE **types, ULONG *fetchedCount)
{
    QMutexLocker locker(&m_mutex);

    if (!types || (count != 1 && !fetchedCount)) {
        return E_POINTER;
    } else if (m_mediaTypeToken != token) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    } else {
        int boundedCount = qBound<int>(0, m_mediaTypes.count() - *index, count);

        for (int i = 0; i < boundedCount; ++i, ++(*index)) {
            types[i] = reinterpret_cast<AM_MEDIA_TYPE *>(CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE)));

            if (types[i]) {
                VideoSurfaceMediaType::copy(types[i], m_mediaTypes.at(*index));
            } else {
                for (--i; i >= 0; --i)
                    CoTaskMemFree(types[i]);

                if (fetchedCount)
                    *fetchedCount = 0;

                return E_OUTOFMEMORY;
            }
        }
        if (fetchedCount)
            *fetchedCount = boundedCount;

        return boundedCount == count ? S_OK : S_FALSE;
    }

}

HRESULT VideoSurfacePin::skipMediaType(int token, int *index, ULONG count)
{
    QMutexLocker locker(&m_mutex);

    if (m_mediaTypeToken != token) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    } else {
        *index = qMin<int>(*index + count, m_mediaTypes.size());

        return *index < m_mediaTypes.size() ? S_OK : S_FALSE;
    }
}

HRESULT VideoSurfacePin::cloneMediaType(int token, int index, IEnumMediaTypes **enumeration)
{
    QMutexLocker locker(&m_mutex);

    if (m_mediaTypeToken != token) {
        return VFW_E_ENUM_OUT_OF_SYNC;
    } else {
        *enumeration = new VideoSurfaceMediaTypeEnum(this, token, index);

        return S_OK;
    }
}

void VideoSurfacePin::customEvent(QEvent *event)
{
    if (event->type() == StartSurface) {
        m_startResult = start();
        ::SetEvent(m_flushEvent);
    } else if (event->type() == StopSurface) {
        stop();
        ::SetEvent(m_flushEvent);
    } else if (event->type() == FlushSurface) {
        flush();
    } else {
       QObject::customEvent(event);
    }
}

void VideoSurfacePin::supportedFormatsChanged()
{
    QMutexLocker locker(&m_mutex);

    m_mediaTypes.clear();

    QList<QVideoFrame::PixelFormat> formats = m_surface->supportedPixelFormats();

    m_mediaTypes.reserve(formats.count());

    AM_MEDIA_TYPE type;
    type.majortype = MEDIATYPE_Video;
    type.bFixedSizeSamples = TRUE;
    type.bTemporalCompression = FALSE;
    type.lSampleSize = 0;
    type.formattype = GUID_NULL;
    type.pUnk = 0;
    type.cbFormat = 0;
    type.pbFormat = 0;

    foreach (QVideoFrame::PixelFormat format, formats) {
        type.subtype = VideoSurfaceMediaType::convertPixelFormat(format);

        if (type.subtype != MEDIASUBTYPE_None)
            m_mediaTypes.append(type);
    }

    ++m_mediaTypeToken;
}

void VideoSurfacePin::sampleReady()
{
    IMediaSample *sample = m_sampleScheduler.takeSample();

    if (sample) {
        m_surface->present(QVideoFrame(
                new MediaSampleVideoBuffer(sample, m_bytesPerLine),
                m_surfaceFormat.frameSize(),
                m_surfaceFormat.pixelFormat()));

        sample->Release();
    }
}
