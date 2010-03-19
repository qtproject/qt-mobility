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

#include <QtCore/qdebug.h>
#include <QWidget>
#include <QFile>

#include "dscamerasession.h"
#include "dsvideorenderer.h"

#include <QtMultimedia/qabstractvideobuffer.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#include <uuids.h>
DEFINE_GUID(MEDIASUBTYPE_I420,
        0x30323449,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);

#define SAFE_RELEASE(x) { if(x) x->Release(); x = NULL; }


class SampleGrabberCallbackPrivate : public ISampleGrabberCB
{
public:
    STDMETHODIMP_(ULONG) AddRef() { return 1; }
    STDMETHODIMP_(ULONG) Release() { return 2; }

    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
    {
        if(NULL==ppvObject) return E_POINTER;
        if(riid==__uuidof(IUnknown)) {
            *ppvObject = static_cast<IUnknown*>(this);
            return S_OK;
        }
        if(riid==__uuidof(ISampleGrabberCB)) {
            *ppvObject = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen)
    {
        if(!cs || active)
            return S_OK;

        if((cs->StillMediaType.majortype != MEDIATYPE_Video) ||
                (cs->StillMediaType.formattype != FORMAT_VideoInfo) ||
                (cs->StillMediaType.cbFormat < sizeof(VIDEOINFOHEADER)))
            return VFW_E_INVALIDMEDIATYPE;

        active = true;

        if(toggle == true) toggle = false;
        else toggle = true;

        if(toggle) {
            active = false;
            return S_OK;
        }

        bool check = false;
        cs->mutex.lock();
        if(cs->frames.size() > 5)
            check = true;
        cs->mutex.unlock();
        if(check)
            return S_OK;


        unsigned char* vidData = new unsigned char[BufferLen];
        memcpy(vidData,pBuffer,BufferLen);

        cs->mutex.lock();

        video_buffer buf;
        buf.buffer = vidData;
        buf.length = BufferLen;
        buf.time   = (qint64)Time;

        cs->frames.append(&buf);

        //qWarning()<<"create frame "<<buf.buffer<<", len="<<buf.length<<" buffered="<<cs->frames.size();

        cs->mutex.unlock();

        QMetaObject::invokeMethod(cs, "captureFrame", Qt::QueuedConnection);

        active = false;

        return S_OK;
    }

    DSCameraSession* cs;
    bool active;
    bool toggle;
};


DSCameraSession::DSCameraSession(QObject *parent)
    :QObject(parent)
{
    opened = false;
    available = false;
    resolutions.clear();
    m_state = QCamera::StoppedState;
    m_device = "default";

    StillCapCB = new SampleGrabberCallbackPrivate;
    StillCapCB->cs = this;
    StillCapCB->active = false;
    StillCapCB->toggle = false;

    m_output = 0;
    m_surface = 0;
    m_windowSize = QSize(320,240);
    pixelF = QVideoFrame::Format_RGB24;
    actualFormat = QVideoSurfaceFormat(m_windowSize,pixelF);

    graph = false;
}

DSCameraSession::~DSCameraSession()
{
    if(opened)
        closeStream();

    CoUninitialize();

    SAFE_RELEASE(pCap);
    SAFE_RELEASE(pSG_Filter);
    SAFE_RELEASE(pGraph);
    SAFE_RELEASE(pBuild);

    if(StillCapCB)
        delete StillCapCB;
}

void DSCameraSession::captureImage(const QString &fileName)
{
    m_snapshot = fileName;
}

void DSCameraSession::cancelCapture()
{
}

void DSCameraSession::setSurface(QAbstractVideoSurface* surface)
{
    m_surface = surface;
}

bool DSCameraSession::deviceReady()
{
    return available;
}

int DSCameraSession::framerate() const
{
    return -1;
}

void DSCameraSession::setFrameRate(int rate)
{
    Q_UNUSED(rate)
}

int DSCameraSession::brightness() const
{
    return -1;
}

void DSCameraSession::setBrightness(int b)
{
    Q_UNUSED(b)
}

int DSCameraSession::contrast() const
{
    return -1;
}

void DSCameraSession::setContrast(int c)
{
    Q_UNUSED(c)
}

int DSCameraSession::saturation() const
{
    return -1;
}

void DSCameraSession::setSaturation(int s)
{
    Q_UNUSED(s)
}

int DSCameraSession::hue() const
{
    return -1;
}

void DSCameraSession::setHue(int h)
{
    Q_UNUSED(h)
}

int DSCameraSession::sharpness() const
{
    return -1;
}

void DSCameraSession::setSharpness(int s)
{
    Q_UNUSED(s)
}

int DSCameraSession::zoom() const
{
    return -1;
}

void DSCameraSession::setZoom(int z)
{
    Q_UNUSED(z)
}

bool DSCameraSession::backlightCompensation() const
{
    return false;
}

void DSCameraSession::setBacklightCompensation(bool b)
{
    Q_UNUSED(b)
}

int DSCameraSession::whitelevel() const
{
    return -1;
}

void DSCameraSession::setWhitelevel(int w)
{
    Q_UNUSED(w)
}

int DSCameraSession::rotation() const
{
    return 0;
}

void DSCameraSession::setRotation(int r)
{
    Q_UNUSED(r)
}

bool DSCameraSession::flash() const
{
    return false;
}

void DSCameraSession::setFlash(bool f)
{
    Q_UNUSED(f)
}

bool DSCameraSession::autofocus() const
{
    return false;
}

void DSCameraSession::setAutofocus(bool f)
{
    Q_UNUSED(f)
}

QSize DSCameraSession::frameSize() const
{
    return m_windowSize;
}

void DSCameraSession::setFrameSize(const QSize& s)
{
    if(supportedResolutions(pixelF).contains(s))
        m_windowSize = s;
    else {
        qWarning()<<"frame size if not supported for current pixel format, no change";
    }
}

void DSCameraSession::setDevice(const QString &device)
{
    if(opened)
        stopStream();

    if(graph) {
        SAFE_RELEASE(pCap);
        SAFE_RELEASE(pSG_Filter);
        SAFE_RELEASE(pGraph);
        SAFE_RELEASE(pBuild);
    }

    //qWarning()<<"setDevice: "<<device;

    available = false;
    m_state = QCamera::StoppedState;

    CoInitialize(NULL);

    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;

    // Create the System device enumerator
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
            CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
            reinterpret_cast<void**>(&pDevEnum));
    if(SUCCEEDED(hr)) {
        // Create the enumerator for the video capture category
        hr = pDevEnum->CreateClassEnumerator(
                CLSID_VideoInputDeviceCategory, &pEnum, 0);
        pEnum->Reset();
        // go through and find all video capture devices
        IMoniker* pMoniker = NULL;
        while(pEnum->Next(1, &pMoniker, NULL) == S_OK) {
            IPropertyBag *pPropBag;
            hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,
                    (void**)(&pPropBag));
            if(FAILED(hr)) {
                pMoniker->Release();
                continue; // skip this one
            }
            // Find the description
            WCHAR str[120];
            VARIANT varName;
            varName.vt = VT_BSTR;
            hr = pPropBag->Read(L"Description", &varName, 0);
            if(FAILED(hr))
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            if(SUCCEEDED(hr)) {
                hr = StringCchCopyW(str,sizeof(str)/sizeof(str[0]), varName.bstrVal);
                QString temp(QString::fromUtf16((unsigned short*)str));
                if(temp.contains(device)) {
                    //qWarning()<<"device valid use it: "<<device;
                    available = true;
                }
            }
            pPropBag->Release();
            pMoniker->Release();
        }
    }
    CoUninitialize();

    if(available) {
        m_device = QByteArray(device.toLocal8Bit().constData());
        graph = createFilterGraph();
        if(!graph)
            available = false;
    }
}

