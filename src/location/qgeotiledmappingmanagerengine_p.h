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

#ifndef QGEOTILEDMAPPINGMANAGERENGINE_P_H
#define QGEOTILEDMAPPINGMANAGERENGINE_P_H

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

#include "qgeomappingmanagerengine_p.h"

#include <QSize>

QTM_BEGIN_NAMESPACE

class QGeoTiledMappingManagerThread;

class QGeoTiledMappingManagerEnginePrivate : public QGeoMappingManagerEnginePrivate
{
public:
    QGeoTiledMappingManagerEnginePrivate();
    QGeoTiledMappingManagerEnginePrivate(const QGeoTiledMappingManagerEnginePrivate &other);
    ~QGeoTiledMappingManagerEnginePrivate();

    QGeoTiledMappingManagerEnginePrivate& operator= (const QGeoTiledMappingManagerEnginePrivate &other);

    QList<QString> supportedImageFormats;
    QSize tileSize;
    QGeoTiledMappingManagerThread *thread;
};

/*

class QGeoTiledMapRequestHandler : public QObject
{
    Q_OBJECT
public:
    QGeoTiledMapRequestHandler(QGeoTiledMapNetworkThread *thread, QGeoTiledMapViewport *viewport);
    ~QGeoTiledMapRequestHandler();

public slots:
    void setRequests(const QList<QGeoTieldMapRequest> &requests);

private slots:
    void sendNextRequest();

    void replyFinished();
    void replyError(QGeoTiledMapReply::Error, const QString &errorString);

signals:
    void tileFinished(QGeoTieldMapReply *reply);
    void tileError(QGeoTiledMapReply *reply, QGeoTieldMapReply::Error error, QString errorString);

private:
    QGeoTiledMapNetworkThread *m_thread;
    QGeoTiledMapViewport *m_viewport;
    QList<QGeoTiledMapRequest> m_queue;
    QSet<QGeoTiledMapReply*> m_replies;
};

// TODO make public and subclassable - this is where the tile requests go
// and the tile manager engine class acts as a factory and provided capability information
class QGeoTiledMapNetworkThread : public QThread
{
    Q_OBJECT
public:
    QGeoTiledMapNetworkThread(QGeoTiledMappingManagerEngine *engine);
    ~QGeoTiledMapNetworkThread();

    QGeoTiledMappingManagerEngine *engine();
    QNetworkAccessManager* networkAccessManager() const;

    void removeViewport(QGeoTiledMapViewport* viewport);

protected:
    void run();

public slots:
    void setRequests(QGeoTiledMapViewport* viewport, const QList<QGeoMapTileRequest*> &requests);

signals:
    void tileFinished(QGeoMapTileResponse *response);
    void tileError(QGeoMapTileResponse *response, QGeoMapReply::Error error, QString errorString);

private:
    QGeoTiledMappingManagerEngine *m_engine;
    QNetworkAccessManager *m_networkManager;
    QNetworkDiskCache *m_diskCache;

    QHash<QGeoTiledMapViewport*, QGeoTiledMapRequestHandler*> m_handlers;
};
*/

QTM_END_NAMESPACE

#endif
