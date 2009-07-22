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

#ifndef QCAMERACONTROL_H
#define QCAMERACONTROL_H

#include "qabstractmediacontrol.h"
#include "qabstractmediaobject.h"

#ifdef VIDEOSERVICES
#include <QtMultimedia/qvideoformat.h>
#include <QtMultimedia/qvideostream.h>
#endif

class QCameraControl : public QAbstractMediaControl
{
    Q_OBJECT

public:
    ~QCameraControl();

#ifdef VIDEOSERVICES
    virtual QList<QVideoFrame::Type> supportedColorFormats() = 0;
    virtual QList<QSize> supportedResolutions(QVideoFrame::Type fmt) = 0;

    virtual QVideoFormat format() const = 0;
    virtual void setFormat(const QVideoFormat &format) = 0;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual int framerate() const = 0;
    virtual void setFrameRate(int rate) = 0;

    virtual int brightness() const = 0;
    virtual void setBrightness(int b) = 0;

    virtual int contrast() const = 0;
    virtual void setContrast(int c) = 0;

    virtual int saturation() const = 0;
    virtual void setSaturation(int s) = 0;

    virtual int hue() const = 0;
    virtual void setHue(int h) = 0;

    virtual int sharpness() const = 0;
    virtual void setSharpness(int s) = 0;

    virtual int zoom() const = 0;
    virtual void setZoom(int z) = 0;

    virtual bool backlightCompensation() const = 0;
    virtual void setBacklightCompensation(bool) = 0;

    virtual int whitelevel() const = 0;
    virtual void setWhitelevel(int w) = 0;

    virtual int rotation() const = 0;
    virtual void setRotation(int r) = 0;

    virtual bool flash() const = 0;
    virtual void setFlash(bool f) = 0;

    virtual bool autofocus() const = 0;
    virtual void setAutofocus(bool f) = 0;

    virtual void setDevice(const QByteArray &device) = 0;

    virtual QVideoStream::State state() const = 0;
#endif
    virtual bool isValid() const = 0;
Q_SIGNALS:
#ifdef VIDEOSERVICES
    void frameReady(QVideoFrame const&);
    void stateChanged(QVideoStream::State);
#endif
protected:
    QCameraControl(QObject* parent);
};

#endif  // QCAMERACONTROL_H

