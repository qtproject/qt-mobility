/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qcoreevent.h>
#include <QtCore/qtimer.h>

#include "s60videowidgetcontrol.h"

#include <coemain.h>    // CCoeEnv
#include <coecntrl.h>   // CCoeControl
#include <w32std.h>

S60VideoWidgetControl::S60VideoWidgetControl(QObject *parent) :
    QVideoWidgetControl(parent), 
    m_widget(new QLabel()),
    m_fullScreen(false)
{
    m_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_widget->setAlignment(Qt::AlignCenter);
    m_widget->setAttribute(Qt::WA_NoSystemBackground, true);
    
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    m_widget->setPalette(palette);

    // Request QEvents
    m_widget->installEventFilter(this);
    m_windowId = m_widget->effectiveWinId();
}

S60VideoWidgetControl::~S60VideoWidgetControl()
{
    delete m_widget;
}

bool S60VideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget) {
        
        switch (e->type()) {
            case QEvent::ParentChange:
            case QEvent::WinIdChange:
            case QEvent::Show:
                m_windowId = m_widget->effectiveWinId();
                emit widgetUpdated();
                break;
            case QEvent::Resize:
                emit widgetResized(m_widget->size());
                break;
            case QEvent::PolishRequest:
                m_widget->ensurePolished();
                break;
            
            default:
                // Do nothing
                break;
        }
    }
    return false;
}

QWidget *S60VideoWidgetControl::videoWidget()
{
    return m_widget;
}

Qt::AspectRatioMode S60VideoWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60VideoWidgetControl::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    if (m_aspectRatioMode==ratio) {
        return;
    }
    m_aspectRatioMode = ratio;

    if (m_aspectRatioMode == Qt::KeepAspectRatio)
        m_widget->setScaledContents(false);
    else {
        m_widget->setScaledContents(true);
    }
}

bool S60VideoWidgetControl::isFullScreen() const
{
    return m_fullScreen;
}

void S60VideoWidgetControl::setFullScreen(bool fullScreen)
{
    if (m_widget && !fullScreen && m_fullScreen) {
        m_widget->showNormal();
        m_fullScreen = false;
    } else if (m_widget && fullScreen) {
        m_widget->showFullScreen();
        m_fullScreen = true;
    }

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

// End of file
