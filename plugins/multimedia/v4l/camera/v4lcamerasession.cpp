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

#include <QtGui/qwidget.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtMultimedia/qabstractvideobuffer.h>
#include <QtMultimedia/qvideosurfaceformat.h>

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
#include "v4lvideorenderer.h"
#include "v4lvideobuffer.h"


V4LCameraSession::V4LCameraSession(QObject *parent)
    :QObject(parent)
{
    available = false;
    resolutions.clear();
    formats.clear();
    m_state = QMediaRecorder::StoppedState;
    m_device = "/dev/video1";
    preview = false;
    toFile = false;
    converter = 0;
    active  = false;

    sfd = ::open(m_device.constData(), O_RDWR);

    if (sfd != -1) {
        available = true;

        // get formats available
        v4l2_fmtdesc fmt;
        memset(&fmt, 0, sizeof(v4l2_fmtdesc));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        int sanity = 0;
        for (fmt.index = 0;; fmt.index++) {
            if (sanity++ > 8)
                break;
            if(  ::ioctl(sfd, VIDIOC_ENUM_FMT, &fmt) == -1) {
                if(errno == EINVAL)
                    break;
            }
            formats.append(fmt.pixelformat);
        }

        // get sizes available
        resolutions << QSize(176, 144) << QSize(320, 240) << QSize(640, 480);

        ::close(sfd);
        sfd = -1;
    }
    m_output = 0;
    m_surface = 0;
    m_windowSize = QSize(320,240);
    pixelF = QVideoFrame::Format_RGB32;
    savedPixelF = QVideoFrame::Format_RGB32;
}

V4LCameraSession::~V4LCameraSession()
{
}

void V4LCameraSession::captureImage(const QString &fileName)
{
    m_snapshot = fileName;
}

void V4LCameraSession::setSurface(QAbstractVideoSurface* surface)
{
    m_surface = surface;
}

bool V4LCameraSession::deviceReady()
{
    return available;
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
    return m_windowSize;
}

void V4LCameraSession::setFrameSize(const QSize& s)
{
    m_windowSize = s;
}

void V4LCameraSession::setDevice(const QString &device)
{
    available = false;
    m_state = QMediaRecorder::StoppedState;
    m_device = QByteArray(device.toLocal8Bit().constData());

    sfd = ::open(m_device.constData(), O_RDWR);

    if (sfd != -1) {
        available = true;

        // get formats available
        v4l2_fmtdesc fmt;
        memset(&fmt, 0, sizeof(v4l2_fmtdesc));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        int sanity = 0;
        for (fmt.index = 0;; fmt.index++) {
            if (sanity++ > 8)
                break;
            if(  ::ioctl(sfd, VIDIOC_ENUM_FMT, &fmt) == -1) {
                if(errno == EINVAL)
                    break;
            }
            formats.append(fmt.pixelformat);
        }

        // get sizes available
        resolutions << QSize(176, 144) << QSize(320, 240) << QSize(640, 480);

        ::close(sfd);
        sfd = -1;
    }
    active = false;
}

QList<QVideoFrame::PixelFormat> V4LCameraSession::supportedPixelFormats()
{
    QList<QVideoFrame::PixelFormat> list;

    if(available) {
        for(int i=0;i<formats.size();i++) {
            if(formats.at(i) == V4L2_PIX_FMT_YUYV)
                list << QVideoFrame::Format_YUYV;
            else if(formats.at(i) == V4L2_PIX_FMT_UYVY)
                list << QVideoFrame::Format_UYVY;
            else if(formats.at(i) == V4L2_PIX_FMT_RGB24)
                list << QVideoFrame::Format_RGB24;
            else if(formats.at(i) == V4L2_PIX_FMT_RGB32)
                list << QVideoFrame::Format_RGB32;
            else if(formats.at(i) == V4L2_PIX_FMT_RGB565)
                list << QVideoFrame::Format_RGB565;
        }
    }

    return list;
}

QVideoFrame::PixelFormat V4LCameraSession::pixelFormat() const
{
    return pixelF;
}

void V4LCameraSession::setPixelFormat(QVideoFrame::PixelFormat fmt)
{
    pixelF = fmt;
    savedPixelF = fmt;
}

QList<QSize> V4LCameraSession::supportedResolutions()
{
    QList<QSize> list;

    if(available) {
        list << resolutions;
    }

    return list;
}

bool V4LCameraSession::setOutputLocation(const QUrl &sink)
{
    m_sink = sink;

    return true;
}

QUrl V4LCameraSession::outputLocation() const
{
    return m_sink;
}

qint64 V4LCameraSession::position() const
{
    return timeStamp.elapsed();
}

QMediaRecorder::State V4LCameraSession::state() const
{
    return m_state;
}

void V4LCameraSession::previewMode(bool value)
{
    preview = value;
}

void V4LCameraSession::captureToFile(bool value)
{
    if(toFile && m_file.isOpen())
        m_file.close();

    toFile = value;
}