QList<QVideoFrame::PixelFormat> DSCameraSession::supportedPixelFormats()
{
    return types;
}

QVideoFrame::PixelFormat DSCameraSession::pixelFormat() const
{
    return pixelF;
}

void DSCameraSession::setPixelFormat(QVideoFrame::PixelFormat fmt)
{
    pixelF = fmt;
}

QList<QSize> DSCameraSession::supportedResolutions(QVideoFrame::PixelFormat format)
{
    if(!resolutions.contains(format)) return QList<QSize>();
    return resolutions.value(format);
}

bool DSCameraSession::setOutputLocation(const QUrl &sink)
{
    m_sink = sink;

    return true;
}

QUrl DSCameraSession::outputLocation() const
{
    return m_sink;
}

qint64 DSCameraSession::position() const
{
    return timeStamp.elapsed();
}

int DSCameraSession::state() const
{
    return int(m_state);
}

void DSCameraSession::record()
{
    if(opened) return;

    if(m_surface) {
        bool match = false;

        if(!m_surface->isFormatSupported(actualFormat)) {
            QList<QVideoFrame::PixelFormat> fmts;
            foreach(QVideoFrame::PixelFormat f, types) {
                if(fmts.contains(f)) {
                    match = true;
                    pixelF = f;
                    actualFormat = QVideoSurfaceFormat(m_windowSize,pixelF);
                    //qWarning()<<"try to use format: "<<pixelF;
                    break;
                }
            }
        }
        if(!m_surface->isFormatSupported(actualFormat) && !match) {
            // fallback
            if(types.contains(QVideoFrame::Format_RGB24)) {
                // get RGB24 from camera and convert to RGB32 for surface!
                pixelF = QVideoFrame::Format_RGB32;
                actualFormat = QVideoSurfaceFormat(m_windowSize,pixelF);
                //qWarning()<<"get RGB24 from camera and convert to RGB32 for surface!";
            }
        }

        if(m_surface->isFormatSupported(actualFormat)) {
            m_surface->start(actualFormat);
            m_state = QCamera::ActiveState;
            emit stateChanged(QCamera::ActiveState);
        } else {
            qWarning()<<"surface doesn't support camera format, cant start";
            m_state = QCamera::StoppedState;
            emit stateChanged(QCamera::StoppedState);
            return;
        }
    } else {
        qWarning()<<"no video surface, cant start";
        m_state = QCamera::StoppedState;
        emit stateChanged(QCamera::StoppedState);
        return;
    }

    opened = startStream();

    if(!opened) {
        m_state = QCamera::StoppedState;
        emit stateChanged(QCamera::StoppedState);
    }
}

