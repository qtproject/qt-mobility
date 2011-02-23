/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "s60videowidget.h"
#ifdef USE_PRIVATE_QWIDGET_METHODS
#include <QtGui/private/qwidget_p.h>
#endif
#include <QEvent>
#include <coemain.h>    // For CCoeEnv
#include <coecntrl.h>   // For CCoeControl

QAbstractVideoWidget::QAbstractVideoWidget(QWidget *parent)
    : QWidget(parent)
{
}

QAbstractVideoWidget::~QAbstractVideoWidget()
{
}

QBlackSurface::QBlackSurface(QWidget *parent)
    : QAbstractVideoWidget(parent)
{
#ifdef USE_PRIVATE_QWIDGET_METHODS
#if QT_VERSION >= 0x040601 && !defined(__WINSCW__)
    qt_widget_private(this)->createExtra();
    qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::Disable;
#endif
#endif
    winId();
}

QBlackSurface::~QBlackSurface()
{
}

void QBlackSurface::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // Do nothing
}

QBlackWidget::QBlackWidget(QWidget *parent)
    : QAbstractVideoWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
    setPalette(QPalette(Qt::black));
#ifdef USE_PRIVATE_QWIDGET_METHODS
#if QT_VERSION >= 0x040601 && !defined(__WINSCW__)
    qt_widget_private(this)->extraData()->nativePaintMode = QWExtra::ZeroFill;
    qt_widget_private(this)->extraData()->receiveNativePaintEvents = true;
#endif
#endif
    winId();
}

QBlackWidget::~QBlackWidget()
{
}

void QBlackWidget::beginNativePaintEvent(const QRect& /*controlRect*/)
{
    emit beginVideoWindowNativePaint();
}

void QBlackWidget::endNativePaintEvent(const QRect& /*controlRect*/)
{
    CCoeEnv::Static()->WsSession().Flush();
    emit endVideoWindowNativePaint();
}

void QBlackWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // Do nothing
}

S60VideoWidgetControl::S60VideoWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(0)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
    , m_fullScreenEnabled(0)
{
    initializeVideoOutput();
}

void S60VideoWidgetControl::initializeVideoOutput()
{
#ifdef MMF_VIDEO_SURFACES_SUPPORTED
    m_widget.reset(new QBlackSurface());
#else
    m_widget.reset(new QBlackWidget());
    connect(m_widget.data(), SIGNAL(beginVideoWindowNativePaint()), this, SIGNAL(beginVideoWindowNativePaint()));
    connect(m_widget.data(), SIGNAL(endVideoWindowNativePaint()), this, SIGNAL(endVideoWindowNativePaint()));
#endif
    m_widget->installEventFilter(this);
}

S60VideoWidgetControl::~S60VideoWidgetControl()
{
    QScopedPointer<QAbstractVideoWidget> widget(m_widget.take());
    // Remove window handle from video player session before widget is deleted
    emit windowHandleChanged();
}

QWidget *S60VideoWidgetControl::videoWidget()
{
    return m_widget.data();
}

Qt::AspectRatioMode S60VideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60VideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    if (m_aspectRatioMode != ratio) {
        m_aspectRatioMode = ratio;
        emit aspectRatioChanged();
    }
}

bool S60VideoWidgetControl::isFullScreen() const
{
    return m_fullScreenEnabled;
}

void S60VideoWidgetControl::setFullScreen(bool fullScreen)
{
    if (m_fullScreenEnabled != fullScreen) {
        m_fullScreenEnabled = fullScreen;
        if (fullScreen && m_widget)
            m_widget->showFullScreen();
        else if (m_widget)
            m_widget->showNormal();
        emit fullScreenChanged(fullScreen);
    }
}

int S60VideoWidgetControl::brightness() const
{
    return 0;
}

void S60VideoWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int S60VideoWidgetControl::contrast() const
{
     return 0;
}

void S60VideoWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int S60VideoWidgetControl::hue() const
{
    return 0;
}

void S60VideoWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int S60VideoWidgetControl::saturation() const
{
    return 0;
}

void S60VideoWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

bool S60VideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget.data()) {
        if (e->type() == QEvent::ParentChange) {
            if (QWidget *parent = m_widget->parentWidget())
                parent->setProperty("_q_DummyWindowSurface", true);
            emit windowHandleChanged();
        } else if (e->type() == QEvent::WinIdChange
            || e->type() == QEvent::Show) {
            emit windowHandleChanged();
        } else if (e->type() == QEvent::Resize) {
            emit displayRectChanged();
        }
#ifndef MMF_VIDEO_SURFACES_SUPPORTED
        // TODO: this is insufficient - we also need to respond to changes in
        // the position of ancestor widgets
        else if (e->type() == QEvent::Move) {
            emit displayRectChanged();
        }
#endif
    }
    return false;
}

WId S60VideoWidgetControl::videoWinId() const
{
    WId wid = 0;
    if (m_widget) {
        if (m_widget->internalWinId())
            wid = m_widget->internalWinId();
        else if (m_widget->parentWidget() && m_widget->effectiveWinId())
            wid = m_widget->effectiveWinId();
    }
    return wid;
}

QRect S60VideoWidgetControl::videoDisplayRect() const
{
    const RWindow *window = videoWindowHandle();
    const TSize size = window ? window->Size() : TSize();
#ifdef MMF_VIDEO_SURFACES_SUPPORTED
    return QRect(0, 0, size.iWidth, size.iHeight);
#else
    const TPoint pos = window ? window->AbsPosition() : TPoint();
    return QRect(pos.iX, pos.iY, size.iWidth, size.iHeight);
#endif
}

Qt::AspectRatioMode S60VideoWidgetControl::videoAspectRatio() const
{
    return aspectRatioMode();
}

void S60VideoWidgetControl::videoStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
#ifdef USE_PRIVATE_QWIDGET_METHODS
#if QT_VERSION <= 0x040600 && !defined(FF_QT)
        qDebug()<<"S60VideoPlayerSession::videoStateChanged() - state == QMediaPlayer::StoppedState";
        qt_widget_private(m_widget.data())->extraData()->disableBlit = false;
#endif
#endif
        m_widget->repaint();
    } else if (state == QMediaPlayer::PlayingState) {
#ifdef USE_PRIVATE_QWIDGET_METHODS
#if QT_VERSION <= 0x040600 && !defined(FF_QT)
        qDebug()<<"S60VideoPlayerSession::videoStateChanged() - state == QMediaPlayer::PlayingState";
        qt_widget_private(m_widget.data())->extraData()->disableBlit = true;
#endif
#endif
    }
}
