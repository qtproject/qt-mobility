
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

    int playerState() const;
    qint64 duration() const;
    int volume() const;
    bool muted() const;
    qint64 position() const;

    QStringList send(QString const &command, bool *ok = 0);

signals:
    void disconnected();
    void notify();
    void playlistChanged();
    void playerChanged();
    void mixerChanged();

private slots:
    void readData();
    void checkStatus();

private:
    MpdDaemonPrivate *d;

    QStringList rawSend(QByteArray const &cmd, bool *ok = 0);
    QStringList rawRecv(bool *ok = 0);
};


#endif  // MPDDAEMON_H
