#include "qlocalmediaplaylistsource.h"
#include "qmediaplaylistsource_p.h"

class QLocalMediaPlaylistSourcePrivate: public QMediaPlaylistSourcePrivate
{
public:
    QList<QMediaSource> sources;
};

QLocalMediaPlaylistSource::QLocalMediaPlaylistSource(QObject *parent)
    :QMediaPlaylistSource(*new QLocalMediaPlaylistSourcePrivate, parent)
{
}

QLocalMediaPlaylistSource::~QLocalMediaPlaylistSource()
{
}

bool QLocalMediaPlaylistSource::isReadOnly() const
{
    return false;
}

int QLocalMediaPlaylistSource::size() const
{
    return d_func()->sources.size();
}

QMediaSource QLocalMediaPlaylistSource::itemAt(int pos) const
{
    return d_func()->sources[pos];
}

bool QLocalMediaPlaylistSource::append(const QMediaSource &source)
{
    Q_D(QLocalMediaPlaylistSource);
    int pos = d->sources.size();
    d->sources.append(source);
    emit itemsInserted(pos, pos);
    return true;
}

bool QLocalMediaPlaylistSource::append(const QList<QMediaSource> &sources)
{
    Q_D(QLocalMediaPlaylistSource);
    int pos = d->sources.size();
    d->sources.append(sources);
    emit itemsInserted(pos, pos+sources.size()-1);
    return true;
}

bool QLocalMediaPlaylistSource::insert(int pos, const QMediaSource &source)
{
    Q_D(QLocalMediaPlaylistSource);
    d->sources.insert(pos, source);

    emit itemsInserted(pos, pos);

    if (pos <= d->currentItem) {
        d->currentItem++;
        emit currentItemChanged(d->currentItem);
    }

    return true;
}

bool QLocalMediaPlaylistSource::remove(int fromPos, int toPos)
{
    Q_D(QLocalMediaPlaylistSource);

    //keep the current item at the same source, if possible
    bool currentChanged = false;
    if (d->currentItem > toPos ) {
        d->currentItem -= (toPos-fromPos+1);
    } else if (d->currentItem >= fromPos && d->currentItem <= toPos) {
        currentChanged = true;
        d->currentItem = fromPos+1;
    }
    d->sources.erase(d->sources.begin()+fromPos, d->sources.begin()+toPos);
    emit itemsRemoved(fromPos, toPos);

    if (currentChanged) {
        if (d->currentItem >= size())
            d->currentItem = -1;

        emit currentItemChanged(currentItem());
    }

    return true;
}

bool QLocalMediaPlaylistSource::clear()
{
    Q_D(QLocalMediaPlaylistSource);
    if (!d->sources.isEmpty()) {
        int oldSize = d->sources.size();
        d->sources.clear();
        d->currentItem = -1;
        emit itemsRemoved(0, oldSize-1);
        emit currentItemChanged(-1);
    }

    return true;
}