void DSCameraSession::pause()
{
    suspendStream();
}

void DSCameraSession::stop()
{
    if(!opened) return;

    stopStream();
    opened = false;
    m_state = QCamera::StoppedState;
    emit stateChanged(QCamera::StoppedState);
}

void DSCameraSession::captureFrame()
{
    if(m_surface && frames.count() > 0) {

        QImage image;

        if(pixelF == QVideoFrame::Format_RGB24) {

            mutex.lock();

            image = QImage(frames.at(0)->buffer,m_windowSize.width(),m_windowSize.height(),
                    QImage::Format_RGB888).rgbSwapped().mirrored(true);

            QVideoFrame frame(image);
            frame.setStartTime(frames.at(0)->time);

            mutex.unlock();

            m_surface->present(frame);

        } else if(pixelF == QVideoFrame::Format_RGB32) {

            mutex.lock();

            image = QImage(frames.at(0)->buffer,m_windowSize.width(),m_windowSize.height(),
                    QImage::Format_RGB888).rgbSwapped().mirrored(true);

            QVideoFrame frame(image.convertToFormat(QImage::Format_RGB32));
            frame.setStartTime(frames.at(0)->time);

            mutex.unlock();

            m_surface->present(frame);

        } else {
            qWarning()<<"TODO:captureFrame() format ="<<pixelF;
        }

        if(m_snapshot.length() > 0) {
            emit imageCaptured(m_snapshot,image);
            image.save(m_snapshot,"JPG");
            m_snapshot.clear();
        }

        frames.removeFirst();
    }
}

