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


#ifndef QGSTREAMERCAPTURESESSION_H
#define QGSTREAMERCAPTURESESSION_H

#include "qmediacapturecontrol.h"
#include "qmediasink.h"
#include "qmediacapture.h"

#include <gst/gst.h>

class QGstreamerMessage;
class QGstreamerBusHelper;
class QGstreamerAudioEncode;

class QGstreamerCaptureSession : public QMediaCaptureControl
{
    Q_OBJECT
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
public:
    QGstreamerCaptureSession(QObject *parent);
    ~QGstreamerCaptureSession();

    QMediaSink sink() const;
    bool setSink(const QMediaSink& sink);

    int state() const;

    qint64 position() const;
    void setPositionUpdatePeriod(int ms);

    QGstreamerAudioEncode *audioEncodeControl() const { return m_audioEncodeControl; }

signals:
    void stateChanged(int state);
    void positionChanged(qint64 position);

public slots:
    void record();
    void pause();
    void stop();

    void setCaptureDevice(const QString &deviceName);

private slots:
    void busMessage(const QGstreamerMessage &message);

private:
    QMediaSink m_sink;
    QMediaCapture::State m_state;
    QGstreamerBusHelper *m_busHelper;
    QGstreamerAudioEncode *m_audioEncodeControl;
    GstBus* m_bus;

    GstElement *m_pipeline;

    GstElement *m_audiosrc;
    GstElement *m_tee;
    GstElement *m_audioconvert1;
    GstElement *m_volume;
    GstElement *m_encoder;    
    GstElement *m_filesink;
    GstElement *m_audioconvert2;
    GstElement *m_fakesink;
};

#endif // QGSTREAMERCAPTURESESSION_H
