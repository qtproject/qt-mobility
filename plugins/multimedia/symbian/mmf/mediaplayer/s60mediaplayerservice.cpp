/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>

#include "s60mediaplayerservice.h"
#include "s60mediaplayercontrol.h"
#include "s60videoplayersession.h"
#include "s60audioplayersession.h"
#include "s60mediametadataprovider.h"
#include "s60videowidget.h"
#include "s60mediarecognizer.h"
#include "s60videooverlay.h"
#ifdef HAS_VIDEORENDERERCONTROL_IN_VIDEOPLAYER
#include "s60videorenderer.h"
#endif
#include "s60mediaplayeraudioendpointselector.h"
#include "s60mediastreamcontrol.h"

#include <qmediaplaylistnavigator.h>
#include <qmediaplaylist.h>

S60MediaPlayerService::S60MediaPlayerService(QObject *parent)
    : QMediaService(parent)
    , m_control(NULL)
    , m_videoPlayerSession(NULL)
    , m_audioPlayerSession(NULL)
    , m_metaData(NULL)
    , m_audioEndpointSelector(NULL)
    , m_streamControl(NULL)
    , m_videoOutput(NULL)
{
    m_control = new S60MediaPlayerControl(*this, this);
    m_metaData = new S60MediaMetaDataProvider(m_control, this);
    m_audioEndpointSelector = new S60MediaPlayerAudioEndpointSelector(m_control, this);
    m_streamControl = new S60MediaStreamControl(m_control, this);
}

S60MediaPlayerService::~S60MediaPlayerService()
{
}

QMediaControl *S60MediaPlayerService::requestControl(const char *name)
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_control;

    if (qstrcmp(name, QMetaDataReaderControl_iid) == 0)
        return m_metaData;

    if (qstrcmp(name, QAudioEndpointSelector_iid) == 0)
        return m_audioEndpointSelector;

    if (qstrcmp(name, QMediaStreamsControl_iid) == 0)
        return m_streamControl;

    if (!m_videoOutput) {
        if (qstrcmp(name, QVideoWidgetControl_iid) == 0) {
            m_videoOutput = new S60VideoWidgetControl(this);
        }
#ifdef HAS_VIDEORENDERERCONTROL_IN_VIDEOPLAYER
        else if (qstrcmp(name, QVideoRendererControl_iid) == 0) {
            m_videoOutput = new S60VideoRenderer(this);
        }
#endif /* HAS_VIDEORENDERERCONTROL_IN_VIDEOPLAYER */
        else if (qstrcmp(name, QVideoWindowControl_iid) == 0) {
            m_videoOutput = new S60VideoOverlay(this);
        }

        if (m_videoOutput) {
            m_control->setVideoOutput(m_videoOutput);
            return m_videoOutput;
        }
    }else {
        if (qstrcmp(name, QVideoWidgetControl_iid) == 0 ||
#ifdef HAS_VIDEORENDERERCONTROL_IN_VIDEOPLAYER
            qstrcmp(name, QVideoRendererControl_iid) == 0 ||
#endif /* HAS_VIDEORENDERERCONTROL_IN_VIDEOPLAYER */
            qstrcmp(name, QVideoWindowControl_iid) == 0){
            return m_videoOutput;
        }
    }
    return 0;
}

void S60MediaPlayerService::releaseControl(QMediaControl *control)
{
    if (control == m_videoOutput) {
        m_videoOutput = 0;
        m_control->setVideoOutput(m_videoOutput);
    }
}

S60MediaPlayerSession* S60MediaPlayerService::PlayerSession()
{
    QUrl url = m_control->media().canonicalUrl();

    if (url.isEmpty() == true) {
        return NULL;
    }

    QScopedPointer<S60MediaRecognizer> mediaRecognizer(new S60MediaRecognizer);
    S60MediaRecognizer::MediaType mediaType = mediaRecognizer->mediaType(url);
    mediaRecognizer.reset();

    switch (mediaType) {
    case S60MediaRecognizer::Video:
    case S60MediaRecognizer::Url:
        {
        m_control->setMediaType(S60MediaSettings::Video);
        return VideoPlayerSession();
        }
    case S60MediaRecognizer::Audio:
        {
        m_control->setMediaType(S60MediaSettings::Audio);
        return AudioPlayerSession();
        }
    default:
        m_control->setMediaType(S60MediaSettings::Unknown);
        break;
    }

    return NULL;
}

