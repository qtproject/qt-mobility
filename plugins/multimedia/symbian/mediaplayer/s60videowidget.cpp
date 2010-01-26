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

#include "s60videowidget.h"

//#include <QtCore/qcoreevent.h>
//#include <QtGui/qpainter.h>
//#include <QtGui/qevent.h>
#include <QtGui/private/qwidget_p.h>

class QBlackWidget : public QWidget
{
public:
    
    QBlackWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAutoFillBackground(false);
    }
    
protected:
    
    void paintEvent(QPaintEvent *)
    {
        if (!updatesEnabled())
            return;
        
        QPainter painter(this);
        painter.fillRect(rect(), Qt::black);
    }
};

S60VideoWidgetControl::S60VideoWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(0)
    , m_windowId(0)
    , m_aspectRatioMode(QVideoWidget::KeepAspectRatio)
{
    m_widget = new QBlackWidget();
    m_windowId = m_widget->winId();
    m_widget->installEventFilter(this);
}

S60VideoWidgetControl::~S60VideoWidgetControl()
{
    delete m_widget;
}

QWidget *S60VideoWidgetControl::videoWidget()
{
    return m_widget;
}

QVideoWidget::AspectRatioMode S60VideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60VideoWidgetControl::setAspectRatioMode(QVideoWidget::AspectRatioMode ratio)
{
    if (m_aspectRatioMode != ratio)
        m_aspectRatioMode = ratio;
}

bool S60VideoWidgetControl::isFullScreen() const
{
    return m_widget->isFullScreen();
}

void S60VideoWidgetControl::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(fullScreen);
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
    if (object == m_widget) {
        if (e->type() == QEvent::ParentChange || e->type() == QEvent::Show) {
            WId newWId = m_widget->winId();
            if (newWId != m_windowId) {
                m_windowId = newWId;
                emit widgetUpdated();
            }
        }

        if (e->type() == QEvent::Resize || e->type() == QEvent::Move)
            emit widgetUpdated();
    }    
	return QVideoWidgetControl::eventFilter(object, e);
}

void S60VideoWidgetControl::videoStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
        qt_widget_private(m_widget)->extraData()->disableBlit = false;
        m_widget->setUpdatesEnabled(true);
        m_widget->repaint();
    } else if (state == QMediaPlayer::PlayingState) {
        qt_widget_private(m_widget)->extraData()->disableBlit = true;
        m_widget->setUpdatesEnabled(false);
    }
}
