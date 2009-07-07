/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwmpplayerservice.h"

#include "qevrwidget.h"
#include "qwmpglobal.h"
#include "qwmpmetadata.h"
#include "qwmpplayercontrol.h"
#include "qwmpplaylist.h"

#include "qmediaplayer.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qvariant.h>

#include <wmprealestate.h>

QWmpPlayerService::QWmpPlayerService(EmbedMode mode, QObject *parent)
    : QMediaPlayerService(parent)
    , m_ref(1)
    , m_embedMode(mode)
    , m_player(0)
    , m_videoOutput(0)
    , m_control(0)
    , m_metaData(0)
    , m_connectionPoint(0)
    , m_adviseCookie(0)
#ifdef QWMP_EVR
    , m_evrHwnd(0)
#endif
{
#ifdef QWMP_EVR
    qRegisterMetaType<IMFActivate *>();
#endif
    HRESULT hr;

    if ((hr = CoCreateInstance(
            __uuidof(WindowsMediaPlayer),
            0,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWMPPlayer4),
            reinterpret_cast<void **>(&m_player))) != S_OK) {
        qWarning("failed to create media player control, %x: %s", hr, qwmp_error_string(hr));
    } else {
        IOleObject *oleObject = 0;

        if ((hr = m_player->QueryInterface(
                __uuidof(IOleObject), reinterpret_cast<void **>(&oleObject))) != S_OK) {
            qWarning("No IOleObject interface, %x: %s", hr, qwmp_error_string(hr));
        } else {
            if ((hr = oleObject->SetClientSite(this)) != S_OK) {
                qWarning("Failed to set site, %x: %s", hr, qwmp_error_string(hr));
            }

            oleObject->Release();
        }

        IConnectionPointContainer *container = 0;

        if ((hr = m_player->QueryInterface(
                IID_IConnectionPointContainer, reinterpret_cast<void **>(&container))) != S_OK) {
            qWarning("No connection point container, %x: %d", hr, qwmp_error_string(hr));
        } else {
            if ((hr = container->FindConnectionPoint(
                    __uuidof(IWMPEvents), &m_connectionPoint)) != S_OK) {
                qWarning("No connection point for IWMPEvents %d", hr);
            } else if ((hr = m_connectionPoint->Advise(
                    static_cast<IWMPEvents3 *>(this), &m_adviseCookie)) != S_OK) {
                qWarning("Failed to link to connection point, %x, %s", hr, qwmp_error_string(hr));

                m_connectionPoint->Release();
                m_connectionPoint = 0;
            }
            container->Release();
        }

        m_control = new QWmpPlayerControl(m_player, this);
        m_metaData = new QWmpMetaData(this);
    }
}

QWmpPlayerService::~QWmpPlayerService()
{
    if (m_connectionPoint) {
        m_connectionPoint->Unadvise(m_adviseCookie);
        m_connectionPoint->Release();
    }

    delete m_control;

    IOleObject *oleObject = 0;

    if (m_player->QueryInterface(
            __uuidof(IOleObject), reinterpret_cast<void **>(&oleObject)) == S_OK) {
        oleObject->SetClientSite(0);
        oleObject->Release();
    }

    if (m_player)
        m_player->Release();

    FreeLibrary(m_evrHwnd);

    Q_ASSERT(m_ref == 1);
}

QAbstractMediaControl *QWmpPlayerService::control(const char *name) const
{
    if (qstrcmp(name, "com.nokia.qt.MediaPlayerControl") == 0)
        return m_control;
    else if (qstrcmp(name, "com.nokia.qt.MetadataControl") == 0)
        return m_metaData;
    else
        return 0;
}

void QWmpPlayerService::setVideoOutput(QObject *output)
{
    QAbstractMediaService::setVideoOutput(output);
#ifdef QWMP_EVR
    IWMPVideoRenderConfig *config = 0;

    if (m_player && m_player->QueryInterface(
            __uuidof(IWMPVideoRenderConfig), reinterpret_cast<void **>(&config)) == S_OK) {
        IMFActivate *activate = 0;

        if (m_videoOutput)
            activate = qvariant_cast<IMFActivate *>(m_videoOutput->property("activate"));

        config->put_presenterActivate(activate);
        config->Release();
    }
#endif
}

QList<QByteArray> QWmpPlayerService::supportedEndpointInterfaces(
        QMediaEndpointInterface::Direction direction) const
{
    QList<QByteArray> interfaces;
#ifndef QWMP_EVR
    Q_UNUSED(direction);
#else
    if (direction == QMediaEndpointInterface::Output && m_evrHwnd)
        interfaces << QMediaWidgetEndpoint_iid;
#endif

    return interfaces;
}

