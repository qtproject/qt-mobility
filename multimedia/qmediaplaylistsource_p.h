#ifndef QMEDIAPLAYLISTSOURCE_P_H
#define QMEDIAPLAYLISTSOURCE_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmediaplaylist.h"

#include <QtCore/private/qobject_p.h>

class QMediaPlaylistSourcePrivate : public QObjectPrivate
{
public:
    QMediaPlaylistSourcePrivate()
        :currentItem(0), playbackMode(QMediaPlaylist::Linear)
    {}

    int currentItem;
    QMediaPlaylist::PlaybackMode playbackMode;
};


#endif // QMEDIAPLAYLISTSOURCE_P_H