HRESULT DSCameraSession::GetPin(IBaseFilter *pFilter,PIN_DIRECTION PinDir, IPin **ppPin)
{
    *ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr))
        return hr;
    pEnum->Reset();
    while(pEnum->Next(1,&pPin,NULL) == S_OK) {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if(ThisPinDir == PinDir) {
            pEnum->Release();
            *ppPin = pPin;
            return S_OK;
        }
        pEnum->Release();
    }
    pEnum->Release();
    return E_FAIL;
}

bool DSCameraSession::createFilterGraph()
{
    HRESULT hr;
    IMoniker* pMoniker = NULL;
    ICreateDevEnum* pDevEnum = NULL;
    IEnumMoniker* pEnum = NULL;
    AM_MEDIA_TYPE am_media_type;

    CoInitialize(NULL);

    // Create the filter graph
    hr = CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC,
            IID_IGraphBuilder, (void**)&pGraph);
    if(FAILED(hr)) {
        qWarning()<<"failed to create filter graph";
        return false;
    }
    // Create the capture graph builder
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC,
            IID_ICaptureGraphBuilder2, (void**)&pBuild);
    if(FAILED(hr)) {
        qWarning()<<"failed to create graph builder";
        return false;
    }
    // Attach the filter graph to the capture graph
    hr = pBuild->SetFiltergraph(pGraph);
    if(FAILED(hr)) {
        qWarning()<<"failed to connect capture graph and filter graph";
        return false;
    }
    // Find the Capture device
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
            CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
            reinterpret_cast<void**>(&pDevEnum));
    if(SUCCEEDED(hr)) {
        // Create an enumerator for the video capture category
        hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
        pDevEnum->Release();
        pEnum->Reset();
        //go through and find all video capture devices
        while(pEnum->Next(1, &pMoniker, NULL) == S_OK) {
            IPropertyBag *pPropBag;
            hr = pMoniker->BindToStorage(0,0,IID_IPropertyBag,(void**)(&pPropBag));
            if(FAILED(hr)) {
                pMoniker->Release();
                continue; // skip this one
            }
            // Find the description
            WCHAR str[120];
            VARIANT varName;
            varName.vt = VT_BSTR;
            hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            if(SUCCEEDED(hr)) {
                // check if it is the selected device
                hr = StringCchCopyW(str,sizeof(str)/sizeof(str[0]), varName.bstrVal);
                QString output = QString::fromUtf16((unsigned short*)str);
                if(m_device.contains(output.toLocal8Bit().constData())) {
                    hr = pMoniker->BindToObject(0,0,IID_IBaseFilter,(void**)&pCap);
                    if(SUCCEEDED(hr)) {
                        pPropBag->Release();
                        pMoniker->Release();
                        break;
                    }
                }
            }
            pPropBag->Release();
            pMoniker->Release();
        }
    }
    pEnum->Release();

    // Get capture device settings
    hr = pBuild->FindInterface(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,pCap,
                        IID_IAMStreamConfig,(void**)&pConfig);
    if(FAILED(hr)) {
        qWarning()<<"failed to get config on capture device";
        return false;
    }

    updateProperties();

    // Sample grabber filter
    hr = CoCreateInstance(CLSID_SampleGrabber,NULL,CLSCTX_INPROC,IID_IBaseFilter,(void**)&pSG_Filter);
    if(FAILED(hr)) {
        qWarning()<<"failed to create sample grabber";
        return false;
    }
    pSG_Filter->QueryInterface(IID_ISampleGrabber,(void**)&pSG);
    if(FAILED(hr)) {
        qWarning()<<"failed to get sample grabber";
        return false;
    }
    pSG->SetOneShot(FALSE);
    pSG->SetBufferSamples(TRUE);
    pSG->SetCallback(StillCapCB, 1);

    // Get stream control
    hr = pGraph->QueryInterface(IID_IMediaControl,(void**)&pControl);
    if(FAILED(hr)) {
        qWarning()<<"failed to get stream control";
        return false;
    }
    hr = pCap->QueryInterface(IID_IAMVideoControl,(void**)&pVideoControl);
    if(FAILED(hr)) {
        qWarning()<<"failed to get video control handle";
        return false;
    }
    hr = pCap->QueryInterface(IID_IAMVideoProcAmp,(void**)&pProcAmp);
    if(FAILED(hr)) {
        qWarning()<<"failed to get video settings handle";
        return false;
    }
    CoUninitialize();

    return true;
}

