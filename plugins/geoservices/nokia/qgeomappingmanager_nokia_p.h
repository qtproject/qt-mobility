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

#ifndef QGEOMAPPINGMANAGER_NOKIA_P_H
#define QGEOMAPPINGMANAGER_NOKIA_P_H

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

#include <QGeoServiceProvider>
#include <QGeoMappingManager>
#include <QGeoMapRequestOptions>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>
#include <QPair>
#include <QRectF>

QTM_USE_NAMESPACE

class QGeoQuadTileCacheNokia;

class QGeoMappingManagerNokia : public QGeoMappingManager
{
    Q_OBJECT

public:
    struct QuadTileInfo
    {
        int row;
        int col;
        int zoomLevel;
        QSize size;
        QGeoMapRequestOptions options;
    };

public:
    QGeoMappingManagerNokia(const QMap<QString, QString> &parameters,
                            QGeoServiceProvider::Error *error,
                            QString *errorString);
    virtual ~QGeoMappingManagerNokia();

    virtual QGeoMapReply* requestMap(const QGeoCoordinate &center,
                                     int zoomLevel,
                                     const QSize &size,
                                     const QGeoMapRequestOptions &requestOptions);

    virtual QGeoMapReply* requestTile(int row, int col, int zoomLevel,
                                      const QSize &size,
                                      const QGeoMapRequestOptions &requestOptions);

    virtual QGeoMapReply* requestTile(const QGeoCoordinate &onTile, int zoomLevel,
                                      const QSize &size,
                                      const QGeoMapRequestOptions &requestOptions);

    //internal map state
    virtual void setZoomLevel(int zoomLevel);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option);
    virtual void setCenter(const QGeoCoordinate &center);
    virtual QGeoCoordinate center() const;
    virtual void pan(int startX, int startY, int endX, int endY);
    virtual QGeoBoundingBox viewBounds() const;
    virtual QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const;
    virtual QGeoCoordinate screenPositionToCoordinate(QPointF screenPosition) const;

private:
    Q_DISABLE_COPY(QGeoMappingManagerNokia)
    Q_DECLARE_PRIVATE(QGeoMappingManager)

    QString getRequestString(const QuadTileInfo &info) const;

private slots:
    void mapFinished();
    void mapError(QGeoMapReply::Error error, const QString &errorString);

private:
    static QString sizeToStr(const QSize &size);
    static QString mapTypeToStr(MapType type);
    static void getMercatorTileIndex(const QGeoCoordinate& coordinate, qint32 level, qint32* row, qint32* col);
    static qint64 getTileIndex(qint32 row, qint32 col, qint32 zoomLevel);

private:
    QNetworkAccessManager *m_nam;
    QString m_host;
    //QGeoQuadTileCacheNokia* m_cache;
    QHash<QGeoMapReply*, QuadTileInfo*> m_pendingReplies;

    QString m_token;
    QString m_referrer;

    //internal map state

    QHash<qint64, QPair<QPixmap, bool> > m_mapTiles;
    QRectF m_viewPort;
    QSize m_tileSize;
};

#endif
