#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

#include <QObject>


class QMediaPlaylistPrivate;

class QMediaPlaylist : public QObject
{
    Q_OBJECT

public:
    QMediaPlaylist(QString name, QPlayListSource = defaultPlayListSource(), QObject* parent = 0);
    ~QMediaPlaylist();

    void enqueue(QMediaSource);
    void remove(QMediaSource);

    void clear();

public Q_SLOTS:
    void start();
    void pause();
    void stop();

    void next();
    void prev();

    void jump(int);

Q_SIGNALS:
    void current(MediaSource);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylist)
};

#endif  // QMEDIAPLAYLIST_H
