#include "qmediaplaylistsource.h"
#include "qmediaplaylistsource_p.h"

/*!
    \class QMediaPlaylistSource
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

QMediaPlaylistSource::QMediaPlaylistSource(QObject *parent)
    :QObject(*new QMediaPlaylistSourcePrivate, parent)
{
}

QMediaPlaylistSource::QMediaPlaylistSource(QMediaPlaylistSourcePrivate &dd, QObject *parent)
    :QObject(dd,parent)
{
}

QMediaPlaylistSource::~QMediaPlaylistSource()
{
}

/*!
    \fn QMediaPlaylistSource::size() const;
*/

/*!
    \fn QMediaPlaylistSource::operator [](int pos) const;
*/

bool QMediaPlaylistSource::load(const QString &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

bool QMediaPlaylistSource::load(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

bool QMediaPlaylistSource::save(const QString &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

bool QMediaPlaylistSource::save(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

int QMediaPlaylistSource::currentItem()
{
    return d_func()->currentItem;
}

bool QMediaPlaylistSource::isReadOnly() const
{
    return true;
}

bool QMediaPlaylistSource::append(const QMediaSource &source)
{
    Q_UNUSED(source);
    return false;
}

bool QMediaPlaylistSource::append(const QList<QMediaSource> &sources)
{
    foreach(const QMediaSource& source, sources ) {
        if (!append(source))
            return false;
    }

    return true;
}

bool QMediaPlaylistSource::insert(int pos, const QMediaSource &source)
{
    Q_UNUSED(pos);
    Q_UNUSED(source);
    return false;
}

bool QMediaPlaylistSource::remove(int pos)
{
    Q_UNUSED(pos);
    return false;
}

bool QMediaPlaylistSource::remove(int start, int end)
{
    for (int pos=start; pos<=end; pos++) {
        if (!remove(pos))
            return false;
    }

    return true;
}

bool QMediaPlaylistSource::clear()
{
    return remove(0, size()-1);
}

QMediaPlaylist::PlaybackMode QMediaPlaylistSource::playbackMode()
{
    return d_func()->playbackMode;
}

void QMediaPlaylistSource::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    Q_D(QMediaPlaylistSource);

    if (d->playbackMode != mode) {
        d->playbackMode = mode;
        emit playbackModeChanged(mode);
    }
}


void QMediaPlaylistSource::setCurrentItem(int pos)
{
    Q_D(QMediaPlaylistSource);
    if (d->currentItem != pos) {
        d->currentItem = pos;
        emit currentItemChanged(pos);
    }
}

void QMediaPlaylistSource::shuffle()
{
}
