/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60videowidget.h"
#include "s60videowidgetdisplay.h"
#include <QtCore/QEvent>
#include <QtCore/QVariant>
#include <fbs.h>
#include <w32std.h>

S60VideoWidgetDisplay::S60VideoWidgetDisplay(QObject *parent)
    : S60VideoDisplay(parent)
    , m_widget(new S60VideoWidget)
{
    connect(this, SIGNAL(paintingEnabledChanged(bool)), m_widget, SLOT(setPaintingEnabled(bool)));
    connect(this, SIGNAL(fullScreenChanged(bool)), m_widget, SLOT(setFullScreen(bool)));
    connect(this, SIGNAL(contentRectChanged(const QRect&)), m_widget, SLOT(setContentRect(const QRect &)));
#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
    connect(m_widget, SIGNAL(beginVideoWidgetNativePaint()), this, SIGNAL(beginVideoWindowNativePaint()));
    connect(m_widget, SIGNAL(endVideoWidgetNativePaint()), this, SIGNAL(endVideoWindowNativePaint()));
#endif
    m_widget->installEventFilter(this);
    m_widget->setPaintingEnabled(false);
}

S60VideoWidgetDisplay::~S60VideoWidgetDisplay()
{
    // Notify observers that window is about to be destroyed
    QScopedPointer<QWidget> widget(m_widget);
    m_widget = 0;
    emit windowHandleChanged(windowHandle());
    // Widget will be deleted by QScopedPointer
}

bool S60VideoWidgetDisplay::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget) {
        switch (e->type()) {
        case QEvent::ParentChange:
            if (QWidget *parent = m_widget->parentWidget())
                parent->setProperty("_q_DummyWindowSurface", true);
            break;
        case QEvent::WinIdChange:
            m_widget->setWindowBackgroundColor();
            emit windowHandleChanged(windowHandle());
            break;
        case QEvent::Resize:
            emit displayRectChanged(extentRect(), clipRect());
            break;
#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
        case QEvent::Move:
            // TODO: this is insufficient - we also need to respond to changes in
            // the position of ancestor widgets
            emit displayRectChanged(extentRect(), clipRect());
            break;
#endif
        case QEvent::Show:
            emit windowHandleChanged(windowHandle());
            emit visibilityChanged(true);
            break;
        case QEvent::Hide:
            emit visibilityChanged(false);
            break;
        default:
            // Do nothing
            break;
        }
    }
    return false;
}

WId S60VideoWidgetDisplay::winId() const
{
    return m_widget ? m_widget->videoWinId() : 0;
}

QRect S60VideoWidgetDisplay::extentRect() const
{
    QRect rect;
    if (const RWindow *window = windowHandle()) {
        const TSize size = window ? window->Size() : TSize();
        if (m_explicitExtentRect.isValid())
            rect = m_explicitExtentRect;
        else
            rect = QRect(0, 0, size.iWidth, size.iHeight);
#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
        const TPoint pos = window ? window->AbsPosition() : TPoint();
        rect.moveTopLeft(QPoint(pos.iX, pos.iY));
#endif
    }
    return rect;
}

void S60VideoWidgetDisplay::setFrame(const CFbsBitmap &bitmap)
{
    m_pixmap = QPixmap::fromSymbianCFbsBitmap(const_cast<CFbsBitmap*>(&bitmap));
    m_widget->setPixmap(&m_pixmap);
}

QWidget *S60VideoWidgetDisplay::widget() const
{
    return m_widget;
}

void S60VideoWidgetDisplay::setTopWinId(WId id)
{
    m_widget->setTopWinId(id);
}

WId S60VideoWidgetDisplay::topWinId() const
{
    return m_widget->topWinId();
}

void S60VideoWidgetDisplay::setOrdinalPosition(int ordinalPosition)
{
    m_widget->setOrdinalPosition(ordinalPosition);
}

int S60VideoWidgetDisplay::ordinalPosition() const
{
    return m_widget->ordinalPosition();
}

const QRect &S60VideoWidgetDisplay::explicitExtentRect() const
{
    return m_explicitExtentRect;
}

void S60VideoWidgetDisplay::setExplicitExtentRect(const QRect &rect)
{
    if (rect != m_explicitExtentRect) {
        m_explicitExtentRect = rect;
        emit displayRectChanged(extentRect(), clipRect());
    }
}
