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

#include <QtCore/qcoreevent.h>
#include <QtCore/qdebug.h>
#include <QtGui/qapplication.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>

#include <QtGui/private/qwidget_p.h>

#include <coemain.h>    // For CCoeEnv
#include <coecntrl.h>
#include <w32std.h>

#include <QTimer>

class S60VideoWidget : public QWidget
{
public:
    S60VideoWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QPalette palette;
        palette.setColor(QPalette::Background, Qt::black);
        setPalette(palette);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
    }

    virtual ~S60VideoWidget() {}

    QSize sizeHint() const
    {
        return m_nativeSize;
    }

    void setNativeSize(const QSize &size)
    {
        if (size != m_nativeSize) {
            m_nativeSize = size;
            if (size.isEmpty())
                setMinimumSize(0,0);
            else
                setMinimumSize(160,120);

            updateGeometry();
        }
    }
    
    void viewFinderFrameReady(const QImage& image)
    {
        m_pixmapImage = QPixmap::fromImage(image);
        repaint();
    }
    
protected:
    void paintEvent(QPaintEvent *)
    {
        if (!updatesEnabled())
            return;
        QPainter painter(this);
        painter.fillRect(rect(), palette().background());
    }

    QPixmap m_pixmapImage;
    QSize m_nativeSize;
};

S60VideoWidgetControl::S60VideoWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(new S60VideoWidget)
{
    m_widget->installEventFilter(this);
}

S60VideoWidgetControl::~S60VideoWidgetControl()
{
    delete m_widget;
}

;bool S60VideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
	if (e->type() == QEvent::Show) {
		m_widget->setAttribute(Qt::WA_NoSystemBackground, true);

		return true;
	}
	
	if (e->type() == QEvent::Resize) {
        emit resizeVideo();
        return true;
	}
	
	if (e->type() == QEvent::Paint) {
        QTimer::singleShot(20, this, SLOT(enableUpdates()));
	}
    
	return QVideoWidgetControl::eventFilter(object, e);
}

void S60VideoWidgetControl::enableUpdates()
{
    emit resizeVideo();
}

void S60VideoWidgetControl::setOverlay()
{
	// TODO:
}

void S60VideoWidgetControl::updateNativeVideoSize()
{
    // TODO:
}


void S60VideoWidgetControl::windowExposed()
{
    // TODO:
}

QWidget *S60VideoWidgetControl::videoWidget()
{
    return m_widget;
}

QVideoWidget::AspectRatioMode S60VideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

QSize S60VideoWidgetControl::customAspectRatio() const
{
    return m_customAspectRatio;
}

void S60VideoWidgetControl::setAspectRatioMode(QVideoWidget::AspectRatioMode ratio)
{
    // TODO:
}

void S60VideoWidgetControl::setCustomAspectRatio(const QSize &ratio)
{
    m_customAspectRatio = ratio;
}

bool S60VideoWidgetControl::isFullScreen() const
{
    return m_widget->isFullScreen();
}

void S60VideoWidgetControl::setFullScreen(bool fullScreen)
{
    if (fullScreen) {
        m_widget->setWindowFlags(m_widget->windowFlags() | Qt::Window | Qt::WindowStaysOnTopHint);
        m_widget->setWindowState(m_widget->windowState() | Qt::WindowFullScreen);

        m_widget->show();

        emit fullScreenChanged(fullScreen);
    } else {
        m_widget->setWindowFlags(m_widget->windowFlags() & ~(Qt::Window | Qt::WindowStaysOnTopHint));
        m_widget->setWindowState(m_widget->windowState() & ~Qt::WindowFullScreen);

        m_widget->show();

        emit fullScreenChanged(fullScreen);
    }
}

int S60VideoWidgetControl::brightness() const
{
    int brightness = 0;

    // TODO:

    return brightness / 10;
}

void S60VideoWidgetControl::setBrightness(int brightness)
{
    // TODO:

    emit brightnessChanged(brightness);
}

int S60VideoWidgetControl::contrast() const
{
    int contrast = 0;

    // TODO:

    return contrast / 10;
}

void S60VideoWidgetControl::setContrast(int contrast)
{
    // TODO:

    emit contrastChanged(contrast);
}

int S60VideoWidgetControl::hue() const
{
    int hue = 0;

    // TODO:

    return hue / 10;
}

void S60VideoWidgetControl::setHue(int hue)
{
    // TODO:

    emit hueChanged(hue);
}

int S60VideoWidgetControl::saturation() const
{
    int saturation = 0;

    // TODO:

    return saturation / 10;
}

void S60VideoWidgetControl::setSaturation(int saturation)
{
    // TODO:

    emit saturationChanged(saturation);
}
