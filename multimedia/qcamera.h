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

#ifndef QCAMERA_H
#define QCAMERA_H

#include <QList>
#include <QStringList>

#include "qabstractmediacontrol.h"
#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"

#include "qmediaserviceprovider.h"

#ifdef VIDEOSERVICES
#include <QtMultimedia/qvideostream.h>
#include <QtMultimedia/qvideoframe.h>
#endif

class QCameraService;
class QCameraControl;

extern QCameraService *createCameraService(QMediaServiceProvider *provider = defaultServiceProvider("camera"));

class QCameraPrivate;

class QCamera : public QAbstractMediaObject
{
    Q_OBJECT
#ifdef VIDEOSERVICES
    Q_PROPERTY(QVideoFormat format READ format WRITE setFormat)
    Q_PROPERTY(QVideoStream::State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int framerate READ framerate WRITE setFrameRate)
    Q_PROPERTY(int brightness READ brightness WRITE setBrightness)
    Q_PROPERTY(int contrast READ contrast WRITE setContrast)
    Q_PROPERTY(int saturation READ saturation WRITE setSaturation)
    Q_PROPERTY(int hue READ hue WRITE setHue)
    Q_PROPERTY(int sharpness READ sharpness WRITE setSharpness)
    Q_PROPERTY(int zoom READ zoom WRITE setZoom)
    Q_PROPERTY(bool backlightCompensation READ backlightCompensation WRITE setBacklightCompensation)
    Q_PROPERTY(int whitelevel READ whitelevel WRITE setWhitelevel)
    Q_PROPERTY(int rotation READ rotation WRITE setRotation)
    Q_PROPERTY(bool flash READ flash WRITE setFlash)
    Q_PROPERTY(bool autofocus READ autofocus WRITE setAutofocus)
#endif
public:
    QCamera(QCameraService *service = createCameraService(), QObject *parent = 0);
    ~QCamera();

#ifdef VIDEOSERVICES
    QList<QByteArray> deviceList();
    QList<QVideoFrame::Type> supportedColorFormats();
    QList<QSize> supportedResolutions(QVideoFrame::Type fmt);

    QVideoFormat format() const;
    void setFormat(const QVideoFormat &format);

    void start();
    void stop();

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
    void setZoom(int);

    bool backlightCompensation() const;
    void setBacklightCompensation(bool);

    int whitelevel() const;
    void setWhitelevel(int);

    int rotation() const;
    void setRotation(int);

    bool flash() const;
    void setFlash(bool);

    bool autofocus() const;
    void setAutofocus(bool);

    void setDevice(const QByteArray &device);

    QVideoStream::State state() const;
#endif
    bool isValid() const;

    QAbstractMediaService* service() const;

#ifdef VIDEOSERVICES
Q_SIGNALS:
    void frameReady(QVideoFrame const &);
    void stateChanged(QVideoStream::State);
#endif
private:
    Q_DISABLE_COPY(QCamera)
    Q_DECLARE_PRIVATE(QCamera)
};

#endif  // QCAMERA_H
