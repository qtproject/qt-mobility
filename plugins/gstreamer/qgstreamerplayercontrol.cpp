#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayersession.h"

#include "qmediaplaylistnavigator.h"
#include "qmediasource.h"

#include <QtCore/qurl.h>

QGstreamerPlayerControl::QGstreamerPlayerControl(QGstreamerPlayerSession *session,QMediaPlaylistNavigator *navigator,QObject *parent)
   :QMediaPlayerControl(parent), m_session(session), m_navigator(navigator)
{
    connect(m_navigator, SIGNAL(activated(QMediaSource)),
            this, SLOT(play(QMediaSource)));
    connect(m_navigator, SIGNAL(currentPositionChanged(int)),
            this, SIGNAL(playlistPositionChanged(int)));

    connect(m_session, SIGNAL(playbackFinished()),
            m_navigator, SLOT(advance()));

    connect(m_session, SIGNAL(positionChanged(qint64)),
            this, SIGNAL(positionChanged(qint64)));
    connect(m_session, SIGNAL(stateChanged(QMediaPlayerControl::State)),
            this, SIGNAL(stateChanged(QMediaPlayerControl::State)));
    connect(m_session,SIGNAL(bufferingChanged(bool)),
            this, SIGNAL(bufferingChanged(bool)));
    connect(m_session,SIGNAL(bufferingProgressChanged(int)),
            this, SIGNAL(bufferingProgressChanged(int)));
}

QGstreamerPlayerControl::~QGstreamerPlayerControl()
{
}

int QGstreamerPlayerControl::playlistPosition() const
{
    return m_navigator->currentPosition();
}

QMediaSource QGstreamerPlayerControl::currentMediaSource() const
{
    return m_navigator->currentItem();
}

qint64 QGstreamerPlayerControl::position() const
{
    return m_session ? m_session->position() : 0;
}

int QGstreamerPlayerControl::bufferingProgress() const
{
    long progress = 0;
    return progress;
}

int QGstreamerPlayerControl::volume() const
{
    return m_session->volume()*100;
}

bool QGstreamerPlayerControl::isMuted() const
{
    return m_session->isMuted();
}

void QGstreamerPlayerControl::jump(int playlistPosition)
{
    m_navigator->jump(playlistPosition);
}

void QGstreamerPlayerControl::advance()
{
    m_navigator->advance();
}

void QGstreamerPlayerControl::back()
{
    m_navigator->back();
}

void QGstreamerPlayerControl::seek(qint64 pos)
{
    m_session->seek(pos);
}

void QGstreamerPlayerControl::play()
{
    m_session->play();
}

void QGstreamerPlayerControl::pause()
{
    m_session->pause();
}

void QGstreamerPlayerControl::stop()
{
    m_session->stop();
}

void QGstreamerPlayerControl::setVolume(int volume)
{
    m_session->setVolume(volume/100.0);
}

void QGstreamerPlayerControl::setMuted(bool muted)
{
    m_session->setMuted(muted);
}

void QGstreamerPlayerControl::play(const QMediaSource &src)
{
    QVariant location = src.dataLocation();
    QUrl url;

    if (location.convert(QVariant::Url)) {
        url = location.toUrl();
    } else if (location.convert(QVariant::String)) {
        url = QUrl(location.toString());
    }

    emit currentMediaChanged(src);

    if (url.isValid()) {
        m_session->stop();
        m_session->load(url);
        m_session->play();        
    } else {
        m_navigator->advance();
    }
}