void DSCameraSession::updateProperties()
{
    HRESULT hr;
    AM_MEDIA_TYPE *pmt = NULL;
    VIDEOINFOHEADER *pvi = NULL;
    VIDEO_STREAM_CONFIG_CAPS scc;
    int iCount, iSize;
    hr = pConfig->GetNumberOfCapabilities(&iCount,&iSize);
    if(FAILED(hr)) {
        qWarning()<<"failed to get capabilities";
        return;
    }

    QList<QSize> sizes;
    QVideoFrame::PixelFormat f;

    types.clear();
    resolutions.clear();

    for(int i=0;i<iCount;i++) {
        hr = pConfig->GetStreamCaps(i,&pmt,reinterpret_cast<BYTE*>(&scc));
        if(hr == S_OK) {
            pvi = (VIDEOINFOHEADER*)pmt->pbFormat;
            if((pmt->majortype == MEDIATYPE_Video) &&
                    (pmt->formattype == FORMAT_VideoInfo)) {
                // Add types
                if(pmt->subtype == MEDIASUBTYPE_RGB24) {
                    if(!types.contains(QVideoFrame::Format_RGB24)) {
                        types.append(QVideoFrame::Format_RGB24);
                        f = QVideoFrame::Format_RGB24;
                        //qWarning()<<"camera supports RGB24";
                    }
                } else if(pmt->subtype == MEDIASUBTYPE_RGB32) {
                    if(!types.contains(QVideoFrame::Format_RGB32)) {
                        types.append(QVideoFrame::Format_RGB32);
                        f = QVideoFrame::Format_RGB32;
                        //qWarning()<<"camera supports RGB32";
                    }
                } else if(pmt->subtype == MEDIASUBTYPE_YUY2) {
                    if(!types.contains(QVideoFrame::Format_YUYV)) {
                        types.append(QVideoFrame::Format_YUYV);
                        f = QVideoFrame::Format_YUYV;
                        //qWarning()<<"camera supports YUY2";
                    }
                } else if(pmt->subtype == MEDIASUBTYPE_MJPG) {
                    //qWarning("MJPG format not supported");

                } else if(pmt->subtype == MEDIASUBTYPE_I420) {
                    if(!types.contains(QVideoFrame::Format_YUV420P)) {
                        types.append(QVideoFrame::Format_YUV420P);
                        f = QVideoFrame::Format_YUV420P;
                        //qWarning()<<"camera supports YUV420P";
                    }
                } else if(pmt->subtype == MEDIASUBTYPE_RGB555) {
                    if(!types.contains(QVideoFrame::Format_RGB555)) {
                        types.append(QVideoFrame::Format_RGB555);
                        f = QVideoFrame::Format_RGB555;
                        //qWarning()<<"camera supports RGB555";
                    }
                } else if(pmt->subtype == MEDIASUBTYPE_YVU9) {
                    //qWarning("YVU9 format not supported");

                } else if(pmt->subtype == MEDIASUBTYPE_UYVY) {
                    if(!types.contains(QVideoFrame::Format_UYVY)) {
                        types.append(QVideoFrame::Format_UYVY);
                        f = QVideoFrame::Format_UYVY;
                        //qWarning()<<"camera supports UYVY";
                    }
                } else {
                    qWarning()<<"UNKNOWN FORMAT: "<<pmt->subtype.Data1;
                }
                // Add resolutions
                QSize res(pvi->bmiHeader.biWidth,pvi->bmiHeader.biHeight);
                if(!resolutions.contains(f)) {
                    sizes.clear();
                    resolutions.insert(f,sizes);
                }
                resolutions[f].append(res);
            }
        }
    }
}