bool V4LCameraSession::isFormatSupported(QVideoFrame::PixelFormat pFormat)
{
    if(sfd == -1)
        return false;

    int ret;
    struct v4l2_format fmt;

    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = m_windowSize.width();
    fmt.fmt.pix.height      = m_windowSize.height();

    if(pFormat == QVideoFrame::Format_YUYV)
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    if(pFormat == QVideoFrame::Format_RGB24)
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
    if(pFormat == QVideoFrame::Format_RGB32)
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
    if(pFormat == QVideoFrame::Format_RGB565)
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
    if(pFormat == QVideoFrame::Format_UYVY)
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;

    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    ret = ::ioctl(sfd, VIDIOC_S_FMT, &fmt);

    if(ret == -1)
        return false;

    return true;
}

void V4LCameraSession::record()
{
    pixelF = savedPixelF;

    if(active && toFile && m_state != QMediaRecorder::PausedState) {
        // Camera is active and captureToFile(true)
        m_state = QMediaRecorder::RecordingState;
        emit recordStateChanged(m_state);
        return;
    } else if(sfd > 0)
        return;

    sfd = ::open(m_device.constData(), O_RDWR);
    if(sfd == -1) {
        qWarning()<<"can't open v4l "<<m_device;
        m_state = QMediaRecorder::StoppedState;
        emit cameraStateChanged(QCamera::StoppedState);
        return;
    }

    bool match = false;
    QList<QVideoFrame::PixelFormat> fmts = supportedPixelFormats();

    // first try and use what we have
    if ((fmts.contains(pixelF)) && isFormatSupported(pixelF) &&
        m_surface->isFormatSupported(QVideoSurfaceFormat(m_windowSize,pixelF)))
        match = true;

    // try and find a match between camera and surface that doesn't require a converter
    if(!match) {
        foreach(QVideoFrame::PixelFormat format, fmts) {
            if (m_surface->isFormatSupported(QVideoSurfaceFormat(m_windowSize,format))) {
                // found a match, try to use it!
                if(isFormatSupported(format)) {
                    match = true;
                    pixelF = format;
                    break;
                }
            }
        }
    }

    // try to see if we can use the converter
    if(!match) {
        // no direct match up found, see if we can use the converter
        if(m_surface->isFormatSupported(QVideoSurfaceFormat(m_windowSize,QVideoFrame::Format_RGB32)) ||
                m_surface->isFormatSupported(QVideoSurfaceFormat(m_windowSize,QVideoFrame::Format_RGB24)) ||
                m_surface->isFormatSupported(QVideoSurfaceFormat(m_windowSize,QVideoFrame::Format_RGB565))) {
            // converter can convert YUYV->RGB565, UYVY->RGB565 and YUV420P->RGB565
            if (pixelF == QVideoFrame::Format_YUYV || pixelF == QVideoFrame::Format_UYVY ||
                    pixelF == QVideoFrame::Format_YUV420P) {
                if(isFormatSupported(pixelF)) {
                    match = true;
                    converter = CameraFormatConverter::createFormatConverter(pixelF,m_windowSize.width(),
                            m_windowSize.height());
                }
            }
            if (!match) {
                // fallback, cant convert your format so set to one that I can get working!
                foreach(QVideoFrame::PixelFormat format, fmts) {
                    if(format == QVideoFrame::Format_YUYV || format == QVideoFrame::Format_UYVY ||
                            format == QVideoFrame::Format_YUV420P) {
                        if(isFormatSupported(format)) {
                            pixelF = format;
                            match = true;
                            converter = CameraFormatConverter::createFormatConverter(pixelF,m_windowSize.width(),
                                    m_windowSize.height());
                            break;
                        }
                    }
                }
            }
        }
    }
    if(!match) {
        qWarning() << "error setting camera format, no supported formats available";
        m_state = QMediaRecorder::StoppedState;
        emit cameraStateChanged(QCamera::StoppedState);
        return;
    }

    int ret;
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count               = 4;
    req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory              = V4L2_MEMORY_MMAP;
    ret = ::ioctl(sfd, VIDIOC_REQBUFS, &req);
    if(ret == -1) {
        qWarning()<<"error allocating buffers";
        m_state = QMediaRecorder::StoppedState;
        emit cameraStateChanged(QCamera::StoppedState);
        return;
    }
    for(int i=0;i<(int)req.count;++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0 , sizeof(buf));
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;
        ret = ::ioctl(sfd, VIDIOC_QUERYBUF, &buf);
        if(ret == -1) {
            qWarning()<<"error allocating buffers";
            m_state = QMediaRecorder::StoppedState;
            emit cameraStateChanged(QCamera::StoppedState);
            return;
        }
        void* mmap_data = ::mmap(0,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,sfd,buf.m.offset);
        if (mmap_data == reinterpret_cast<void*>(-1)) {
            qWarning()<<"can't mmap video data";
            ::close(sfd);
            sfd = -1;
            m_state = QMediaRecorder::StoppedState;
            emit cameraStateChanged(QCamera::StoppedState);
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
            sfd = -1;
            m_state = QMediaRecorder::StoppedState;
            emit cameraStateChanged(QCamera::StoppedState);
            return;
        }
    }
    v4l2_buf_type buf_type;
    buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ::ioctl(sfd, VIDIOC_STREAMON, &buf_type);
    if(ret < 0) {
        qWarning()<<"can't start capture";
        ::close(sfd);
        sfd = -1;
        m_state = QMediaRecorder::StoppedState;
        emit cameraStateChanged(QCamera::StoppedState);
        return;
    }
    notifier = new QSocketNotifier(sfd, QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)), this, SLOT(captureFrame()));
    notifier->setEnabled(1);
    if (!converter) {
        QVideoSurfaceFormat requestedFormat(m_windowSize,pixelF);

        bool check = m_surface->isFormatSupported(requestedFormat);

        if(check) {
            m_surface->start(requestedFormat);

            m_state = QMediaRecorder::RecordingState;
            emit cameraStateChanged(QCamera::ActiveState);
            timeStamp.restart();
        }
    } else {
        QVideoSurfaceFormat requestedFormat(m_windowSize,QVideoFrame::Format_RGB32);
        m_surface->start(requestedFormat);
        m_state = QMediaRecorder::RecordingState;
        emit cameraStateChanged(QCamera::ActiveState);
        timeStamp.restart();
    }
    active = true;
}

