/*
* ==============================================================================
*  Name        : QMapMarker.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPMARKER_H
#define QLOCATION_MAPMARKER_H

#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QFont>
#include <QColor>

#include "qmapobject.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

class QMapMarker : public QMapObject
{
    friend class QMapView;

public:
    QMapMarker(const QMapView& mapView, const QGeoCoordinateMaps& point,
               const QString& text, const QFont& font, const QColor& fontColor,
               const QPixmap& icon, const QRectF& textRect,
               quint16 layerIndex);
    /*!
    * @return The icon used for displaying the marker.
    */
    inline QPixmap icon() const {
        return icn;
    }
    /*!
    * @return The geo coordinate that is marked.
    */
    inline QGeoCoordinateMaps point() const {
        return pt;
    }
    inline QString text() const {
        return txt;
    }
    inline QRectF textRect() const {
        return txtRect;
    }
    inline QFont font() const {
        return txtFont;
    }

protected:
    virtual bool intersects(const QRectF& tileRect) const;
    virtual void compMapCoords();
    virtual void paint(QPainter* painter, const QRectF& viewPort);

    void constructMarker(QPainter* painter, const QPointF& point);

protected:
    QGeoCoordinateMaps pt;
    QPointF mapPt;
    QRectF box;
    QPixmap icn;
    QString txt;
    QRectF txtRect;
    QFont txtFont;
    QColor fColor;
};

QTM_END_NAMESPACE

#endif