bool DSCameraSession::setProperties()
{
    CoInitialize(NULL);

    HRESULT hr;
    AM_MEDIA_TYPE am_media_type;
    AM_MEDIA_TYPE *pmt = NULL;
    VIDEOINFOHEADER *pvi = NULL;
    VIDEO_STREAM_CONFIG_CAPS scc;
    int iCount, iSize;
    hr = pConfig->GetNumberOfCapabilities(&iCount,&iSize);
    if(FAILED(hr)) {
        qWarning()<<"failed to get capabilities";
        return false;
    }
    for(int i=0;i<iCount;i++) {
        hr = pConfig->GetStreamCaps(i,&pmt,reinterpret_cast<BYTE*>(&scc));
        if(hr == S_OK) {
            pvi = (VIDEOINFOHEADER*)pmt->pbFormat;

            if((pmt->majortype == MEDIATYPE_Video) &&
                    (pmt->formattype == FORMAT_VideoInfo)) {
                if((actualFormat.pixelFormat() == QVideoFrame::Format_RGB24) ||
                        (actualFormat.pixelFormat() == QVideoFrame::Format_RGB32) ||
                        (actualFormat.pixelFormat() == QVideoFrame::Format_YUYV)) {
                    if((actualFormat.frameWidth() == pvi->bmiHeader.biWidth) &&
                            (actualFormat.frameHeight() == pvi->bmiHeader.biHeight)) {
                        if(actualFormat.pixelFormat() == QVideoFrame::Format_RGB24)
                            pmt->subtype = MEDIASUBTYPE_RGB24;
                        else if(actualFormat.pixelFormat() == QVideoFrame::Format_YUYV)
                            pmt->subtype = MEDIASUBTYPE_YUY2;
                        else if(actualFormat.pixelFormat() == QVideoFrame::Format_RGB32)
                            pmt->subtype = MEDIASUBTYPE_RGB24;
                        else if(actualFormat.pixelFormat() == QVideoFrame::Format_YUV420P)
                            pmt->subtype = MEDIASUBTYPE_I420;
                        else if(actualFormat.pixelFormat() == QVideoFrame::Format_RGB555)
                            pmt->subtype = MEDIASUBTYPE_RGB555;
                        else if(actualFormat.pixelFormat() == QVideoFrame::Format_UYVY)
                            pmt->subtype = MEDIASUBTYPE_UYVY;
                        else {
                            qWarning()<<"unknown format?";
                            return false;
                        }
                        hr = pConfig->SetFormat(pmt);
                        if(FAILED(hr)) {
                            qWarning()<<"failed to set format";
                            return false;
                        } else
                            break;
                    }
                }
            }
        }
    }
    // Set Sample Grabber config to match capture
    ZeroMemory(&am_media_type, sizeof(am_media_type));
    am_media_type.majortype = MEDIATYPE_Video;

    if(actualFormat.pixelFormat() == QVideoFrame::Format_RGB32)
        am_media_type.subtype = MEDIASUBTYPE_RGB24;
    else if(actualFormat.pixelFormat() == QVideoFrame::Format_RGB24)
        am_media_type.subtype = MEDIASUBTYPE_RGB24;
    else if(actualFormat.pixelFormat() == QVideoFrame::Format_YUYV)
        am_media_type.subtype = MEDIASUBTYPE_YUY2;
    else if(actualFormat.pixelFormat() == QVideoFrame::Format_YUV420P)
        am_media_type.subtype = MEDIASUBTYPE_I420;
    else if(actualFormat.pixelFormat() == QVideoFrame::Format_RGB555)
        am_media_type.subtype = MEDIASUBTYPE_RGB555;
    else if(actualFormat.pixelFormat() == QVideoFrame::Format_UYVY)
        am_media_type.subtype = MEDIASUBTYPE_UYVY;
    else {
        qWarning()<<"unknown format? for SG";
        return false;
    }

    am_media_type.formattype = FORMAT_VideoInfo;
    hr = pSG->SetMediaType(&am_media_type);
    if(FAILED(hr)) {
        qWarning()<<"failed to set video format on grabber";
        return false;
    }
    pSG->GetConnectedMediaType(&StillMediaType);

    CoUninitialize();

    return true;
}

