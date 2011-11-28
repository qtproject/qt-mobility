/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "s60mediarecognizer.h"
#include "s60videowidgetcontrol.h"
#include "s60videowindowcontrol.h"
#include "s60mediaplayeraudioendpointselector.h"
#include "s60medianetworkaccesscontrol.h"
#include "s60mediastreamcontrol.h"
#include "s60mmtrace.h"

#include <qmediaplaylistnavigator.h>
#include <qmediaplaylist.h>

/*!
    Construct a media service with the given \a parent.
*/

S60MediaPlayerService::S60MediaPlayerService(QObject *parent)
    : QMediaService(parent)
    , m_control(NULL)
    , m_metaData(NULL)
    , m_audioEndpointSelector(NULL)
    , m_streamControl(NULL)
    , m_networkAccessControl(NULL)
    , m_videoOutput(NULL)
{
    TRACE("S60MediaPlayerService::S60MediaPlayerService" << qtThisPtr());

    m_control = new S60MediaPlayerControl(this);
    m_metaData = new S60MediaMetaDataProvider(m_control, this);
    m_audioEndpointSelector = new S60MediaPlayerAudioEndpointSelector(m_control, this);
    m_streamControl = new S60MediaStreamControl(m_control, this);
    m_networkAccessControl =  new S60MediaNetworkAccessControl(this);
}

/*!
    Destroys a media service.
*/

S60MediaPlayerService::~S60MediaPlayerService()
{
    TRACE("S60MediaPlayerService::~S60MediaPlayerService" << qtThisPtr());
}

/*!
    \return a pointer to the media control, which matches the controller \a name.

    If the service does not implement the control, or if it is unavailable a
    null pointer is returned instead.

    Controls must be returned to the service when no longer needed using the
    releaseControl() function.
*/

QMediaControl *S60MediaPlayerService::requestControl(const char *name)
{
    QMediaControl *result = 0;

    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        result = m_control;

    if (qstrcmp(name, QMediaNetworkAccessControl_iid) == 0)
        result = m_networkAccessControl;

    if (qstrcmp(name, QMetaDataReaderControl_iid) == 0)
        result = m_metaData;

    if (qstrcmp(name, QAudioEndpointSelector_iid) == 0)
        result = m_audioEndpointSelector;

    if (qstrcmp(name, QMediaStreamsControl_iid) == 0)
        result = m_streamControl;

    if (!m_videoOutput) {
        if (qstrcmp(name, QVideoWidgetControl_iid) == 0) {
            m_videoOutput = new S60VideoWidgetControl(this);
        }
        else if (qstrcmp(name, QVideoWindowControl_iid) == 0) {
            m_videoOutput = new S60VideoWindowControl(this);
        }

        if (m_videoOutput) {
            m_control->setVideoOutput(m_videoOutput);
            result = m_videoOutput;
        }
    }else {
        if (qstrcmp(name, QVideoWidgetControl_iid) == 0 ||
            qstrcmp(name, QVideoWindowControl_iid) == 0){
            result = m_videoOutput;
        }
    }
    TRACE("S60MediaPlayerService::requestControl" << qtThisPtr()
          << "name" << name << "result" << result);
    return result;
}

/*!
    Releases a \a control back to the service.
*/

void S60MediaPlayerService::releaseControl(QMediaControl *control)
{
    TRACE("S60MediaPlayerService::releaseControl" << qtThisPtr()
          << "control" << control);

    if (control == m_videoOutput) {
        m_videoOutput = 0;
        m_control->setVideoOutput(m_videoOutput);
    }
}

/*!
 * \return media player session(audio playersesion/video playersession)
 *  by recognizing whether media is audio or video and sets it on media type.
*/
S60MediaPlayerSession* S60MediaPlayerService::createPlayerSession(const QMediaContent &content)
{
    QUrl url = content.canonicalUrl();
    S60MediaPlayerSession *session = 0;
    S60MediaSettings::TMediaType mediaType = S60MediaSettings::Unknown;
    if (!url.isEmpty()) {
        S60MediaRecognizer *recognizer = new S60MediaRecognizer;
        const S60MediaRecognizer::MediaType recognizerMediaType = recognizer->mediaType(url);
        delete recognizer;
        switch (recognizerMediaType) {
        case S60MediaRecognizer::Video:
        case S60MediaRecognizer::Url:
            mediaType = S60MediaSettings::Video;
            session = createVideoPlayerSession();
            break;
        case S60MediaRecognizer::Audio:
            mediaType = S60MediaSettings::Audio;
            session = createAudioPlayerSession();
            break;
        }
    }
    m_control->setMediaType(mediaType);
    return session;
}

/*!
 *  \return media playersession (videoplayersession).
 *  constructs the videoplayersession object and connects all the respective signals and slots.
 *  and initialises all the media settings.
*/

