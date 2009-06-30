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

