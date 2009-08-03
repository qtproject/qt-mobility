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

#include "qevrvideooverlay.h"


QEvrVideoOverlay::QEvrVideoOverlay(QObject *parent)
    : QVideoOverlayEndpoint(parent)
    , m_displayControl(0)
{
}

QEvrVideoOverlay::~QEvrVideoOverlay()
{
}

void QEvrVideoOverlay::setEnabled(bool enabled)
{
    if (m_displayControl && enabled && winId()) {
        m_displayControl->SetVideoWindow(winId());

        QRect rect = displayRect();

        RECT displayRect = { rect.left(), rect.top(), rect.right(), rect.bottom() };

        m_displayControl->SetVideoPosition(0, &displayRect);
    }
}

void QEvrVideoOverlay::setWinId(WId id)
{
    if (m_displayControl && isEnabled()) {
        m_displayControl->SetVideoWindow(id);

        QRect rect = displayRect();

        RECT displayRect = { rect.left(), rect.top(), rect.right(), rect.bottom() };

        m_displayControl->SetVideoPosition(0, &displayRect);
    }

    QVideoOverlayEndpoint::setWinId(id);
}

void QEvrVideoOverlay::setDisplayRect(const QRect &rect)
{
    if (m_displayControl && isEnabled()) {
        RECT displayRect = { rect.left(), rect.top(), rect.right(), rect.bottom() };

        m_displayControl->SetVideoPosition(0, &displayRect);
    }

    QVideoOverlayEndpoint::setDisplayRect(rect);
}

void QEvrVideoOverlay::setFullscreen(bool fullscreen)
{
    if (m_displayControl && m_displayControl->SetFullScreen(fullscreen) == S_OK)
        QVideoOverlayEndpoint::setFullscreen(fullscreen);
}

QSize QEvrVideoOverlay::nativeSize() const
{
    SIZE size;

    if (m_displayControl && m_displayControl->GetNativeVideoSize(&size, 0) == S_OK) {
        return QSize(size.cx, size.cy);
    } else {
        return QSize();
    }
}

void QEvrVideoOverlay::setDisplayControl(IMFVideoDisplayControl *control)
{
    if (m_displayControl)
        m_displayControl->Release();

    m_displayControl = control;

    if (m_displayControl)
        m_displayControl->AddRef();
}
