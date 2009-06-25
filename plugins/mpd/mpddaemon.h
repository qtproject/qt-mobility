
#ifndef MPDDAEMON_H
#define MPDDAEMON_H

#include <QtCore/qobject.h>


class MpdDaemonPrivate;
class MpdDaemon : public QObject
{
    Q_OBJECT

public:
    MpdDaemon(QObject *parent);
    ~MpdDaemon();

    bool connect();
    void disconnect();

    void play();
    void pause();
    void stop();

    int playerState() const;
    qint64 duration() const;
    int volume() const;
    bool muted() const;
    qint64 position() const;

signals:
    void disconnected();
    void notify();

private slots:
    void readData();
    void checkStatus();

private:
    MpdDaemonPrivate *d;

    void send(QByteArray const &cmd);
};


#endif  // MPDDAEMON_H