S60MediaPlayerSession* S60MediaPlayerService::createVideoPlayerSession()
{
    S60VideoPlayerSession *session = new S60VideoPlayerSession(this, m_networkAccessControl);

    connect(session, SIGNAL(positionChanged(qint64)),
            m_control, SIGNAL(positionChanged(qint64)));
    connect(session, SIGNAL(playbackRateChanged(qreal)),
            m_control, SIGNAL(playbackRateChanged(qreal)));
    connect(session, SIGNAL(volumeChanged(int)),
            m_control, SIGNAL(volumeChanged(int)));
    connect(session, SIGNAL(mutedChanged(bool)),
            m_control, SIGNAL(mutedChanged(bool)));
    connect(session, SIGNAL(durationChanged(qint64)),
            m_control, SIGNAL(durationChanged(qint64)));
    connect(session, SIGNAL(stateChanged(QMediaPlayer::State)),
            m_control, SIGNAL(stateChanged(QMediaPlayer::State)));
    connect(session, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            m_control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(session,SIGNAL(bufferStatusChanged(int)),
            m_control, SIGNAL(bufferStatusChanged(int)));
    connect(session, SIGNAL(videoAvailableChanged(bool)),
            m_control, SIGNAL(videoAvailableChanged(bool)));
    connect(session, SIGNAL(audioAvailableChanged(bool)),
            m_control, SIGNAL(audioAvailableChanged(bool)));
    connect(session, SIGNAL(seekableChanged(bool)),
            m_control, SIGNAL(seekableChanged(bool)));
    connect(session, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)),
            m_control, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)));
    connect(session, SIGNAL(error(int, const QString &)),
            m_control, SIGNAL(error(int, const QString &)));
    connect(session, SIGNAL(metaDataChanged()),
            m_metaData, SIGNAL(metaDataChanged()));
    connect(session, SIGNAL(activeEndpointChanged(const QString&)),
            m_audioEndpointSelector, SIGNAL(activeEndpointChanged(const QString&)));
    connect(session, SIGNAL(mediaChanged()),
            m_streamControl, SLOT(handleStreamsChanged()));
    connect(session, SIGNAL(accessPointChanged(int)),
            m_networkAccessControl, SLOT(accessPointChanged(int)));

    session->setVolume(m_control->mediaControlSettings().volume());
    session->setMuted(m_control->mediaControlSettings().isMuted());
    session->setAudioEndpoint(m_control->mediaControlSettings().audioEndpoint());
    session->setVideoRenderer(m_control->mediaControlSettings().videoOutput());

    return session;
}

/*!
 *  \return media playersession (audioplayersession).
 *  constructs the audioplayersession object and connects all the respective signals and slots.
 *  and initialises all the media settings.
*/

S60MediaPlayerSession* S60MediaPlayerService::createAudioPlayerSession()
{
    S60AudioPlayerSession *session = new S60AudioPlayerSession(this);

    connect(session, SIGNAL(positionChanged(qint64)),
            m_control, SIGNAL(positionChanged(qint64)));
    connect(session, SIGNAL(playbackRateChanged(qreal)),
            m_control, SIGNAL(playbackRateChanged(qreal)));
    connect(session, SIGNAL(volumeChanged(int)),
            m_control, SIGNAL(volumeChanged(int)));
    connect(session, SIGNAL(mutedChanged(bool)),
            m_control, SIGNAL(mutedChanged(bool)));
    connect(session, SIGNAL(durationChanged(qint64)),
            m_control, SIGNAL(durationChanged(qint64)));
    connect(session, SIGNAL(stateChanged(QMediaPlayer::State)),
            m_control, SIGNAL(stateChanged(QMediaPlayer::State)));
    connect(session, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            m_control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(session,SIGNAL(bufferStatusChanged(int)),
            m_control, SIGNAL(bufferStatusChanged(int)));
    connect(session, SIGNAL(videoAvailableChanged(bool)),
            m_control, SIGNAL(videoAvailableChanged(bool)));
    connect(session, SIGNAL(audioAvailableChanged(bool)),
            m_control, SIGNAL(audioAvailableChanged(bool)));
    connect(session, SIGNAL(seekableChanged(bool)),
            m_control, SIGNAL(seekableChanged(bool)));
    connect(session, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)),
            m_control, SIGNAL(availablePlaybackRangesChanged(const QMediaTimeRange&)));
    connect(session, SIGNAL(error(int, const QString &)),
            m_control, SIGNAL(error(int, const QString &)));
    connect(session, SIGNAL(metaDataChanged()),
            m_metaData, SIGNAL(metaDataChanged()));
    connect(session, SIGNAL(activeEndpointChanged(const QString&)),
            m_audioEndpointSelector, SIGNAL(activeEndpointChanged(const QString&)));
    connect(session, SIGNAL(mediaChanged()),
            m_streamControl, SLOT(handleStreamsChanged()));

    session->setVolume(m_control->mediaControlSettings().volume());
    session->setMuted(m_control->mediaControlSettings().isMuted());
    session->setAudioEndpoint(m_control->mediaControlSettings().audioEndpoint());

    return session;
}
