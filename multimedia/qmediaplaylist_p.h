#ifndef QMEDIAPLAYLIST_P_H
#define QMEDIAPLAYLIST_P_H

#include <QtCore/private/qobject_p.h>
#include "qmediasource.h"

class QMediaPlaylistSource;
class QMediaPlaylistReader;
class QMediaPlaylistWritter;

class QMediaPlaylistPrivate : public QObjectPrivate
{
public:
    QMediaPlaylistSource *source;

    bool readItems(QMediaPlaylistReader *reader);
    bool writeItems(QMediaPlaylistWritter *writter);
};


#endif // QMEDIAPLAYLIST_P_H
