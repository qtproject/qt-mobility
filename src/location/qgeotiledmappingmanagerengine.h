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

#ifndef QGEOTILEDMAPPINGMANAGERENGINE_H
#define QGEOTILEDMAPPINGMANAGERENGINE_H

#include "qgeomappingmanagerengine.h"
#include "qgeotiledmapreply.h"

class QNetworkAccessManager;
class QNetworkProxy;

QTM_BEGIN_NAMESPACE

class QGeoTiledMapRequest;
class QGeoTiledMapViewport;

class QGeoTiledMappingManagerThread;
class QGeoTiledMappingManagerEnginePrivate;

class Q_LOCATION_EXPORT QGeoTiledMappingManagerEngine : public QGeoMappingManagerEngine
{
    Q_OBJECT
public:
    virtual ~QGeoTiledMappingManagerEngine();

    virtual QGeoMapViewport* createViewport(QGeoMapWidget *widget);

    virtual void updateMapImage(QGeoMapViewport *viewport);

    virtual QPoint screenPositionToTilePosition(const QGeoMapViewport *viewport, const QPointF &screenPosition) const;

    QList<QString> supportedImageFormats() const;
    QSize tileSize() const;

private slots:
    void init();
    void tileFinished(QGeoTiledMapReply *reply);
    void tileError(QGeoTiledMapReply *reply, QGeoTiledMapReply::Error error, QString errorString);

signals:
    void tileRequestsPrepared(QGeoTiledMapViewport* viewport, const QList<QGeoTiledMapRequest> &requests);

protected:
    QGeoTiledMappingManagerEngine(QObject *parent = 0);
    virtual QGeoTiledMappingManagerThread* createTileManagerThread() = 0;

    void setSupportedImageFormats(const QList<QString> &imageFormats);
    void setTileSize(const QSize &tileSize);

private:
    Q_DECLARE_PRIVATE(QGeoTiledMappingManagerEngine)
    Q_DISABLE_COPY(QGeoTiledMappingManagerEngine)
};

QTM_END_NAMESPACE

#endif