S60MediaPlayerSession* S60MediaPlayerService::VideoPlayerSession()
{
    if (!m_videoPlayerSession) {
        m_videoPlayerSession = new S60VideoPlayerSession(this);

        connect(m_videoPlayerSession, SIGNAL(positionChanged(qint64)),
                m_control, SIGNAL(positionChanged(qint64)));
        connect(m_videoPlayerSession, SIGNAL(durationChanged(qint64)),
                m_control, SIGNAL(durationChanged(qint64)));
        connect(m_videoPlayerSession, SIGNAL(stateChanged(QMediaPlayer::State)),
                m_control, SIGNAL(stateChanged(QMediaPlayer::State)));
        connect(m_videoPlayerSession, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                m_control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
        connect(m_videoPlayerSession,SIGNAL(bufferStatusChanged(int)),
                m_control, SIGNAL(bufferStatusChanged(int)));
        connect(m_videoPlayerSession, SIGNAL(videoAvailableChanged(bool)),
                m_control, SIGNAL(videoAvailableChanged(bool)));
        connect(m_videoPlayerSession, SIGNAL(audioAvailableChanged(bool)),
                m_control, SIGNAL(audioAvailableChanged(bool)));
        connect(m_videoPlayerSession, SIGNAL(seekableChanged(bool)),
                m_control, SIGNAL(seekableChanged(bool)));
        connect(m_videoPlayerSession, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)),
                m_control, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)));
        connect(m_videoPlayerSession, SIGNAL(error(int, const QString &)),
                m_control, SIGNAL(error(int, const QString &)));
        connect(m_videoPlayerSession, SIGNAL(metaDataChanged()),
                m_metaData, SIGNAL(metaDataChanged()));
        connect(m_videoPlayerSession, SIGNAL(activeEndpointChanged(const QString&)),
                m_audioEndpointSelector, SIGNAL(activeEndpointChanged(const QString&)));
        connect(m_videoPlayerSession, SIGNAL(mediaChanged()),
                m_streamControl, SLOT(handleStreamsChanged()));
    }

    m_videoPlayerSession->setVolume(m_control->mediaControlSettings().volume());
    m_videoPlayerSession->setMuted(m_control->mediaControlSettings().isMuted());
    m_videoPlayerSession->setAudioEndpoint(m_control->mediaControlSettings().audioEndpoint());
    return m_videoPlayerSession;
}

S60MediaPlayerSession* S60MediaPlayerService::AudioPlayerSession()
{
    if (!m_audioPlayerSession) {
        m_audioPlayerSession = new S60AudioPlayerSession(this);

        connect(m_audioPlayerSession, SIGNAL(positionChanged(qint64)),
                m_control, SIGNAL(positionChanged(qint64)));
        connect(m_audioPlayerSession, SIGNAL(durationChanged(qint64)),
                m_control, SIGNAL(durationChanged(qint64)));
        connect(m_audioPlayerSession, SIGNAL(stateChanged(QMediaPlayer::State)),
                m_control, SIGNAL(stateChanged(QMediaPlayer::State)));
        connect(m_audioPlayerSession, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                m_control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
        connect(m_audioPlayerSession,SIGNAL(bufferStatusChanged(int)),
                m_control, SIGNAL(bufferStatusChanged(int)));
        connect(m_audioPlayerSession, SIGNAL(videoAvailableChanged(bool)),
                m_control, SIGNAL(videoAvailableChanged(bool)));
        connect(m_audioPlayerSession, SIGNAL(audioAvailableChanged(bool)),
                m_control, SIGNAL(audioAvailableChanged(bool)));
        connect(m_audioPlayerSession, SIGNAL(seekableChanged(bool)),
                m_control, SIGNAL(seekableChanged(bool)));
        connect(m_audioPlayerSession, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)),
                m_control, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)));
        connect(m_audioPlayerSession, SIGNAL(error(int, const QString &)),
                m_control, SIGNAL(error(int, const QString &)));
        connect(m_audioPlayerSession, SIGNAL(metaDataChanged()),
                m_metaData, SIGNAL(metaDataChanged()));
        connect(m_audioPlayerSession, SIGNAL(activeEndpointChanged(const QString&)),
                m_audioEndpointSelector, SIGNAL(activeEndpointChanged(const QString&)));
        connect(m_audioPlayerSession, SIGNAL(mediaChanged()),
                m_streamControl, SLOT(handleStreamsChanged()));
    }

    m_audioPlayerSession->setVolume(m_control->mediaControlSettings().volume());
    m_audioPlayerSession->setMuted(m_control->mediaControlSettings().isMuted());
    m_audioPlayerSession->setAudioEndpoint(m_control->mediaControlSettings().audioEndpoint());
    return m_audioPlayerSession;
}
