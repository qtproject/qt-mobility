/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstreamercameracontrol.h"

#include <QtCore/qdebug.h>

QGstreamerCameraControl::QGstreamerCameraControl(QGstreamerCaptureSession *session)
    :QCameraControl(session), m_session(session), m_state(QCamera::StoppedState)
{
    connect(m_session, SIGNAL(stateChanged(QGstreamerCaptureSession::State)),
            this, SLOT(updateState()));
}

QGstreamerCameraControl::~QGstreamerCameraControl()
{
}

GstElement *QGstreamerCameraControl::buildElement()
{
    //TODO: add caps filter with desired camera settings, like resolution, framerate, etc
    GstElement *camera = gst_element_factory_make("v4l2src", "camera_source");

    if (camera && !m_device.isEmpty() )
        g_object_set(G_OBJECT(camera), "device", m_device.constData(), NULL);

    return camera;
}

void QGstreamerCameraControl::start()
{
    m_session->enablePreview(true);
}

void QGstreamerCameraControl::stop()
{
    m_session->enablePreview(false);
}

void QGstreamerCameraControl::setDevice(const QString &device)
{
    qDebug() << "QGstreamerCameraControl::setDevice" << device;
    m_device = device.toLocal8Bit().constData();
}

QCamera::State QGstreamerCameraControl::state() const
{
    if (m_session->state() == QGstreamerCaptureSession::StoppedState)
        return QCamera::StoppedState;
    else
        return QCamera::ActiveState;
}

void QGstreamerCameraControl::updateState()
{
    QCamera::State newState = state();
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(m_state);
    }
}
