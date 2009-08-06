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

#include <QtCore/qdebug.h>
#include <QWidget>

#include <linux/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include "v4lcamerasession.h"
#include "v4lvideowidget.h"


V4LCameraSession::V4LCameraSession(QObject *parent)
    :QObject(parent)
{
    available = false;
    m_state = QCamera::StoppedState;
    m_device = "/dev/video0";

    sfd = ::open(m_device.constData(), O_RDWR);

    if (sfd != -1) {
        available = true;
        ::close(sfd);
    }
    m_output = 0;
}

V4LCameraSession::~V4LCameraSession()
{
}

bool V4LCameraSession::deviceReady()
{
    return available;
}

void V4LCameraSession::setVideoOutput(QWidget* widget)
{
    m_output = qobject_cast<V4LVideoWidget*>(widget);
    m_output->setBaseSize(QSize(320,240));
}

int V4LCameraSession::framerate() const
{
    return -1;
}

void V4LCameraSession::setFrameRate(int rate)
{
    Q_UNUSED(rate)
}

int V4LCameraSession::brightness() const
{
    if(sfd == -1) return -1;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_BRIGHTNESS;
    if ((err = ::ioctl(sfd, VIDIOC_G_CTRL, &control_s)) < 0)
        return -1;

    return control_s.value;
}

void V4LCameraSession::setBrightness(int b)
{
    if(sfd == -1) return;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_BRIGHTNESS;
    control_s.value = b;
    if ((err = ::ioctl(sfd, VIDIOC_S_CTRL, &control_s)) < 0)
        return;
}

int V4LCameraSession::contrast() const
{
    if(sfd == -1) return -1;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_CONTRAST;
    if ((err = ::ioctl(sfd, VIDIOC_G_CTRL, &control_s)) < 0)
        return -1;

    return control_s.value;
}

void V4LCameraSession::setContrast(int c)
{
    if(sfd == -1) return;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_CONTRAST;
    control_s.value = c;
    if ((err = ::ioctl(sfd, VIDIOC_S_CTRL, &control_s)) < 0)
        return;
}

int V4LCameraSession::saturation() const
{
    if(sfd == -1) return -1;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_SATURATION;
    if ((err = ::ioctl(sfd, VIDIOC_G_CTRL, &control_s)) < 0)
        return -1;

    return control_s.value;
}

void V4LCameraSession::setSaturation(int s)
{
    if(sfd == -1) return;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_SATURATION;
    control_s.value = s;
    if ((err = ::ioctl(sfd, VIDIOC_S_CTRL, &control_s)) < 0)
        return;
}

int V4LCameraSession::hue() const
{
    if(sfd == -1) return -1;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_HUE;
    if ((err = ::ioctl(sfd, VIDIOC_G_CTRL, &control_s)) < 0)
        return -1;

    return control_s.value;
}

void V4LCameraSession::setHue(int h)
{
    if(sfd == -1) return;

    struct v4l2_control control_s;
    int err;
    control_s.id = V4L2_CID_HUE;
    control_s.value = h;
    if ((err = ::ioctl(sfd, VIDIOC_S_CTRL, &control_s)) < 0)
        return;
}

int V4LCameraSession::sharpness() const
{
    return -1;
}

void V4LCameraSession::setSharpness(int s)
{
    Q_UNUSED(s)
}

int V4LCameraSession::zoom() const
{
    return -1;
}

void V4LCameraSession::setZoom(int z)
{
    Q_UNUSED(z)
}

bool V4LCameraSession::backlightCompensation() const
{
    return false;
}

void V4LCameraSession::setBacklightCompensation(bool b)
{
    Q_UNUSED(b)
}

int V4LCameraSession::whitelevel() const
{
    return -1;
}

void V4LCameraSession::setWhitelevel(int w)
{
    Q_UNUSED(w)
}

int V4LCameraSession::rotation() const
{
    return 0;
}

void V4LCameraSession::setRotation(int r)
{
    Q_UNUSED(r)
}

bool V4LCameraSession::flash() const
{
    return false;
}

void V4LCameraSession::setFlash(bool f)
{
    Q_UNUSED(f)
}

bool V4LCameraSession::autofocus() const
{
    return false;
}

void V4LCameraSession::setAutofocus(bool f)
{
    Q_UNUSED(f)
}

QSize V4LCameraSession::frameSize() const
{
    return QSize(320,240);
}

void V4LCameraSession::setFrameSize(const QSize& s)
{
    Q_UNUSED(s)
}

void V4LCameraSession::setDevice(const QByteArray &device)
{
    qWarning()<<"setDevice "<<device;

    available = false;
    m_state = QCamera::StoppedState;
    m_device = device;

    sfd = ::open(m_device.constData(), O_RDWR);

    if (sfd != -1) {
        available = true;
        ::close(sfd);
    }
}

QList<QVideoFrame::PixelFormat> V4LCameraSession::supportedPixelFormats()
{
    QList<QVideoFrame::PixelFormat> list;

    if(available) {
        list << QVideoFrame::Format_RGB32;
    }

    return list;
}

QVideoFrame::PixelFormat V4LCameraSession::pixelFormat() const
{
    return QVideoFrame::Format_RGB32;

    return QVideoFrame::Format_Invalid;
}