void V4LCameraSession::pause()
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
        sfd = -1;
        m_state = QMediaRecorder::PausedState;
        emit recordStateChanged(m_state);
    }
}

void V4LCameraSession::stop()
{
    if(sfd != -1) {

        if(toFile && m_file.isOpen() && m_state != QMediaRecorder::StoppedState) {
            // just stop writing to file.
            m_file.close();

            m_state = QMediaRecorder::StoppedState;
            emit recordStateChanged(m_state);
            return;
        }

        v4l2_buf_type buf_type;
        buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ::ioctl(sfd, VIDIOC_STREAMOFF, &buf_type);
        if (notifier) {
            notifier->setEnabled(false);
            disconnect(notifier, 0, 0, 0);
            delete notifier;
            notifier = 0;
        }
        // Dequeue remaining buffers
        for(int i = 0;i < 4; ++i) {
            v4l2_buffer buf;
            memset(&buf, 0, sizeof(struct v4l2_buffer));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            if (ioctl(sfd, VIDIOC_QUERYBUF, &buf) == 0) {
                if (buf.flags & V4L2_BUF_FLAG_QUEUED) {
                    ::ioctl(sfd, VIDIOC_DQBUF, &buf);
                }
            }
            munmap(buffers.at(buf.index).start,buf.length);
        }

        ::close(sfd);
        sfd = -1;
        m_state = QMediaRecorder::StoppedState;
        emit cameraStateChanged(QCamera::StoppedState);

        if(converter)
            delete converter;
        converter = 0;
        active = false;
    }
}

void V4LCameraSession::captureFrame()
{
    if(sfd == -1) return;
    v4l2_buffer buf;
    memset(&buf, 0, sizeof(struct v4l2_buffer));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    int ret = ioctl(sfd, VIDIOC_DQBUF, &buf);
    if (ret < 0 ) {
        qWarning()<<"error reading frame";
        return;
    }

    if(m_surface) {
        if(converter) {
            QImage image;
            image = QImage(converter->convert((unsigned char*)buffers.at(buf.index).start,buf.bytesused),
             m_windowSize.width(),m_windowSize.height(),QImage::Format_RGB16).convertToFormat(QImage::Format_RGB32);
            if(m_snapshot.length() > 0) {
                image.save(m_snapshot,"JPG");
                m_snapshot.clear();
            }
            if(m_sink.toString().length() > 0 && toFile) {
                // save to file
                if(!m_file.isOpen()) {
                    if(m_sink.toLocalFile().length() > 0)
                        m_file.setFileName(m_sink.toLocalFile());
                    else
                        m_file.setFileName(m_sink.toString());
                    m_file.open(QIODevice::WriteOnly);
                }
                image.save(qobject_cast<QIODevice*>(&m_file),"JPG");
            }
            if(preview) {
                QVideoFrame frame(image);
                m_surface->present(frame);
            }
            ret = ioctl(sfd, VIDIOC_QBUF, &buf);

        } else {
            QVideoSurfaceFormat sfmt = m_surface->surfaceFormat();
            if(sfmt.pixelFormat() == QVideoFrame::Format_RGB565) {
                QImage image;
                image = QImage((unsigned char*)buffers.at(buf.index).start,
                        m_windowSize.width(), m_windowSize.height(), QImage::Format_RGB16);
                QVideoFrame frame(image);
                m_surface->present(frame);
                ret = ioctl(sfd, VIDIOC_QBUF, &buf);

            } else {
                V4LVideoBuffer* packet = new V4LVideoBuffer((unsigned char*)buffers.at(buf.index).start, sfd, buf);
                packet->setBytesPerLine(m_windowSize.width()*4);
                QVideoFrame frame(packet,m_windowSize,pixelF);
                frame.setStartTime(buf.timestamp.tv_sec);
                m_surface->present(frame);
            }
        }

    } else
        ret = ioctl(sfd, VIDIOC_QBUF, &buf);
}
