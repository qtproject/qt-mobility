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

#ifndef QGEOTILEDMAPPINGMANAGER_H
#define QGEOTILEDMAPPINGMANAGER_H

#include "qgeomappingmanager.h"

QTM_BEGIN_NAMESPACE

class QGeoTiledMappingManagerPrivate;

class Q_LOCATION_EXPORT QGeoTiledMappingManager : public QGeoMappingManager
{
    Q_OBJECT

public:
    enum ErrorCode {
        NoError
    };

    virtual ~QGeoTiledMappingManager();

    virtual QGeoMapViewport* createViewport(QGeoMapWidget *widget);

    virtual void updateMapImage(QGeoMapViewport *viewport);

    virtual QPoint screenPositionToTilePosition(const QGeoMapViewport *viewport, const QPointF &screenPosition) const;

    virtual QGeoMapReply* getTileImage(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex, QGeoMapWidget::MapType mapType, const QString &imageFormat) const = 0;

    QList<QString> supportedImageFormats() const;
    QSize tileSize() const;

    //    // TODO better way of returning these results
    //    virtual void coordinateToTileIndices(const QGeoCoordinate &coordinate, qint32 zoomLevel, qint32 *rowIndex, qint32 *columnIndex);
    //    virtual QGeoCoordinate tileIndicesToUpperLeftCoordinate(qint32 zoomLevel, qint32 rowIndex, qint32 columIndex);
    //    virtual QGeoBoundingBox getTileBounds(qint32 zoomLevel, qint32 rowIndex, qint32 columnIndex);



protected:
    QGeoTiledMappingManager();

    void setSupportedImageFormats(const QList<QString> &imageFormats);
    void setTileSize(const QSize &tileSize);

signals:
    void finished(QGeoMapReply* reply);
    void error(QGeoMapReply* reply, QGeoMapReply::Error error, QString errorString = QString());

private:

    Q_DECLARE_PRIVATE(QGeoTiledMappingManager)
    Q_DISABLE_COPY(QGeoTiledMappingManager)
};

QTM_END_NAMESPACE

#endif
