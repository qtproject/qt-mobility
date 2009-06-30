#include <QtCore/qstringlist.h>

#include "mpddaemon.h"
#include "mpdplaylistsource.h"
#include <qmediaplaylistsource_p.h>



class MpdPlaylistSourcePrivate : public QMediaPlaylistSourcePrivate
{
public:
    int entries;
    MpdDaemon* daemon;
};

MpdPlaylistSource::MpdPlaylistSource(MpdDaemon *daemon, QObject *parent):
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

bool MpdPlaylistSource::load(const QString &location, const char *format)
{
    return false;
}

bool MpdPlaylistSource::load(QIODevice * device, const char *format)
{
    return false;
}

bool MpdPlaylistSource::save(const QString &location, const char *format)
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
    bool    ok = false;
    d_func()->daemon->send(QString("delete %1").arg(pos), &ok);
    return ok;
}

bool MpdPlaylistSource::remove(int start, int end)
{
    for (int i = start; i < end; ++i)
        remove(i);

    return true;
}

bool MpdPlaylistSource::clear()
{
    bool    ok = false;
    d_func()->daemon->send("clear", &ok);
    return ok;
}

//public Q_SLOTS:
void MpdPlaylistSource::shuffle()
{
    d_func()->daemon->send("shuffe");
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
