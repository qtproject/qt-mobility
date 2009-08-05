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

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>

#include "endpoints/qvideorendererendpoint.h"

#include "v4lcameraservice.h"
#include "v4lcameracontrol.h"
#include "v4lmediacontrol.h"

V4LCameraService::V4LCameraService(QObject *parent)
    : QCameraService(parent)
{
    m_control = new V4LCameraControl(this, this);
    m_media = new V4LMediaControl(this);
    m_media->setCameraControl(m_control);
}

V4LCameraService::~V4LCameraService()
{
    delete m_media;
    delete m_control;
}

QAbstractMediaControl *V4LCameraService::control(const char *name) const
{
    qWarning()<<"eeeeeeeeeeee "<<name;

    if (qstrcmp(name,"com.nokia.qt.MediaRecorderControl") == 0) {
        qWarning()<<"found media control";
        return m_media;
    }

    if(qstrcmp(name,"com.nokia.qt.CameraControl") == 0) {
        qWarning()<<"found camera control";
        return m_control;
    }

    return 0;
}

QList<QByteArray> V4LCameraService::supportedEndpointInterfaces(
        QMediaEndpointInterface::Direction direction) const
{
    QList<QByteArray> list;

    if (direction == QMediaEndpointInterface::Input)
        list << QByteArray(QVideoRendererEndpoint_iid);

    return list;
}

QObject *V4LCameraService::createEndpoint(const char *interface)
{
    if (qstrcmp(interface, QVideoRendererEndpoint_iid) == 0) {
        return new QVideoRendererEndpoint(this);
    }

    return 0;
}

QList<QByteArray> V4LCameraService::deviceList()
{
    QList<QByteArray> devices;

    devices << "/dev/video0" << "/dev/video1";

    return devices;
}

