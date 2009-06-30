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

#include <QtCore/qvariant.h>
#include <QtGui/qwidget.h>

#include "qgstreamerplayerservice.h"
#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayersession.h"

#include "qmediaplaylistnavigator.h"
#include "qmediaplaylist.h"

QGstreamerPlayerService::QGstreamerPlayerService(QObject *parent)
    : QMediaPlayerService(parent)    
    , m_control(0)    
    , m_navigator(0)
    , m_session(0)
{
    QMediaPlaylist *playlist = new QMediaPlaylist(0,this);
    m_navigator = new QMediaPlaylistNavigator(playlist,this);
    m_session = new QGstreamerPlayerSession(this);
    m_control = new QGstreamerPlayerControl(m_session, m_navigator, this);
    m_navigator->setPlaybackMode(QMediaPlaylistNavigator::Linear);
}

QGstreamerPlayerService::~QGstreamerPlayerService()
{
}

QMediaPlayerControl *QGstreamerPlayerService::control()
{
    return m_control;
}

QMediaMetaData *QGstreamerPlayerService::metaData()
{
    return 0;// m_metaData;
}

QMediaPlaylist *QGstreamerPlayerService::playlist()
{
    return m_navigator->playlist();
}

bool QGstreamerPlayerService::setPlaylist(QMediaPlaylist *playlist)
{
    m_navigator->setPlaylist(playlist);
    return true;
}

QWidget *QGstreamerPlayerService::createVideoWidget()
{
    return new QWidget;
}

QObject *QGstreamerPlayerService::videoOutput() const
{
    return m_session->videoOutput();
}

bool QGstreamerPlayerService::setVideoOutput(QObject *output)
{
    return m_session->setVideoOutput(output);
}