void V4LCameraSession::setPixelFormat(QVideoFrame::PixelFormat fmt)
{
    Q_UNUSED(fmt)
    // ignore for example
}

QList<QSize> V4LCameraSession::supportedResolutions()
{
    QList<QSize> list;

    if(available) {
        list <<QSize(320, 240);
    }

    return list;
}

bool V4LCameraSession::setSink(const QMediaSink &sink)
{
    m_sink = sink;

    return true;
}

QMediaSink V4LCameraSession::sink() const
{
    return m_sink;
}

qint64 V4LCameraSession::position() const
{
    return 0;
}

int V4LCameraSession::state() const
{
    return int(m_state);
}
void V4LCameraSession::record()
{
    qWarning()<<"start";

    sfd = ::open(m_device.constData(), O_RDWR);
    if(sfd == -1) {
        qWarning()<<"can't open v4l "<<m_device;
        emit stateChanged(QCamera::StoppedState);
        return;
    }

    //struct v4l2_cropcap cropcap;
    //struct v4l2_crop crop;
    struct v4l2_format fmt;
    //unsigned int min;
    int ret;
/*
    memset(&cropcap, 0, sizeof(cropcap));
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ::ioctl(sfd, VIDIOC_CROPCAP, &cropcap);
    if(ret == 0) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect;
        ret = ::ioctl(sfd, VIDIOC_S_CROP, &crop);
    }
*/
    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = 320;
    fmt.fmt.pix.height      = 240;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    ret = ::ioctl(sfd, VIDIOC_S_FMT, &fmt);

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count               = 4;
    req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory              = V4L2_MEMORY_MMAP;
    ret = ::ioctl(sfd, VIDIOC_REQBUFS, &req);
    if(ret == -1) {
        qWarning()<<"error allocating buffers";
        emit stateChanged(QCamera::StoppedState);
        return;
    }
    qWarning()<<"buffers = "<<req.count;

    for(int i=0;i<(int)req.count;++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0 , sizeof(buf));
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;
        ret = ::ioctl(sfd, VIDIOC_QUERYBUF, &buf);
        if(ret == -1) {
            qWarning()<<"error allocating buffers";
            emit stateChanged(QCamera::StoppedState);
            return;
        }
        void* mmap_data = ::mmap(0,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,sfd,buf.m.offset);
        if (mmap_data == reinterpret_cast<void*>(-1)) {
            qWarning()<<"can't mmap video data";
            ::close(sfd);
            emit stateChanged(QCamera::StoppedState);
            return;
        }
        video_buffer  v4l_buf;
        v4l_buf.start = reinterpret_cast<char*>(mmap_data);
        v4l_buf.length = buf.length;
        buffers.append(v4l_buf);
    }

    // start stream
    for(int i=0;i<(int)req.count;++i) {
        struct v4l2_buffer buf;
        memset(&buf,0,sizeof(buf));
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;
        ret = ::ioctl(sfd, VIDIOC_QBUF, &buf);
        if(ret == -1) {
            qWarning()<<"can't mmap video data";
            ::close(sfd);
            emit stateChanged(QCamera::StoppedState);
            return;
        }
    }
    v4l2_buf_type buf_type;
    buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ::ioctl(sfd, VIDIOC_STREAMON, &buf_type);
    if(ret < 0) {
        qWarning()<<"can't start capture";
        ::close(sfd);
        emit stateChanged(QCamera::StoppedState);
        return;
    }
    notifier = new QSocketNotifier(sfd, QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)), this, SLOT(captureFrame()));
    notifier->setEnabled(1);

    m_state = QCamera::ActiveState;
    emit stateChanged(QCamera::ActiveState);
}

void V4LCameraSession::pause()
{
}

void V4LCameraSession::stop()
{
    if(sfd != -1) {
        v4l2_buf_type buf_type;
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ::ioctl(sfd, VIDIOC_STREAMOFF, &buf_type);
        if (notifier) {
            notifier->setEnabled(false);
            disconnect(notifier, 0, 0, 0);
            delete notifier;
            notifier = 0;
        }
        ::close(sfd);
        m_state = QCamera::StoppedState;
        emit stateChanged(QCamera::StoppedState);
    }
}

void V4LCameraSession::captureFrame()
{
    if(sfd == -1) return;

    qWarning()<<"TODO:captureFrame()";
    v4l2_buffer buf;
    memset(&buf, 0, sizeof(struct v4l2_buffer));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    int ret = ioctl(sfd, VIDIOC_DQBUF, &buf);
    if (ret < 0 ) {
        qWarning()<<"error reading frame";
        return;
    }

    qWarning()<<"size: "<<buf.bytesused<<", time: "<<(quint64)buf.timestamp.tv_sec/1000000 + (quint64)buf.timestamp.tv_usec*1000000;;
    qWarning()<<"index = "<<buf.index;
    qWarning()<<"l="<<buffers.at(buf.index).length;
    qWarning()<<"*="<<buffers.at(buf.index).start;
    qWarning()<<"m_output = "<<m_output;

    if(m_output) {
        m_output->setLength(buffers.at(buf.index).length);
        m_output->setData((char*)buffers.at(buf.index).start);
        m_output->repaint();
    }

    ret = ioctl(sfd, VIDIOC_QBUF, &buf);
}