bool DSCameraSession::openStream()
{
    //Opens the stream for reading and allocates any neccesary resources needed
    //Return true if success, false otherwise

    if(opened) return true;

    if(!graph)
        graph = createFilterGraph();

    if(!graph)
        return false;

    CoInitialize(NULL);

    HRESULT hr;

    hr = pGraph->AddFilter(pCap, L"Capture Filter");
    if(FAILED(hr)) {
        qWarning()<<"failed to create capture filter";
        return false;
    }
    hr = pGraph->AddFilter(pSG_Filter, L"Sample Grabber");
    if(FAILED(hr)) {
        qWarning()<<"failed to add sample grabber";
        return false;
    }
    hr = pBuild->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video,
                              pCap,NULL,pSG_Filter);
    if(FAILED(hr)) {
        qWarning()<<"failed to renderstream";
        return false;
    }
    pSG->GetConnectedMediaType(&StillMediaType);
    pSG_Filter->Release();

    CoUninitialize();

    return true;
}

void DSCameraSession::closeStream()
{
    // Closes the stream and internally frees any resources used
    opened = false;

    HRESULT hr;
    IPin *pPin = 0;
    hr = GetPin(pCap,PINDIR_OUTPUT,&pPin);
    if(FAILED(hr)) {
        qWarning()<<"failed to disconnect capture filter";
        return;
    }
    pGraph->Disconnect(pPin);
    if(FAILED(hr)) {
        qWarning()<<"failed to disconnect grabber filter";
        return;
    }
    hr = GetPin(pSG_Filter,PINDIR_INPUT,&pPin);
    pGraph->Disconnect(pPin);
    pGraph->RemoveFilter(pSG_Filter);
    pGraph->RemoveFilter(pCap);

    SAFE_RELEASE(pCap);
    SAFE_RELEASE(pSG_Filter);
    SAFE_RELEASE(pGraph);
    SAFE_RELEASE(pBuild);

    graph = false;
}

bool DSCameraSession::startStream()
{
    // Starts the stream, by emitting either QVideoPackets
    // or QvideoFrames, depending on Format chosen

    if(!setProperties()) {
        qWarning()<<"try to create!!!!";
        closeStream();
        if(openStream())
            return false;
    }

    if(!opened) {
        opened = openStream();
    }
    if(!opened) {
        qWarning()<<"failed to openStream()";
        return false;
    }

    HRESULT hr;

    hr = pGraph->QueryInterface(IID_IMediaControl,(void**)&pControl);
    if(FAILED(hr)) {
        qWarning()<<"failed to get stream control";
        return false;
    }

    hr = pControl->Run();
    if(FAILED(hr)) {
        qWarning()<<"failed to start";
        return false;
    } else {
        pControl->Release();
    }
    active = true;
    return true;
}

void DSCameraSession::stopStream()
{
    // Stops the stream from emitting packets

    HRESULT hr;

    hr = pGraph->QueryInterface(IID_IMediaControl,(void**)&pControl);
    if(FAILED(hr)) {
        qWarning()<<"failed to get stream control";
        return;
    }

    hr = pControl->Stop();
    if(FAILED(hr)) {
        qWarning()<<"failed to stop";
        return;
    } else {
        pControl->Release();
    }
    active = false;

    if(opened)
        closeStream();
}

void DSCameraSession::suspendStream()
{
    // Pauses the stream
    HRESULT hr;

    hr = pGraph->QueryInterface(IID_IMediaControl,(void**)&pControl);
    if(FAILED(hr)) {
        qWarning()<<"failed to get stream control";
        return;
    }

    hr = pControl->Pause();
    if(FAILED(hr)) {
        qWarning()<<"failed to pause";
        return;
    } else {
        pControl->Release();
    }
    active = false;
}

void DSCameraSession::resumeStream()
{
    // Resumes a paused stream
    startStream();
}




