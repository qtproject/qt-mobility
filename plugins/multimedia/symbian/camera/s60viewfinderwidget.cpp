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

#include "s60viewfinderwidget.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qdebug.h>
#include <QtGui/private/qwidget_p.h>

#include <coemain.h>    // For CCoeEnv
#include <coecntrl.h>
#include <w32std.h>

#include <QTimer>

S60ViewFinderWidgetControl::S60ViewFinderWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(new QLabel())
{
    m_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_widget->setAlignment(Qt::AlignCenter);
    m_widget->setAttribute(Qt::WA_NoSystemBackground, true);
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    m_widget->setPalette(palette);    
    m_widget->installEventFilter(this);
    m_windowId = m_widget->effectiveWinId();
}

S60ViewFinderWidgetControl::~S60ViewFinderWidgetControl()
{
    delete m_widget;
}

bool S60ViewFinderWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget) {
        if ((e->type() == QEvent::ParentChange || e->type() == QEvent::Show) &&
            m_widget->effectiveWinId() != m_windowId) {
                m_windowId = m_widget->effectiveWinId();
                emit widgetResized(m_widget->size());
            }

        if (e->type() == QEvent::Resize || e->type() == QEvent::Move)
            emit widgetResized(m_widget->size());
    }    
    return false;   
}

QWidget *S60ViewFinderWidgetControl::videoWidget()
{
    return m_widget;
}

QVideoWidget::AspectRatioMode S60ViewFinderWidgetControl::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60ViewFinderWidgetControl::setAspectRatioMode(QVideoWidget::AspectRatioMode ratio)
{
    if (m_aspectRatioMode==ratio)
        return;
    
    m_aspectRatioMode=ratio;
    if (m_aspectRatioMode == QVideoWidget::KeepAspectRatio)
        m_widget->setScaledContents(false);
    else
        m_widget->setScaledContents(true);
}

bool S60ViewFinderWidgetControl::isFullScreen() const
{
    return m_widget->isFullScreen();
}

void S60ViewFinderWidgetControl::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(fullScreen);
}

int S60ViewFinderWidgetControl::brightness() const
{
    return 0;
}

void S60ViewFinderWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int S60ViewFinderWidgetControl::contrast() const
{
    return 0;
}

void S60ViewFinderWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int S60ViewFinderWidgetControl::hue() const
{
    return 0;
}

void S60ViewFinderWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int S60ViewFinderWidgetControl::saturation() const
{
    return 0;
}

void S60ViewFinderWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}
