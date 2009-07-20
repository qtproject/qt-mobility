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

#include "cameracontrol.h"
#include "cameraservice.h"
#include "qmediasource.h"
#include "qvideorendererendpoint.h"

#include <QtMultimedia/qvideocamera.h>

#include <QtCore/qdebug.h>

CameraControl::CameraControl(QObject *parent)
    :QCameraControl(parent)
{
    m_camera = 0;
}

CameraControl::CameraControl(CameraService *service, QObject *parent)
   :QCameraControl(parent), m_service(service)
{
    m_camera = 0;
}

CameraControl::~CameraControl()
{
    if(m_camera) delete m_camera;
}

QList<QVideoFrame::Type> CameraControl::supportedColorFormats()
{
    QList<QVideoFrame::Type> list;

    if(m_camera) {
        list = m_camera->preview()->supportedColorFormats();
    }

    return list;
}

QList<QSize> CameraControl::supportedResolutions(QVideoFrame::Type fmt)
{
    QList<QSize> list;

    if(m_camera) {
        list = m_camera->preview()->supportedResolutions(fmt);
    }

    return list;
}

QVideoFormat CameraControl::format() const
{
    if(m_camera) {
        return m_camera->preview()->format();
    }

    return QVideoFormat();
}

void CameraControl::setFormat(const QVideoFormat &format)
{
    if(m_camera) {
        m_camera->preview()->setFormat(format);
    }
}

void CameraControl::start()
{
    if(m_camera) {
        m_camera->preview()->start();
    }
}

void CameraControl::stop()
{
    if(m_camera) {
        m_camera->preview()->stop();
    }
}

int CameraControl::framerate() const
{
    if(m_camera)
        m_camera->framerate();

    return 0;
}

void CameraControl::setFrameRate(int rate)
{
    if(m_camera)
        m_camera->setFramerate(rate);
}

int CameraControl::brightness() const
{
    if(m_camera)
        m_camera->brightness();

    return 0;
}

void CameraControl::setBrightness(int b)
{
    if(m_camera)
        m_camera->setBrightness(b);
}

int CameraControl::contrast() const
{
    if(m_camera)
        m_camera->contrast();

    return 0;
}

void CameraControl::setContrast(int c)
{
    if(m_camera)
        m_camera->setContrast(c);
}

int CameraControl::saturation() const
{
    if(m_camera)
        m_camera->saturation();

    return 0;
}

void CameraControl::setSaturation(int s)
{
    if(m_camera)
        m_camera->setSaturation(s);
}

int CameraControl::hue() const
{
    if(m_camera)
        m_camera->hue();

    return 0;
}

void CameraControl::setHue(int h)
{
    if(m_camera)
        m_camera->setHue(h);
}

int CameraControl::sharpness() const
{
    if(m_camera)
        m_camera->sharpness();

    return 0;
}

void CameraControl::setSharpness(int s)
{
    if(m_camera)
        m_camera->setSharpness(s);
}

int CameraControl::zoom() const
{
    if(m_camera)
        m_camera->zoom();

    return 0;
}

void CameraControl::setZoom(int z)
{
    if(m_camera)
        m_camera->setZoom(z);
}

bool CameraControl::backlightCompensation() const
{
    if(m_camera)
        m_camera->backlightCompensation();

    return false;
}

void CameraControl::setBacklightCompensation(bool b)
{
    if(m_camera)
        m_camera->setBacklightCompensation(b);
}

int CameraControl::whitelevel() const
{
    if(m_camera)
        m_camera->whitelevel();

    return 0;
}

void CameraControl::setWhitelevel(int w)
{
    if(m_camera)
        m_camera->setWhitelevel(w);
}

int CameraControl::rotation() const
{
    if(m_camera)
        m_camera->rotation();

    return 0;
}

void CameraControl::setRotation(int r)
{
    if(m_camera)
        m_camera->setRotation(r);
}

bool CameraControl::flash() const
{
    if(m_camera)
        m_camera->flash();

    return false;
}

void CameraControl::setFlash(bool f)
{
    if(m_camera)
        m_camera->setFlash(f);
}

bool CameraControl::autofocus() const
{
    if(m_camera)
        m_camera->autofocus();

    return false;
}

void CameraControl::setAutofocus(bool f)
{
    if(m_camera)
        m_camera->setAutofocus(f);
}

bool CameraControl::isValid() const
{
    if(m_camera)
        return true;
    else
        return false;
}

void CameraControl::setDevice(QByteArray device)
{
    if(m_camera) delete m_camera;
    m_camera = new QVideoCamera(device,this);
    connect(m_camera,SIGNAL(frameReady(QVideoFrame const&)),this,SIGNAL(frameReady(QVideoFrame const&)));
    connect(m_camera,SIGNAL(stateChanged(QVideoStream::State)),this,SIGNAL((stateChanged(QVideoStream::State))));
}



