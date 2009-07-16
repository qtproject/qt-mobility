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

#ifndef QWMPPLAYERSERVICE_H
#define QWMPPLAYERSERVICE_H

#include "qmediaplayerservice.h"

#include "qwmpevents.h"

#include <wmp.h>

class QMediaMetaData;
class QMediaPlayerControl;
class QMediaPlaylist;

class QWmpMetaData;
class QWmpPlaceholderWidget;
class QWmpPlayerControl;
class QWmpPlaylist;

class QWmpPlayerService
    : public QMediaPlayerService
    , public IOleClientSite
    , public IOleInPlaceSite
    , public IOleInPlaceFrame
    , public IServiceProvider
    , public IWMPRemoteMediaServices
{
    Q_OBJECT
public:
    enum EmbedMode
    {
        LocalEmbed,
        RemoteEmbed
    };

    QWmpPlayerService(EmbedMode mode, QObject *parent = 0);
    ~QWmpPlayerService();

    QAbstractMediaControl *control(const char *name) const;

    void setVideoOutput(QObject *output);

    QList<QByteArray> supportedEndpointInterfaces(
            QMediaEndpointInterface::Direction direction) const;

    QObject *createEndpoint(const char *iid);

    bool eventFilter(QObject *object, QEvent *event);

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **object);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IOleClientSite
    HRESULT STDMETHODCALLTYPE SaveObject();
    HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
    HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer);
    HRESULT STDMETHODCALLTYPE ShowObject();
    HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
    HRESULT STDMETHODCALLTYPE RequestNewObjectLayout();

    // IOleWindow
    HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);

    // IOleInPlaceSite
    HRESULT STDMETHODCALLTYPE CanInPlaceActivate();
    HRESULT STDMETHODCALLTYPE OnInPlaceActivate();
    HRESULT STDMETHODCALLTYPE OnUIActivate();
    HRESULT STDMETHODCALLTYPE GetWindowContext(
            IOleInPlaceFrame **ppFrame,
            IOleInPlaceUIWindow **ppDoc,
            LPRECT lprcPosRect,
            LPRECT lprcClipRect,
            LPOLEINPLACEFRAMEINFO lpFrameInfo);
    HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
    HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate();
    HRESULT STDMETHODCALLTYPE DiscardUndoState();
    HRESULT STDMETHODCALLTYPE DeactivateAndUndo();
    HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);

    // IOleInPlaceUIWindow
    HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder);
    HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths);
    HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths);
    HRESULT STDMETHODCALLTYPE SetActiveObject(
            IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);

    // IOleInPlaceFrame
    HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
    HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
    HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared);
    HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText);
    HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID);

    // IServiceProvider
    HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, REFIID riid, void **ppvObject);

    // IWMPRemoteMediaServices
    HRESULT STDMETHODCALLTYPE GetServiceType(BSTR *pbstrType);
    HRESULT STDMETHODCALLTYPE GetApplicationName(BSTR *pbstrName);
    HRESULT STDMETHODCALLTYPE GetScriptableObject(BSTR *pbstrName, IDispatch **ppDispatch);
    HRESULT STDMETHODCALLTYPE GetCustomUIMode(BSTR *pbstrFile);

private:
    volatile LONG m_ref;
    EmbedMode m_embedMode;
    IWMPPlayer4 *m_player;
    IOleObject *m_oleObject;
    IOleInPlaceObject *m_inPlaceObject;
    QWmpEvents *m_events;
    QWmpPlaceholderWidget *m_placeholderWidget;
    QWmpPlayerControl *m_control;
    QWmpMetaData *m_metaData;

#ifdef QWMP_EVR
    HINSTANCE m_evrHwnd;
#endif
};

#endif
