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

#include "v4lcameracontrol.h"
#include "v4lcameraservice.h"
#include "v4lcamerasession.h"

#include <QtCore/qdebug.h>


V4LCameraControl::V4LCameraControl(QObject *parent)
    :QCameraControl(parent)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);
}

V4LCameraControl::V4LCameraControl(V4LCameraService *service, QObject *parent)
   :QCameraControl(parent), m_service(service)
{
    m_session = qobject_cast<V4LCameraSession*>(parent);
}

V4LCameraControl::~V4LCameraControl()
{
}

QList<QSize> V4LCameraControl::supportedResolutions()
{
    QList<QSize> list;

    if(m_session->deviceReady())
        list << m_session->supportedResolutions();

    return list;
}

int V4LCameraControl::framerate() const
{
    return m_session->framerate();
}

void V4LCameraControl::setFrameRate(int rate)
{
    m_session->setFrameRate(rate);
}

int V4LCameraControl::brightness() const
{
    return m_session->brightness();
}

void V4LCameraControl::setBrightness(int b)
{
    m_session->setBrightness(b);
}

int V4LCameraControl::contrast() const
{
    return m_session->contrast();
}

void V4LCameraControl::setContrast(int c)
{
    m_session->setContrast(c);
}

int V4LCameraControl::saturation() const
{
    return m_session->saturation();
}

void V4LCameraControl::setSaturation(int s)
{
    m_session->setSaturation(s);
}

int V4LCameraControl::hue() const
{
    return m_session->hue();
}

void V4LCameraControl::setHue(int h)
{
    m_session->setHue(h);
}

int V4LCameraControl::sharpness() const
{
    return m_session->sharpness();
}

void V4LCameraControl::setSharpness(int s)
{
    m_session->setSharpness(s);
}

int V4LCameraControl::zoom() const
{
    return m_session->zoom();
}

void V4LCameraControl::setZoom(int z)
{
    m_session->setZoom(z);
}

bool V4LCameraControl::backlightCompensation() const
{
    return m_session->backlightCompensation();
}

void V4LCameraControl::setBacklightCompensation(bool b)
{
    m_session->setBacklightCompensation(b);
}

int V4LCameraControl::whitelevel() const
{
    return m_session->whitelevel();
}

void V4LCameraControl::setWhitelevel(int w)
{
    m_session->setWhitelevel(w);
}

int V4LCameraControl::rotation() const
{
    return m_session->rotation();
}

void V4LCameraControl::setRotation(int r)
{
    m_session->setRotation(r);
}

bool V4LCameraControl::flash() const
{
    return m_session->flash();
}

void V4LCameraControl::setFlash(bool f)
{
    m_session->setFlash(f);
}

bool V4LCameraControl::autofocus() const
{
    return m_session->autofocus();
}

void V4LCameraControl::setAutofocus(bool f)
{
    m_session->setAutofocus(f);
}

bool V4LCameraControl::isValid() const
{
    return m_session->deviceReady();
}

void V4LCameraControl::setDevice(const QByteArray &device)
{
    m_session->setDevice(device);
}

QSize V4LCameraControl::frameSize() const
{
    return m_session->frameSize();
}

void V4LCameraControl::setFrameSize(const QSize& s)
{
    m_session->setFrameSize(s);
}


