#include <private/qobject_p.h>

#include "qmediaplaylist.h"

/*!
    \class QMediaPlaylist
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaPlaylistPrivate : public QObjectPrivate
{
public:
};

QMediaPlaylist::QMediaPlaylist(QString name, QPlayListSource = defaultPlayListSource(), QObject* parent = 0):
    QObject(*new QMediaPlaylistPrivate, parent)
{
}

QMediaPlaylist::~QMediaPlaylist()
{
}

void QMediaPlaylist::enqueue(QMediaSource)
{
}

void QMediaPlaylist::remove(QMediaSource)
{
}

void QMediaPlaylist::clear()
{
}

//public Q_SLOTS:
void QMediaPlaylist::start()
{
}

void QMediaPlaylist::pause()
{
}

void QMediaPlaylist::stop()
{
}

void QMediaPlaylist::next()
{
}

void QMediaPlaylist::prev()
{
}

void QMediaPlaylist::jump(int)
{
}
