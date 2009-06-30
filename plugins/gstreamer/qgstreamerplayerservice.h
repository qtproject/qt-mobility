#ifndef QGSTREAMERPLAYERSERVICE_H
#define QGSTREAMERPLAYERSERVICE_H

#include <QtCore/qobject.h>

class QMediaMetaData;
class QMediaPlayerControl;
class QMediaPlaylist;


class QMediaPlayerService : public QObject
{
    Q_OBJECT
public:
    QMediaPlayerService(QObject *parent = 0) : QObject(parent) {}

    virtual QWidget *createVideoWidget() = 0;

    virtual QObject *videoOutput() const = 0;
    virtual bool setVideoOutput(QObject *output) = 0;

    virtual QMediaPlayerControl *control() = 0;
    virtual QMediaMetaData *metaData() = 0;

    virtual QMediaPlaylist *playlist() = 0;
    virtual bool setPlaylist(QMediaPlaylist *) = 0;
};

class QGstreamerMetaData;
class QGstreamerPlayerControl;
class QGstreamerPlayerSession;

class QMediaPlaylistNavigator;

class QGstreamerPlayerService : public QMediaPlayerService
{
    Q_OBJECT
public:
    QGstreamerPlayerService(QObject *parent = 0);
    ~QGstreamerPlayerService();

    QMediaPlayerControl *control();
    QMediaMetaData *metaData();

    QWidget *createVideoWidget();

    QObject *videoOutput() const;
    bool setVideoOutput(QObject *output);

    QMediaPlaylist *playlist();
    bool setPlaylist(QMediaPlaylist *);

private:
    QGstreamerPlayerControl *m_control;    
    QMediaPlaylistNavigator *m_navigator;
    QGstreamerPlayerSession *m_session;    
};

#endif
