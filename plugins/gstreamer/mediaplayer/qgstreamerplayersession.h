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

#ifndef QGSTREAMERPLAYERSESSION_H
#define QGSTREAMERPLAYERSESSION_H

#include <QObject>
#include <QUrl>
#include "qgstreamerplayercontrol.h"
#include "qmediaplayer.h"

#include <gst/gst.h>

class QGstreamerBusHelper;
class QGstreamerMessage;

class QGstreamerVideoRendererInterface
{
public:
    virtual ~QGstreamerVideoRendererInterface();
    virtual GstElement *videoSink() = 0;
    virtual void precessNewStream() {}
};

#define QGstreamerVideoRendererInterface_iid "com.nokia.Qt.QGstreamerVideoRendererInterface/1.0"

Q_DECLARE_INTERFACE(QGstreamerVideoRendererInterface, QGstreamerVideoRendererInterface_iid)



class QGstreamerPlayerSession : public QObject
{
Q_OBJECT

public:
    QGstreamerPlayerSession(QObject *parent);
    virtual ~QGstreamerPlayerSession();

    QUrl url() const;

    QMediaPlayer::State state() const { return m_state; }

    qint64 duration() const;
    qint64 position() const;

    bool isBuffering() const;

    int bufferingProgress() const;

    int volume() const;
    bool isMuted() const;

    void setVideoRenderer(QObject *renderer);
    bool isVideoAvailable() const;

    bool isSeekable() const;

    QMap<QString,QVariant> tags() const { return m_tags; }
    QMap<QString,QVariant> streamProperties(int streamNumber) const { return m_streamProperties[streamNumber]; }
    int streamCount() const { return m_streamProperties.count(); }

public slots:
    void load(const QUrl &url);

    void play();
    void pause();
    void stop();

    void seek(qint64 pos);

    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayer::State state);
    void volumeChanged(int volume);
    void mutedStateChaned(bool muted);
    void videoAvailabilityChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);
    void bufferingProgressChanged(int percentFilled);
    void playbackFinished();
    void tagsChanged();
    void streamsChanged();
    void seekableChanged(bool);

private slots:
    void busMessage(const QGstreamerMessage &message);
    void getStreamsInfo();
    void setSeekable(bool);

private:
    QUrl m_url;
    QMediaPlayer::State m_state;
    QGstreamerBusHelper* m_busHelper;
    GstElement* m_playbin;
    GstBus* m_bus;
    QGstreamerVideoRendererInterface *m_renderer;

    QMap<QString,QVariant> m_tags;
    QList< QMap<QString,QVariant> > m_streamProperties;


    int m_volume;
    bool m_muted;
    bool m_videoAvailable;
    bool m_seekable;

    qint64 m_lastPosition;
    qint64 m_duration;
};

#endif // QGSTREAMERPLAYERSESSION_H
