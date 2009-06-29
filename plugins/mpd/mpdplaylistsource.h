
#ifndef MPDPLAYLISTSOURCE_H
#define MPDPLAYLISTSOURCE_H


#include <QMediaPlaylistSource>

class MpdDaemon;

class MpdPlaylistSource : public QMediaPlaylistSource
{
    Q_OBJECT

public:
    MpdPlaylistSource(MpdDaemon *daemon, QObject *parent);
    ~MpdPlaylistSource();

    bool load(const QString &location, const char *format = 0);
    bool load(QIODevice *device, const char *format = 0);
    bool save(const QString &location, const char *format = 0);
    bool save(QIODevice *device, const char *format);

    int size() const;
    QMediaSource itemAt(int pos) const;

    bool isReadOnly() const;

    bool append(const QMediaSource &source);
    bool append(const QList<QMediaSource> &sources);
    bool insert(int pos, const QMediaSource &source);
    bool remove(int pos);
    bool remove(int start, int end);
    bool clear();

    void shuffle();

private slots:
    void playlistChanged();

private:
    void loadPlaylist();

    Q_DECLARE_PRIVATE(MpdPlaylistSource);
};
#endif  // MPDPLAYLISTSOURCE_H
