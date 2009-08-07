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

#include "qwmpvideooverlay.h"

#include "qwmpglobal.h"


QWmpVideoOverlay::QWmpVideoOverlay(QObject *parent)
    : QVideoOverlayEndpoint(parent)
    , m_object(0)
    , m_inPlaceObject(0)
    , m_clientSite(0)
{
}

QWmpVideoOverlay::~QWmpVideoOverlay()
{
}

void QWmpVideoOverlay::setEnabled(bool enabled)
{
    if (m_inPlaceObject && enabled && winId()) {
        QRect rect = displayRect();

        RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
        m_object->DoVerb(
                OLEIVERB_INPLACEACTIVATE, 0, m_clientSite, 0, winId(), &rcPos);

        QVideoOverlayEndpoint::setEnabled(true);
    } else {
        QVideoOverlayEndpoint::setEnabled(false);
    }
}

void QWmpVideoOverlay::setWinId(WId id)
{
    if (m_inPlaceObject && isEnabled()) {
        QRect rect = displayRect();

        RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
        m_object->DoVerb(
                OLEIVERB_INPLACEACTIVATE, 0, m_clientSite, 0, id, &rcPos);
    }

    QVideoOverlayEndpoint::setWinId(id);
}

extern HDC Q_GUI_EXPORT qt_win_display_dc();

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   ((HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   (((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH)

void QWmpVideoOverlay::setDisplayRect(const QRect &rect)
{
    if (m_inPlaceObject && isEnabled()) {
        HDC gdc = qt_win_display_dc();

        SIZEL hmSize = {
                MAP_PIX_TO_LOGHIM(rect.width(), GetDeviceCaps(gdc, LOGPIXELSX)),
                MAP_PIX_TO_LOGHIM(rect.height(), GetDeviceCaps(gdc, LOGPIXELSY)) };

        m_object->SetExtent(DVASPECT_CONTENT, &hmSize);

        RECT rcPos = { rect.left(), rect.top(), rect.right(), rect.bottom() };
        m_inPlaceObject->SetObjectRects(&rcPos, &rcPos);
    }

    QVideoOverlayEndpoint::setDisplayRect(rect);
}

void QWmpVideoOverlay::setFullscreen(bool fullscreen)
{
}

QSize QWmpVideoOverlay::nativeSize() const
{
    return m_sizeHint;
}

void QWmpVideoOverlay::setNativeSize(const QSize &size)
{    
    if (m_sizeHint != size) {
        m_sizeHint = size;

        emit nativeSizeChanged();
    }
}

void QWmpVideoOverlay::setObject(IOleObject *object, IOleClientSite *site)
{
    if (m_inPlaceObject) {
        m_inPlaceObject->Release();
        m_inPlaceObject = 0;
    }

    if (m_object)
        m_object->Release();

    if (m_clientSite)
        m_clientSite->Release();

    m_object = object;
    m_clientSite = site;

    if (m_object) {
        m_object->AddRef();

        HRESULT hr;

        if ((hr = m_object->QueryInterface(
                __uuidof(IOleInPlaceObject),
                reinterpret_cast<void **>(&m_inPlaceObject))) != S_OK) {
            qWarning("No IOleInnPlaceObject interface, %x: %s", hr, qwmp_error_string(hr));
        }
    }

    if (m_clientSite)
        m_clientSite->AddRef();
}
