/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOTILEDMAPPINGMANAGER_P_H
#define QGEOTILEDMAPPINGMANAGER_P_H

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

#include "qgeomappingmanager_p.h"

#include "qgeomapreply.h"

#include <QSize>
#include <QList>
#include <QSet>

QTM_BEGIN_NAMESPACE

class QGeoMapViewport;
class QGeoMapTileReplyContainer;

class QGeoTiledMappingManagerPrivate : public QGeoMappingManagerPrivate
{
public:
    QGeoTiledMappingManagerPrivate();
    QGeoTiledMappingManagerPrivate(const QGeoTiledMappingManagerPrivate &other);
    ~QGeoTiledMappingManagerPrivate();

    QGeoTiledMappingManagerPrivate& operator= (const QGeoTiledMappingManagerPrivate &other);

    QList<QString> supportedImageFormats;
    QSize tileSize;
    QHash<QGeoMapViewport*, QGeoMapTileReplyContainer*> replies;
};

class QGeoMapTileReply {
public:
    QGeoMapTileReply(QGeoMapReply *reply, const QRectF &source, const QRectF &dest);
    ~QGeoMapTileReply();

    QGeoMapReply *m_reply;
    QRectF m_source;
    QRectF m_dest;
};

class QGeoMapTileReplyContainer : public QObject
{
    Q_OBJECT
public:
    QGeoMapTileReplyContainer(QGeoMapViewport *viewport);
    ~QGeoMapTileReplyContainer();

    void cancelAllReplies();
    void addReply(QGeoMapTileReply *reply);

private slots:
    void replyFinished();
    void replyError(QGeoMapReply::Error, const QString &errorString);

private:
    QGeoMapViewport *m_viewport;
    QHash<QGeoMapReply*, QGeoMapTileReply*> m_replies;
};

QTM_END_NAMESPACE

#endif
