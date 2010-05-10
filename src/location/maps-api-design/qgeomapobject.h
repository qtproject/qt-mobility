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

#ifndef QGEOMAPOBJECT_H
#define QGEOMAPOBJECT_H

#include "qgeomapwidget.h"

#include <QList>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;
class QGeoMapWidget;
class QGeoRoute;

class QGraphicsItem;
class QPen;
class QBrush;
class QUrl;

class QGeoMapObjectPrivate;

// TODO better name - qgeomapgraphic? qgeomapdecoration?
// TODO decide if we even need QGeoMapObject
//      - can potentially just have QGeoMapWidget manage lists of QGraphicsItems
//      - the current QGeoMapObject subclasses would just subclass QGraphicsItem
//        directly

class Q_LOCATION_EXPORT QGeoMapObject : public QGraphicsItem
{

// graphics item can handle painting, hit detection, selection management etc

public:
    QGeoMapObject(QGeoMapWidget *parent);
    virtual ~QGeoMapObject();

    // for overlapping etc...
    void setZLevel(int zLevel);
    int zLevel() const;

    // don't draw objects that would appear as less than mlod x mlod pixels
    void setMinimumLevelOfDetail(int minimumLevelOfDetail);
    int minimumLevelOfDetail() const;

    // pen and brush for selected state as well?
    void setPen(const QPen &pen);
    QPen pen() const;

    void setBrush(const QBrush &brush);
    QBrush brush() const;

protected:
    QGeoMapObject(const QGeoMapObjectPrivate*d_ptr);
    QGeoMapObjectPrivate* d_ptr;

private:
    Q_DISABLE_COPY(QGeoMapObject)
    Q_DECLARE_PRIVATE(QGeoMapObject)
};

class QMapCircleObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapCircleObject : public QGeoMapObject
{
public:
    QGeoMapCircleObject(QGeoMapWidget *parent);
    ~QGeoMapCircleObject();

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setRadius(double radius, QGeoMapWidget::DistanceUnits units = QGeoMapWidget::Metres);
    double radius(QGeoMapWidget::DistanceUnits units = QGeoMapWidget::Metres) const;

private:
    Q_DISABLE_COPY(QGeoMapCircleObject)
    Q_DECLARE_PRIVATE(QGeoMapCircleObject)
};

class QMapRectangleObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapRectangleObject : public QGeoMapObject
{
public:
    QGeoMapRectangleObject(QGeoMapWidget *parent);
    ~QGeoMapRectangleObject();

    void setRectangle(const QGeoBoundingBox &rectangle);
    QGeoBoundingBox rectangle() const;

private:
    Q_DISABLE_COPY(QGeoMapRectangleObject)
    Q_DECLARE_PRIVATE(QGeoMapRectangleObject)
};

class QMapCirclePolylinePrivate;

class Q_LOCATION_EXPORT QGeoMapPolylineObject : public QGeoMapObject
{
public:
    QGeoMapPolylineObject(QGeoMapWidget *parent);
    ~QGeoMapPolylineObject();

    void setPoints(const QList<QGeoCoordinate> &points);
    QList<QGeoCoordinate> points() const;

private:
    Q_DISABLE_COPY(QGeoMapPolylineObject)
    Q_DECLARE_PRIVATE(QGeoMapPolylineObject)
};

class QMapPolygonObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapPolygonObject : public QGeoMapObject
{
public:
    QGeoMapPolygonObject(QGeoMapWidget *parent);
    ~QGeoMapPolygonObject();

    void setPoints(const QList<QGeoCoordinate> &points);
    QList<QGeoCoordinate> points() const;

private:
    Q_DISABLE_COPY(QGeoMapPolygonObject)
    Q_DECLARE_PRIVATE(QGeoMapPolygonObject)
};

// Option - standard markers
// Enum of marker types, map to images
// Do we let people update the mapping themselves?
// There may be system specific images which are meant to be used to meet
// UI guidelines, unclear what to do if this is the case for only some of
// the marker types

class Q_LOCATION_EXPORT QGeoMapMarkerObject : public QGeoMapObject
{
public:
    QGeoMapMarkerObject(QGeoMapWidget *parent);
    ~QGeoMapMarkerObject();

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setText(const QString &text);
    QString text() const;

    void setTextPen(const QPen &pen);
    QPen textPen() const;

    // centre of text at offset?
    void setTextOffsetFromCenter(const QPointF &offset);
    QPointF textOffsetFromCenter() const;


    void setImageUrl(const QUrl &imageUrl);
    QUrl imageUrl() const;

    // centre of image at offset?
    void setImageOffsetFromCenter(const QPointF &offset);
    QPointF imageOffsetFromCenter() const;

private:
    Q_DISABLE_COPY(QGeoMapMarkerObject)
    Q_DECLARE_PRIVATE(QGeoMapMarkerObject)
};

class Q_LOCATION_EXPORT QGeoMapBalloonObject : public QGeoMapObject
{
public:
    QGeoMapBalloonObject(QGeoMapWidget *parent);
    ~QGeoMapBalloonObject();

    // TODO work out what this needs

private:
    Q_DISABLE_COPY(QGeoMapBalloonObject)
    Q_DECLARE_PRIVATE(QGeoMapBalloonObject)
};

class Q_LOCATION_EXPORT QGeoMapRouteObject : public QGeoMapObject
{
public:
    QGeoMapRouteObject(QGeoMapWidget *parent);
    ~QGeoMapRouteObject();

    void setRoute(const QGeoRoute &route);
    QGeoRoute route() const;

private:
    Q_DISABLE_COPY(QGeoMapRouteObject)
    Q_DECLARE_PRIVATE(QGeoMapRouteObject)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif

