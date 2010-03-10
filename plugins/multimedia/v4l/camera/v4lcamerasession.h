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

#ifndef V4LCAMERASESSION_H
#define V4LCAMERASESSION_H

#include <QtCore/qobject.h>
#include <QtCore/qsocketnotifier.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qurl.h>
#include <QtCore/qfile.h>
#include <QtMultimedia/qvideoframe.h>
#include <QtMultimedia/qabstractvideosurface.h>

#include <qmediarecorder.h>
#include <experimental/qcamera.h>

#include "cameraformatconverter.h"

QTM_USE_NAMESPACE

class V4LVideoRenderer;

struct video_buffer {
    void* start;
    size_t length;
};

class V4LCameraSession : public QObject
{
    Q_OBJECT
public:
    V4LCameraSession(QObject *parent = 0);
    ~V4LCameraSession();

    bool deviceReady();

    // camera controls

    int framerate() const;
    void setFrameRate(int rate);
    int brightness() const;
    void setBrightness(int b);
    int contrast() const;
    void setContrast(int c);
    int saturation() const;
    void setSaturation(int s);
    int hue() const;
    void setHue(int h);
    int sharpness() const;
    void setSharpness(int s);
    int zoom() const;
    void setZoom(int z);
    bool backlightCompensation() const;
    void setBacklightCompensation(bool);
    int whitelevel() const;
    void setWhitelevel(int w);
    int rotation() const;
    void setRotation(int r);
    bool flash() const;
    void setFlash(bool f);
    bool autofocus() const;
    void setAutofocus(bool f);

    QSize frameSize() const;
    void setFrameSize(const QSize& s);
    void setDevice(const QString &device);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats();
    QVideoFrame::PixelFormat pixelFormat() const;
    void setPixelFormat(QVideoFrame::PixelFormat fmt);
    QList<QSize> supportedResolutions();

    // media control

    bool setOutputLocation(const QUrl &sink);
    QUrl outputLocation() const;
    qint64 position() const;
    QMediaRecorder::State state() const;
    void record();
    void pause();
    void stop();

    void setSurface(QAbstractVideoSurface* surface);

    void captureImage(const QString &fileName);

    void previewMode(bool value);
    void captureToFile(bool value);

Q_SIGNALS:
    void durationChanged(qint64 position);
    void error(int error, const QString &errorString);
    void cameraStateChanged(QCamera::State);
    void recordStateChanged(QMediaRecorder::State);
    void imageCaptured(const QString &fileName, const QImage &img);

private Q_SLOTS:
    void captureFrame();

private:
    bool isFormatSupported(QVideoFrame::PixelFormat fmt);

    QSocketNotifier *notifier;
    QList<video_buffer> buffers;

    int sfd;
    QTime timeStamp;
    bool available;
    bool preview;
    bool toFile;
    bool active;
    QMediaRecorder::State m_state;
    QByteArray m_device;
    QUrl m_sink;
    QFile m_file;
    V4LVideoRenderer*   m_output;
    QAbstractVideoSurface* m_surface;
    QVideoFrame::PixelFormat pixelF;
    QVideoFrame::PixelFormat savedPixelF;
    QSize m_windowSize;
    QList<QSize> resolutions;
    QList<unsigned int> formats;

    CameraFormatConverter* converter;

    QString m_snapshot;
};

#endif
