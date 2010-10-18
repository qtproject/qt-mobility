/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGRAPHICSGEOMAP_H
#define QDECLARATIVEGRAPHICSGEOMAP_H

#include <qgraphicsgeomap.h>
#include <QtDeclarative/qdeclarativeitem.h>

QTM_BEGIN_NAMESPACE

class QGeoMapObject;
class QGeoMapData;
class QGeoServiceProvider;
class QDeclarativeCoordinate;
class QDeclarativeGeoServiceProvider;

class QDeclarativeGraphicsGeoMap : public QDeclarativeItem
{
    Q_OBJECT

    Q_ENUMS(MapType)
    Q_ENUMS(ConnectivityMode)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QSizeF size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel CONSTANT)
    Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel CONSTANT)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(MapType mapType READ mapType WRITE setMapType NOTIFY mapTypeChanged)
    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY declarativeCenterChanged)
    Q_PROPERTY(ConnectivityMode connectivityMode READ connectivityMode WRITE setConnectivityMode NOTIFY connectivityModeChanged)
    Q_PROPERTY(QDeclarativeListProperty<QGeoMapObject> objects READ objects)

    Q_CLASSINFO("DefaultProperty", "objects")

public:
    enum MapType {
        NoMap = QGraphicsGeoMap::NoMap,
        StreetMap = QGraphicsGeoMap::StreetMap,
        SatelliteMapDay = QGraphicsGeoMap::SatelliteMapDay,
        SatelliteMapNight = QGraphicsGeoMap::SatelliteMapNight,
        TerrainMap = QGraphicsGeoMap::TerrainMap
    };

    enum ConnectivityMode {
        NoConnectivity = QGraphicsGeoMap::NoConnectivity,
        OfflineMode = QGraphicsGeoMap::OfflineMode,
        OnlineMode = QGraphicsGeoMap::OnlineMode,
        HybridMode = QGraphicsGeoMap::HybridMode
    };

public:
    QDeclarativeGraphicsGeoMap(QDeclarativeItem *parent = 0);
    ~QDeclarativeGraphicsGeoMap();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    // TODO make these more QML like
    //QList<MapType> supportedMapTypes() const;
    //QList<ConnectivityMode> supportedConnectivityModes() const;

    void setSize(const QSizeF &size);
    QSizeF size() const;

    void setZoomLevel(qreal zoomLevel);
    qreal zoomLevel() const;

    void setCenter(const QDeclarativeCoordinate *center);
    QDeclarativeCoordinate* center();

    void setMapType(MapType mapType);
    MapType mapType() const;

    void setConnectivityMode(ConnectivityMode connectivityMode);
    ConnectivityMode connectivityMode() const;

    QDeclarativeListProperty<QGeoMapObject> objects();

    Q_INVOKABLE QDeclarativeCoordinate* toCoordinate(QPointF screenPosition) const;
    Q_INVOKABLE QPointF toScreenPosition(QDeclarativeCoordinate* coordinate) const;

public Q_SLOTS:
    void pan(int dx, int dy);

protected:
    void geometryChanged(const QRectF &newGeometry,
                         const QRectF &oldGeometry);

Q_SIGNALS:
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void sizeChanged(const QSizeF &size);
    void zoomLevelChanged(qreal zoomLevel);
    void declarativeCenterChanged(const QDeclarativeCoordinate *coordinate);
    void mapTypeChanged(QDeclarativeGraphicsGeoMap::MapType mapType);
    void connectivityModeChanged(QDeclarativeGraphicsGeoMap::ConnectivityMode connectivityMode);

private Q_SLOTS:
    void updateMapDisplay(const QRectF& target);
    void internalCenterChanged(const QGeoCoordinate &coordinate);
    void internalMapTypeChanged(QGraphicsGeoMap::MapType mapType);
    void internalConnectivityModeChanged(QGraphicsGeoMap::ConnectivityMode connectivityMode);
    void centerLatitudeChanged(double latitude);
    void centerLongitudeChanged(double longitude);
    void centerAltitudeChanged(double altitude);

private:
    static void object_append(QDeclarativeListProperty<QGeoMapObject> *prop, QGeoMapObject *mapObject);
    static int object_count(QDeclarativeListProperty<QGeoMapObject> *prop);
    static QGeoMapObject* object_at(QDeclarativeListProperty<QGeoMapObject> *prop, int index);
    static void object_clear(QDeclarativeListProperty<QGeoMapObject> *prop);

    QDeclarativeGeoServiceProvider* plugin_;
    QGeoServiceProvider* serviceProvider_;
    QGeoMappingManager* mappingManager_;
    QGeoMapData* mapData_;

    qreal zoomLevel_;
    QDeclarativeCoordinate* center_;
    QDeclarativeGraphicsGeoMap::MapType mapType_;
    QDeclarativeGraphicsGeoMap::ConnectivityMode connectivityMode_;
    QSizeF size_;
    QList<QGeoMapObject*> objects_;

    Q_DISABLE_COPY(QDeclarativeGraphicsGeoMap)
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGraphicsGeoMap));

#endif
