/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAPBOX_H
#define MAPBOX_H

#include <qmobilityglobal.h>

#include <QWidget>
#include <QMap>

class GeoMap;

class QGraphicsView;
class QGraphicsScene;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPushButton;
class QLineEdit;

QTM_BEGIN_NAMESPACE
    class QGeoCoordinate;
    class QGeoServiceProvider;
    class QGeoMappingManager;
    class QGeoMapRectangleObject;
    class QGeoMapCircleObject;
    class QGeoMapPolylineObject;
    class QGeoMapPolygonObject;
    class QGeoMapRouteObject;
    class QGeoMapObject;
    class QGeoMapPixmapObject;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class MapBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode);

public:
    explicit MapBox(QWidget * parent = 0);
    ~MapBox();

    GeoMap * map() { return m_mapWidget; }
    void setProvider(const QString & providerId);

    QGeoMapRectangleObject * addRectangle(qreal top, qreal left, qreal bottom, qreal right);
    QGeoMapRectangleObject * addRectangle(const QGeoCoordinate & topLeft, const QGeoCoordinate & bottomRight);
    QGeoMapPolylineObject * addPolyline(const QList<QGeoCoordinate> & path);
    QGeoMapPolygonObject * addPolygon(const QList<QGeoCoordinate> & path);
    QGeoMapCircleObject * addCircle(const QGeoCoordinate & center, qreal radius);
    void addRoute(const QGeoCoordinate & start, const QGeoCoordinate & end);
    void addRoute(const QList<QGeoCoordinate> & waypoints);

    enum DisplayMode {
        DisplayNone,
        DisplayMap,
        DisplayOptions
    };

    void setDisplayMode(DisplayMode mode);
    DisplayMode displayMode();

private:
    QGraphicsView * m_qgv;
    QGraphicsScene * m_scene;
    GeoMap * m_mapWidget;

    QGeoServiceProvider * m_serviceProvider;
    QGeoMappingManager * m_mapManager;
    QString m_providerId;
    QMap<QString,QVariant> m_parameters;
    QPixmap m_markerIcon;
    QPoint m_markerOffset;
    QList<QGeoMapPixmapObject *> m_markerObjects;

    QMenu * m_popupMenu;
    QMenu * m_popupMenuMapObject;
    QPointF m_lastClicked;
    QGeoMapObject * m_lastClickedMapObject;

    QLineEdit * m_latitudeEdit;
    QLineEdit * m_longitudeEdit;
    QDialog * m_coordControlDialog;
    QPushButton * m_setCoordsButton;

    void createProvider();
    void createMapWidget();
    void createPixmapIcon();
    void createMenus();
    void createDialogs();
    void createOptionsDialog();
    DisplayMode m_displayMode;

    QWidget * m_optionsWidget;
    QPushButton * m_optionsButton;

protected:
    virtual void resizeEvent(QResizeEvent * event);

private slots:
    void routeFinished();
    void mapContextMenu(QGraphicsSceneContextMenuEvent * event, QGeoMapObject * clickedMapObject);
    void gotoCoordsClicked();

    void mapTypeToggled(int type);
    void connectivityModeToggled(int type);
    void showOptions();

private slots:
    void demo1();
    void demo2();
    void demo3();
    void drawRect();
    void drawPixmap();
    void drawPolyline();
    void drawPolygon();
    void drawCircle();
    void drawText();
    void removePixmaps();
    void removeMapObject();
    void selectObjects();
    void calcRoute();
    void captureCoordinates();
};

#endif // MAPBOX_H
