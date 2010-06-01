/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOPLAYERSERVICE_H
#define S60VIDEOPLAYERSERVICE_H

#include <QtCore/qobject.h>

#include <qmediaservice.h>
//FIXME #include <qvideooutputcontrol.h>

//FIXME #include "s60videooutputcontrol.h"
#include "ms60mediaplayerresolver.h"

#include "s60mediaplayeraudioendpointselector.h"

QT_BEGIN_NAMESPACE
class QMediaMetaData;
class QMediaPlayerControl;
class QMediaPlaylist;
QT_END_NAMESPACE

QT_USE_NAMESPACE

class S60VideoPlayerSession;
class S60AudioPlayerSession;
class S60MediaPlayerControl;
class S60MediaMetaDataProvider;
class S60VideoWidgetControl;
class S60MediaRecognizer;
class S60VideoRenderer;
class S60VideoOverlay;

class QMediaPlaylistNavigator;

class S60MediaPlayerService : public QMediaService, public MS60MediaPlayerResolver
{
    Q_OBJECT
    
public:   
    S60MediaPlayerService(QObject *parent = 0);
    ~S60MediaPlayerService();

    QMediaControl *requestControl(const char *name);
    void releaseControl(QMediaControl *control);

//FIXME  private slots:
//    void videoOutputChanged(QVideoOutputControl::Output output);

protected: // From MS60MediaPlayerResolver
    S60MediaPlayerSession* PlayerSession();
    S60MediaPlayerSession* VideoPlayerSession();
    S60MediaPlayerSession* AudioPlayerSession();
    
private:
    S60MediaPlayerControl *m_control;
//FIXME    mutable S60VideoOutputControl *m_videoOutput;
    S60VideoPlayerSession *m_videoPlayerSession;
    S60AudioPlayerSession *m_audioPlayerSession;
    mutable S60MediaMetaDataProvider *m_metaData;
    mutable S60VideoWidgetControl *m_videoWidget;
    mutable S60VideoOverlay *m_videoWindow;
    mutable S60VideoRenderer *m_videoRenderer;
    S60MediaPlayerAudioEndpointSelector *m_audioEndpointSelector;
};

#endif