QObject *QWmpPlayerService::createEndpoint(const char *iid)
{
#ifndef QWMP_EVR
    Q_UNUSED(iid);
#else
    if (strcmp(iid, QMediaWidgetEndpoint_iid) == 0 && m_evrHwnd)
        return new QEvrWidget;
#endif
    return 0;
}

// IUnknown
HRESULT QWmpPlayerService::QueryInterface(REFIID riid, void **object)
{
    if (!object) {
        return E_POINTER;
    } else if (riid == __uuidof(IUnknown)
            || riid == __uuidof(IWMPEvents)
            || riid == __uuidof(IWMPEvents2)
            || riid == __uuidof(IWMPEvents3)) {
        *object = static_cast<IWMPEvents3 *>(this);
    } else if (riid == __uuidof(IOleClientSite)) {
        *object = static_cast<IOleClientSite *>(this);
    } else if (riid == __uuidof(IServiceProvider)) {
        *object = static_cast<IServiceProvider *>(this);
    } else if (riid == __uuidof(IWMPRemoteMediaServices)) {
        *object = static_cast<IWMPRemoteMediaServices *>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();

    return S_OK;
}

ULONG QWmpPlayerService::AddRef()
{
    return InterlockedIncrement(&m_ref);
}

ULONG QWmpPlayerService::Release()
{
    ULONG ref = InterlockedDecrement(&m_ref);

    Q_ASSERT(ref != 0);

    return ref;
}

// IWMPEvents
void QWmpPlayerService::PlayStateChange(long NewState)
{
    switch (NewState) {
    case wmppsStopped:
        m_control->setState(QMediaPlayer::StoppedState);
        break;
    case wmppsPaused:
        m_control->setState(QMediaPlayer::PausedState);
        break;
    case wmppsPlaying:
        m_control->setState(QMediaPlayer::PlayingState);
        break;
    default:
        break;
    }
}

void QWmpPlayerService::Buffering(VARIANT_BOOL Start)
{
    m_control->setBuffering(Start);
}

void QWmpPlayerService::PositionChange(double oldPosition, double newPosition)
{
    Q_UNUSED(oldPosition);

    m_control->positionChanged(newPosition);
}

void QWmpPlayerService::MediaChange(IDispatch *Item)
{
    Q_UNUSED(Item);

    IWMPMedia *media = 0;
    if (m_player->get_currentMedia(&media) == S_OK) {
        double duration = 0;

        media->get_duration(&duration);

        m_control->setDuration(duration);

        m_metaData->setMedia(media);

        media->Release();
    }
}

// IOleClientSite
HRESULT QWmpPlayerService::SaveObject()
{
    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
    Q_UNUSED(dwAssign);
    Q_UNUSED(dwWhichMoniker);
    Q_UNUSED(ppmk);

    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::GetContainer(IOleContainer **ppContainer)
{
    if (!ppContainer) {
        return E_POINTER;
    } else {
        *ppContainer = 0;

        return E_NOINTERFACE;
    }
}

HRESULT QWmpPlayerService::ShowObject()
{
    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::OnShowWindow(BOOL fShow)
{
    Q_UNUSED(fShow);

    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::RequestNewObjectLayout()
{
    return E_NOTIMPL;
}

// IServiceProvider
HRESULT QWmpPlayerService::QueryService(REFGUID guidService, REFIID riid, void **ppvObject)
{
    Q_UNUSED(guidService);

    if (!ppvObject) {
        return E_POINTER;
    } else if (riid == __uuidof(IWMPRemoteMediaServices)) {
        *ppvObject = static_cast<IWMPRemoteMediaServices *>(this);

        AddRef();

        return S_OK;
    } else {
        return E_NOINTERFACE;
    }
}

// IWMPRemoteMediaServices
HRESULT QWmpPlayerService::GetServiceType(BSTR *pbstrType)
{
    if (!pbstrType) {
        return E_POINTER;
    } else if (m_embedMode == RemoteEmbed) {
        *pbstrType = SysAllocString(L"Remote");

        return S_OK;
    } else {
        *pbstrType = SysAllocString(L"Local");

        return S_OK;
    }
}

HRESULT QWmpPlayerService::GetApplicationName(BSTR *pbstrName)
{
    if (!pbstrName) {
        return E_POINTER;
    } else {
        *pbstrName = SysAllocString(static_cast<const wchar_t *>(
                QCoreApplication::applicationName().utf16()));

        return S_OK;
    }
}

HRESULT QWmpPlayerService::GetScriptableObject(BSTR *pbstrName, IDispatch **ppDispatch)
{
    Q_UNUSED(pbstrName);
    Q_UNUSED(ppDispatch);

    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::GetCustomUIMode(BSTR *pbstrFile)
{
    Q_UNUSED(pbstrFile);

    return E_NOTIMPL;
}
