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
#include "qwmpplaceholderwidget.h"
#include "qwmpplayercontrol.h"
#include "qwmpplaylist.h"

#include "qmediaplayer.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/quuid.h>
#include <QtCore/qvariant.h>
#include <QtGui/qevent.h>

#include <wmprealestate.h>

QWmpPlayerService::QWmpPlayerService(EmbedMode mode, QObject *parent)
    : QMediaPlayerService(parent)
    , m_ref(1)
    , m_embedMode(mode)
    , m_player(0)
    , m_oleObject(0)
    , m_inPlaceObject(0)
    , m_events(0)
    , m_placeholderWidget(0)
    , m_control(0)
    , m_metaData(0)
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
        if ((hr = m_player->QueryInterface(
                __uuidof(IOleObject), reinterpret_cast<void **>(&m_oleObject))) != S_OK) {
            qWarning("No IOleObject interface, %x: %s", hr, qwmp_error_string(hr));
        } else if ((hr = m_oleObject->SetClientSite(this)) != S_OK) {
            qWarning("Failed to set site, %x: %s", hr, qwmp_error_string(hr));
        } else if ((hr = m_player->QueryInterface(
                __uuidof(IOleInPlaceObject),
                reinterpret_cast<void **>(&m_inPlaceObject))) != S_OK) {
            qWarning("No IOleInnPlaceObject interface, %x: %s", hr, qwmp_error_string(hr));
        }

        m_events = new QWmpEvents(m_player);
        m_metaData = new QWmpMetaData(m_events);
        m_control = new QWmpPlayerControl(m_player, m_events);

    }
}

