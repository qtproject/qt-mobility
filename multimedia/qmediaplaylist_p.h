/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIAPLAYLIST_P_H
#define QMEDIAPLAYLIST_P_H

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

#include <QtCore/private/qobject_p.h>
#include "qmediasource.h"
#include "qmediaplaylist.h"

class QMediaPlaylistSource;
class QMediaPlaylistReader;
class QMediaPlaylistWritter;

class QMediaPlaylistPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaPlaylist)
public:
    QMediaPlaylistPrivate()
        : source(0)
        , startPendingInsert(-1)
        , endPendingInsert(-1)
        , startPendingRemove(-1)
        , endPendingRemove(-1)
    {
    }

    QMediaPlaylistSource *source;

    int startPendingInsert;
    int endPendingInsert;
    int startPendingRemove;
    int endPendingRemove;

    bool readItems(QMediaPlaylistReader *reader);
    bool writeItems(QMediaPlaylistWritter *writter);

    void _q_itemsAboutToBeInserted(int start, int end);
    void _q_itemsInserted();
    void _q_itemsAboutToBeRemoved(int start, int end);
    void _q_itemsRemoved();

};


#endif // QMEDIAPLAYLIST_P_H
