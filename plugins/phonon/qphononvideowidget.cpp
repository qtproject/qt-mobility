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

#include "qphononvideowidget.h"

#include <QEvent>
#include <QApplication>
#include <QGridLayout>

QPhononVideoWidget::QPhononVideoWidget(Phonon::VideoWidget *videoWidget, QObject *parent)
    :QVideoWidgetControl(parent), m_videoWidget(videoWidget), m_isFullscreen(false)
{
    setAspectRatioMode(QVideoWidget::KeepAspectRatio);
}

QPhononVideoWidget::~QPhononVideoWidget()
{
}

QVideoWidget::AspectRatioMode QPhononVideoWidget::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QPhononVideoWidget::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;
    switch (mode) {
        case QVideoWidget::KeepAspectRatio:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
            break;
        case QVideoWidget::IgnoreAspectRatio:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
            break;
        default:
            m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
    }
}

bool QPhononVideoWidget::isFullScreen() const
{
    return m_isFullscreen;
}

void QPhononVideoWidget::setFullScreen(bool fullScreen)
{
    if (m_isFullscreen != fullScreen) {
        m_isFullscreen = fullScreen;
        emit fullScreenChanged(fullScreen);
    }
}

int QPhononVideoWidget::brightness() const
{
    return qRound(100.0*m_videoWidget->brightness());
}

void QPhononVideoWidget::setBrightness(int brightness)
{
    m_videoWidget->setBrightness(brightness/100.0);
    emit brightnessChanged(brightness);
}

int QPhononVideoWidget::contrast() const
{
    return qRound(100.0*m_videoWidget->contrast());
}

void QPhononVideoWidget::setContrast(int contrast)
{
    m_videoWidget->setContrast(contrast/100.0);
    emit contrastChanged(contrast);
}

int QPhononVideoWidget::hue() const
{
    return qRound(100.0*m_videoWidget->hue());
}

void QPhononVideoWidget::setHue(int hue)
{
    m_videoWidget->setHue(hue/100.0);
    emit hueChanged(hue);
}

int QPhononVideoWidget::saturation() const
{
    return qRound(100.0*m_videoWidget->saturation());
}

void QPhononVideoWidget::setSaturation(int saturation)
{
    m_videoWidget->setSaturation(saturation/100.0);
    emit saturationChanged(saturation);
}