QWmpPlayerService::~QWmpPlayerService()
{
    if (m_placeholderWidget)
        m_placeholderWidget->removeEventFilter(this);

    delete m_control;
    delete m_metaData;
    delete m_events;

    if (m_inPlaceObject)
        m_inPlaceObject->Release();

    if (m_oleObject) {
        m_oleObject->SetClientSite(0);
        m_oleObject->Release();
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

    if (m_placeholderWidget)
        m_placeholderWidget->removeEventFilter(this);

#ifdef QWMP_EVR
    IWMPVideoRenderConfig *config = 0;

    if (m_player && m_player->QueryInterface(
            __uuidof(IWMPVideoRenderConfig), reinterpret_cast<void **>(&config)) == S_OK) {
        IMFActivate *activate = 0;

        if (output)
            activate = qvariant_cast<IMFActivate *>(output->property("activate"));

        config->put_presenterActivate(activate);
        config->Release();
    }
#endif
    m_placeholderWidget = qobject_cast<QWmpPlaceholderWidget *>(output);

    if (m_placeholderWidget) {
        m_placeholderWidget->installEventFilter(this);
        
        BSTR mode = ::SysAllocString(L"none");
        m_player->put_uiMode(mode);
        ::SysFreeString(mode);
    }
}

QList<QByteArray> QWmpPlayerService::supportedEndpointInterfaces(
        QMediaEndpointInterface::Direction direction) const
{
    QList<QByteArray> interfaces;
#ifdef QWMP_EVR
    if (direction == QMediaEndpointInterface::Output && m_evrHwnd)
        interfaces << QMediaWidgetEndpoint_iid;
#endif
    if (direction == QMediaEndpointInterface::Output)
        interfaces << QMediaWidgetEndpoint_iid;

    return interfaces;
}

QObject *QWmpPlayerService::createEndpoint(const char *iid)
{
#ifdef QWMP_EVR
    if (strcmp(iid, QMediaWidgetEndpoint_iid) == 0 && m_evrHwnd)
        return new QEvrWidget;
#endif
    if (strcmp(iid, QMediaWidgetEndpoint_iid) == 0)
        return new QWmpPlaceholderWidget;
    return 0;
}

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   ((HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   (((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH)

bool QWmpPlayerService::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_placeholderWidget) {
        switch (event->type()) {
        case QEvent::Show:
            {
                QRect rect = m_placeholderWidget->geometry();
                rect.moveTo(m_placeholderWidget->mapTo(
                        m_placeholderWidget->nativeParentWidget(), rect.topLeft()));

                RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
                m_oleObject->DoVerb(
                        OLEIVERB_INPLACEACTIVATE,
                        0,
                        static_cast<IOleClientSite *>(this),
                        0,
                        m_placeholderWidget->effectiveWinId(),
                        &rcPos);
            }
            break;
        case QEvent::Hide:
        case QEvent::Resize:
        case QEvent::Move:
            {
                QRect rect = m_placeholderWidget->geometry();
                rect.moveTo(m_placeholderWidget->mapTo(
                        m_placeholderWidget->nativeParentWidget(), rect.topLeft()));

                SIZEL hmSize;
                hmSize.cx = MAP_PIX_TO_LOGHIM(rect.width(), m_placeholderWidget->logicalDpiX());
                hmSize.cy = MAP_PIX_TO_LOGHIM(rect.height(), m_placeholderWidget->logicalDpiY());

                if (m_oleObject)
                    m_oleObject->SetExtent(DVASPECT_CONTENT, &hmSize);
                if (m_inPlaceObject) {
                    RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
                    m_inPlaceObject->SetObjectRects(&rcPos, &rcPos);
                }
            }
            break;
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
            {
                IOleInPlaceActiveObject *activateObject = 0;

                if (m_oleObject && m_player->QueryInterface(
                        __uuidof(IOleInPlaceActiveObject),
                        reinterpret_cast<void **>(&activateObject)) == S_OK) {
                    activateObject->OnFrameWindowActivate(event->type() == QEvent::WindowActivate);
                    activateObject->Release();
                }
            }
            break;
        default:
            break;
        }
    }

    return false;
}

// IUnknown
HRESULT QWmpPlayerService::QueryInterface(REFIID riid, void **object)
{
    if (!object) {
        return E_POINTER;
    } else if (riid == __uuidof(IUnknown)
            || riid == __uuidof(IOleClientSite)) {
        *object = static_cast<IOleClientSite *>(this);
    } else if (riid == __uuidof(IOleWindow)
            || riid == __uuidof(IOleInPlaceSite)) {
        *object = static_cast<IOleInPlaceSite *>(this);
    } else if (riid == __uuidof(IOleInPlaceUIWindow)
            || riid == __uuidof(IOleInPlaceFrame)) {
        *object = static_cast<IOleInPlaceFrame *>(this);
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
    return S_OK;
}

HRESULT QWmpPlayerService::OnShowWindow(BOOL fShow)
{
    Q_UNUSED(fShow);

    return S_OK;
}

HRESULT QWmpPlayerService::RequestNewObjectLayout()
{
    return E_NOTIMPL;
}

// IOleWindow
HRESULT QWmpPlayerService::GetWindow(HWND *phwnd)
{
    if (!phwnd) {
        return E_POINTER;
    } else if (!m_placeholderWidget) {
        *phwnd = 0;
        return E_UNEXPECTED;
    } else {
        *phwnd = m_placeholderWidget->effectiveWinId();
        return *phwnd ? S_OK : E_UNEXPECTED;
    }
}

HRESULT QWmpPlayerService::ContextSensitiveHelp(BOOL fEnterMode)
{
    Q_UNUSED(fEnterMode);

    return E_NOTIMPL;
}

// IOleInPlaceSite
HRESULT QWmpPlayerService::CanInPlaceActivate()
{
    return S_OK;
}

HRESULT QWmpPlayerService::OnInPlaceActivate()
{
    return S_OK;
}

HRESULT QWmpPlayerService::OnUIActivate()
{
    return S_OK;
}

HRESULT QWmpPlayerService::GetWindowContext(
        IOleInPlaceFrame **ppFrame,
        IOleInPlaceUIWindow **ppDoc,
        LPRECT lprcPosRect,
        LPRECT lprcClipRect,
        LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    if (!ppFrame || !ppDoc || !lprcPosRect || !lprcClipRect || !lpFrameInfo)
        return E_POINTER;

    QueryInterface(IID_IOleInPlaceFrame, reinterpret_cast<void **>(ppFrame));
    QueryInterface(IID_IOleInPlaceUIWindow, reinterpret_cast<void **>(ppDoc));
    
    HWND winId = m_placeholderWidget ? m_placeholderWidget->effectiveWinId() : 0;

    if (winId) {
        QRect rect = m_placeholderWidget->rect();
        rect.moveTo(m_placeholderWidget->mapTo(
                m_placeholderWidget->nativeParentWidget(), rect.topLeft()));

        SetRect(lprcPosRect, rect.left(), rect.top(), rect.right(), rect.bottom());
        SetRect(lprcClipRect, rect.left(), rect.top(), rect.right(), rect.bottom());
    } else {
        SetRectEmpty(lprcPosRect);
        SetRectEmpty(lprcClipRect);
    }

    lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp = false;
    lpFrameInfo->haccel = 0;
    lpFrameInfo->cAccelEntries = 0;
    lpFrameInfo->hwndFrame = winId;

    return S_OK;
}

HRESULT QWmpPlayerService::Scroll(SIZE scrollExtant)
{
    Q_UNUSED(scrollExtant);

    return S_FALSE;
}

HRESULT QWmpPlayerService::OnUIDeactivate(BOOL fUndoable)
{
    Q_UNUSED(fUndoable);

    return S_OK;
}

HRESULT QWmpPlayerService::OnInPlaceDeactivate()
{
    return S_OK;
}

HRESULT QWmpPlayerService::DiscardUndoState()
{
    return S_OK;
}

HRESULT QWmpPlayerService::DeactivateAndUndo()
{
    IOleInPlaceObject *object = 0;
    if (m_player && m_player->QueryInterface(
            __uuidof(IOleInPlaceObject), reinterpret_cast<void **>(&object))) {
        object->UIDeactivate();
        object->Release();
    }

    return S_OK;
}

HRESULT QWmpPlayerService::OnPosRectChange(LPCRECT lprcPosRect)
{
    Q_UNUSED(lprcPosRect);

    return S_OK;
}

// IOleInPlaceUIWindow
HRESULT QWmpPlayerService::GetBorder(LPRECT lprectBorder)
{
    Q_UNUSED(lprectBorder);

    return INPLACE_E_NOTOOLSPACE;
}

HRESULT QWmpPlayerService::RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
    Q_UNUSED(pborderwidths);

    return INPLACE_E_NOTOOLSPACE;
}

HRESULT QWmpPlayerService::SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
{
    Q_UNUSED(pborderwidths);

    return OLE_E_INVALIDRECT;
}

HRESULT QWmpPlayerService::SetActiveObject(
        IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
    Q_UNUSED(pActiveObject);
    Q_UNUSED(pszObjName);

    return  S_OK;
}

// IOleInPlaceFrame
HRESULT QWmpPlayerService::InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
    Q_UNUSED(hmenuShared);
    Q_UNUSED(lpMenuWidths);

    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
    Q_UNUSED(hmenuShared);
    Q_UNUSED(holemenu);
    Q_UNUSED(hwndActiveObject);

    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::RemoveMenus(HMENU hmenuShared)
{
    Q_UNUSED(hmenuShared);

    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::SetStatusText(LPCOLESTR pszStatusText)
{
    if (m_placeholderWidget) {
        QStatusTipEvent event(QString::fromWCharArray(pszStatusText));
        QCoreApplication::sendEvent(m_placeholderWidget, &event);
    }
    return S_OK;
}

HRESULT QWmpPlayerService::EnableModeless(BOOL fEnable)
{
    Q_UNUSED(fEnable);

    return E_NOTIMPL;
}

HRESULT QWmpPlayerService::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
    return TranslateAccelerator(lpmsg, static_cast<DWORD>(wID));
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
