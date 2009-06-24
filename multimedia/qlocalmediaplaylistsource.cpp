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
    emit itemsAboutToBeInserted(pos, pos);
    d->sources.append(source);
    emit itemsInserted();
    return true;
}

bool QLocalMediaPlaylistSource::append(const QList<QMediaSource> &sources)
{
    Q_D(QLocalMediaPlaylistSource);

    int pos = d->sources.size();

    emit itemsAboutToBeInserted(pos, pos+sources.size()-1);
    d->sources.append(sources);
    emit itemsInserted();

    return true;
}

bool QLocalMediaPlaylistSource::insert(int pos, const QMediaSource &source)
{
    Q_D(QLocalMediaPlaylistSource);

    emit itemsAboutToBeInserted(pos, pos);
    d->sources.insert(pos, source);
    emit itemsInserted();

    return true;
}

bool QLocalMediaPlaylistSource::remove(int fromPos, int toPos)
{
    Q_D(QLocalMediaPlaylistSource);

    Q_ASSERT(fromPos > 0);
    Q_ASSERT(fromPos <= toPos);
    Q_ASSERT(toPos < size());

    emit itemsAboutToBeRemoved(fromPos, toPos);
    d->sources.erase(d->sources.begin()+fromPos, d->sources.begin()+toPos);
    emit itemsRemoved();

    return true;
}

bool QLocalMediaPlaylistSource::remove(int pos)
{
    Q_D(QLocalMediaPlaylistSource);

    emit itemsAboutToBeRemoved(pos, pos);
    d->sources.removeAt(pos);
    emit itemsRemoved();

    return true;
}

bool QLocalMediaPlaylistSource::clear()
{
    Q_D(QLocalMediaPlaylistSource);
    if (!d->sources.isEmpty()) {        
        emit itemsAboutToBeRemoved(0, size()-1);
        d->sources.clear();
        emit itemsRemoved();
    }

    return true;
}

void QLocalMediaPlaylistSource::shuffle()
{
    Q_D(QLocalMediaPlaylistSource);
    if (!d->sources.isEmpty()) {
        QList<QMediaSource> sources;

        while (!d->sources.isEmpty()) {
            sources.append(d->sources.takeAt(qrand() % d->sources.size()));
        }

        d->sources = sources;
        emit itemsChanged(0,size()-1);
    }

}
