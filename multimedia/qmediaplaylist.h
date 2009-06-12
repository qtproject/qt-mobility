#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

class QMediaPlaylist
{
public:
    QMediaPlaylist(QString name, QPlayListSource = defaultPlayListSource());

    void enqueue(QMediaSource);
    void remove(QMediaSource);

    clear();

public Q_SLOTS:
    void start();
    void pause();
    void stop();

    void next();
    void prev();

    void jump(int);

Q_SIGNALS:
    void current(MediaSource);
};

#endif  // QMEDIAPLAYLIST_H
