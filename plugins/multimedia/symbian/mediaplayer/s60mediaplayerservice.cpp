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

#ifndef QT_NO_MULTIMEDIA
#include "s60videooverlay.h"
#include "s60videorenderer.h"
#endif

#include <QMediaPlaylistNavigator>
#include <QMediaPlaylist>

S60MediaPlayerService::S60MediaPlayerService(QObject *parent)
    : QMediaService(parent),
      m_videoPlayerSession(NULL),
      m_audioPlayerSession(NULL),
      m_videoWindow(NULL),
      m_videoWidget(NULL),
      m_videoRenderer(NULL),
      m_control(NULL),
      m_videoOutput(NULL),
      m_metaData(NULL)
{
    m_control = new S60MediaPlayerControl(*this, this);
    m_mediaRecognizer = new S60MediaRecognizer(this);  
    m_metaData = new S60MediaMetaDataProvider(*this);
}

S60MediaPlayerService::~S60MediaPlayerService()
{
    delete m_videoWidget;
    delete m_videoRenderer;
    delete m_videoWindow;
    delete m_metaData;
}

QMediaControl *S60MediaPlayerService::control(const char *name) const
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_control;

    if (qstrcmp(name, QMetaDataControl_iid) == 0) {
        //if (!m_metaData) {
        //    m_metaData = new S60MediaMetaDataProvider(const_cast<S60MediaPlayerService&>(*this));
        //}
        return m_metaData;
    }

    if (qstrcmp(name, QVideoOutputControl_iid) == 0) {
        if (!m_videoOutput) {
            m_videoOutput = new S60VideoOutputControl;
            connect(m_videoOutput, SIGNAL(outputChanged(QVideoOutputControl::Output)),
                    this, SLOT(videoOutputChanged(QVideoOutputControl::Output)));
            m_videoOutput->setAvailableOutputs(QList<QVideoOutputControl::Output>() 
#ifndef QT_NO_MULTIMEDIA
                        << QVideoOutputControl::RendererOutput
                        << QVideoOutputControl::WindowOutput
#endif
                        << QVideoOutputControl::WidgetOutput);
            
        }
        return m_videoOutput;
    }

    if (qstrcmp(name, QVideoWidgetControl_iid) == 0) {
        if (!m_videoWidget)
            m_videoWidget = new S60VideoWidgetControl;
        return m_videoWidget;
    }
    
#ifndef QT_NO_MULTIMEDIA
    if (qstrcmp(name, QVideoRendererControl_iid) == 0) {
        if (m_videoRenderer)
            m_videoRenderer = new S60VideoRenderer;
        return m_videoRenderer;
    }
    
    if (qstrcmp(name, QVideoWindowControl_iid) == 0) {
        if (!m_videoWindow)
            m_videoWindow = new S60VideoOverlay;
        return m_videoWindow;
    }
#endif

    return 0;

}

void S60MediaPlayerService::videoOutputChanged(QVideoOutputControl::Output output)
{
    switch (output) {
    case QVideoOutputControl::NoOutput:
        m_control->setVideoOutput(0);
        break;

#ifndef QT_NO_MULTIMEDIA
    case QVideoOutputControl::RendererOutput:
        m_control->setVideoOutput(m_videoRenderer);
        break;
    case QVideoOutputControl::WindowOutput:
        m_control->setVideoOutput(m_videoWindow);
        break;
#endif

    case QVideoOutputControl::WidgetOutput:
        m_control->setVideoOutput(m_videoWidget);
        break;
    default:
        qWarning("Invalid video output selection");
        break;
    }
}

S60MediaPlayerSession* S60MediaPlayerService::PlayerSession()
{
    QUrl url = m_control->media().canonicalUri();
    S60MediaRecognizer::MediaType mediaType = m_mediaRecognizer->IdentifyMediaTypeL(url);
    
    if (mediaType == S60MediaRecognizer::Video) {
        return VideoPlayerSession();
    } else if (mediaType == S60MediaRecognizer::Audio) {
        return AudioPlayerSession();
    }
   
    return 0;
}

S60MediaPlayerSession* S60MediaPlayerService::VideoPlayerSession()
{
    if (!m_videoPlayerSession) {
        m_videoPlayerSession = new S60VideoPlayerSession(this);
        
        connect(m_videoPlayerSession, SIGNAL(positionChanged(qint64)),
                m_control, SIGNAL(positionChanged(qint64)));
        connect(m_videoPlayerSession, SIGNAL(durationChanged(qint64)),
                m_control, SIGNAL(durationChanged(qint64)));
        connect(m_videoPlayerSession, SIGNAL(mutedStateChaned(bool)),
                m_control, SIGNAL(mutingChanged(bool)));
        connect(m_videoPlayerSession, SIGNAL(volumeChanged(int)),
                m_control, SIGNAL(volumeChanged(int)));
        connect(m_videoPlayerSession, SIGNAL(stateChanged(QMediaPlayer::State)),
                m_control, SIGNAL(stateChanged(QMediaPlayer::State)));
        connect(m_videoPlayerSession, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                m_control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
        connect(m_videoPlayerSession,SIGNAL(bufferingProgressChanged(int)),
                m_control, SIGNAL(bufferStatusChanged(int)));
        connect(m_videoPlayerSession, SIGNAL(videoAvailabilityChanged(bool)),
                m_control, SIGNAL(videoAvailabilityChanged(bool)));
        connect(m_videoPlayerSession, SIGNAL(seekableChanged(bool)),
                m_control, SIGNAL(seekableChanged(bool)));
        connect(m_videoPlayerSession, SIGNAL(metaDataChanged()), 
                m_metaData, SIGNAL(metaDataChanged()));
    }
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
        connect(m_audioPlayerSession, SIGNAL(mutedStateChaned(bool)),
                m_control, SIGNAL(mutingChanged(bool)));
        connect(m_audioPlayerSession, SIGNAL(volumeChanged(int)),
                m_control, SIGNAL(volumeChanged(int)));
        connect(m_audioPlayerSession, SIGNAL(stateChanged(QMediaPlayer::State)),
                m_control, SIGNAL(stateChanged(QMediaPlayer::State)));
        connect(m_audioPlayerSession, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                m_control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
        connect(m_audioPlayerSession,SIGNAL(bufferingProgressChanged(int)),
                m_control, SIGNAL(bufferStatusChanged(int)));
        connect(m_audioPlayerSession, SIGNAL(videoAvailabilityChanged(bool)),
                m_control, SIGNAL(videoAvailabilityChanged(bool)));
        connect(m_audioPlayerSession, SIGNAL(seekableChanged(bool)),
                m_control, SIGNAL(seekableChanged(bool)));
        connect(m_audioPlayerSession, SIGNAL(metaDataChanged()), 
                m_metaData, SIGNAL(metaDataChanged()));
    }
    return m_audioPlayerSession;
}

