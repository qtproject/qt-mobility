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

#ifndef MAPSWIDGET_H_
#define MAPSWIDGET_H_

#include "markerlist.h"

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QTimer>

#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

#include <qgraphicsgeomap.h>
#include <qgeomappingmanager.h>
#include <qgeomappixmapobject.h>
#include <qgeocoordinate.h>

class GeoMap;

QTM_USE_NAMESPACE

class MapsWidget: public QWidget
{
    Q_OBJECT

public:
    MapsWidget(QWidget *parent = 0);
    ~MapsWidget();

    MarkerObject* getMyLocation();
    bool getWaypointSelect();

public slots:
    void initialize(QGeoMappingManager *mappingManager);
    void centerOnMyLocation();
    void centerOnMarker(MarkerObject *marker);
    void createMarker(MarkerObject *marker);
    void deleteMarker(MarkerObject *marker);
    void moveMarker();

    void setMarker();
    MarkerObject* setMarkerCoordinate(const QGeoCoordinate &coord);
    MarkerObject* setMarkerAddress(const QString &address);
    void setMarkerLocation(const QGeoCoordinate &coord);
    void setMouseMarker(QGeoMapObject *marker);

    void setMapType(QString type);

    void beginWaypointSelect(MarkerObject *marker);

signals:
    void addMarker(MarkerObject *marker);
    void removeMarker(MarkerObject *marker);
    void moveMarker(const QGeoCoordinate &last, const QGeoCoordinate &curr);
    void markerPressed(MarkerObject *marker);
    void markerReleased();
    void mapMoved();
    void waypointSelected();

private:
    bool waypointSelect;

    QGeoMappingManager *m_mappingManager;

    QGraphicsView *view;
    GeoMap *geomap;

    QGeoCoordinate myLocation;
    QGeoCoordinate markerLocation;

    MarkerObject *m_marker;
    MarkerObject *myLocationMarker;

    friend class GeoMap;
};

#endif /* MAPSWIDGET_H_ */
