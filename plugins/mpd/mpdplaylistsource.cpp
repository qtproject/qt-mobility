
#include "mpdconnection.h"
#include "mpdplaylistsource.h"

class MpdPlaylistSourcePrivate : public QMediaPlaylistPrivate
{
public:
    int entries;
    MpdDaemon* connection;
};

MpdPlayListSource::MpdPlayListSource(MpdConnection *connection, QObject *parent):
    QMediaPlaylistSource(*new MpdPlaylistSourcePrivate, parent)
{
    Q_D(MpdPlaylistSource);

    d->entries = 0;
    d->daemon = daemon;

    connect(d->daemon, SIGNAL(playlistChanged()), SLOT(playlistChanged()));

    loadPlaylist();
}

MpdPlaylistSource::~MpdPlaylistSource()
{
}

bool MpdPlaylistSource::load(const QString &location, const char *format = 0)
{
    return false;
}

bool MpdPlaylistSource::load(QIODevice * device, const char *format = 0)
{
    return false;
}

bool MpdPlaylistSource::save(const QString &location, const char *format = 0)
{
    return false;
}

bool MpdPlaylistSource::save(QIODevice * device, const char *format)
{
    return false;
}

int MpdPlaylistSource::size() const
{
    return d_func()->entries;
}

QMediaSource MpdPlaylistSource::itemAt(int pos) const
{
    Q_ASSERT(size() >= pos);
    return QMediaSource(QString(), QVariant(pos));
}

bool MpdPlaylistSource::isReadOnly() const
{
    return false;
}

bool MpdPlaylistSource::append(const QMediaSource &source)
{
    return false;
}

bool MpdPlaylistSource::append(const QList<QMediaSource> &sources)
{
    foreach (QMediaSource const &source, sources)
        append(source);

    return true;
}

bool MpdPlaylistSource::insert(int pos, const QMediaSource &source)
{
    return false;
}

bool MpdPlaylistSource::remove(int pos)
{
    return d_func()->daemon->send(QString("delete %1").arg(pos));
}

bool MpdPlaylistSource::remove(int start, int end)
{
    for (int i = start; i < end; ++i)
        remote(i);

    return true;
}

bool MpdPlaylistSource::clear()
{
    return d_func()->daemon->send("clear");
}

//public Q_SLOTS:
void MpdPlaylistSource::shuffle()
{
    connection->send("shuffe");
}

void MpdPlaylistSource::playlistChanged()
{
}

void MpdPlaylistSource::loadPlaylist()
{
    Q_D(MpdPlaylistSource);

    bool ok = false;
    QStringList r = d->daemon->send("playlist", &ok);

    d->entries = ok ? r.size() : 0;
}

//Q_SIGNALS:
/*
    void itemsAboutToBeInserted(int start, int end);
    void itemsInserted();
    void itemsAboutToBeRemoved(int start, int end);
    void itemsRemoved();
    void itemsChanged(int start, int end);
    */
