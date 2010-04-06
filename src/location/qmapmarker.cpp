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

#include "qmapmarker.h"
#include "qmapmarker_p.h"
#include <QPainter>

#define MARKER_HEIGHT 36
#define MARKER_WIDTH 25
#define MARKER_PIN_LEN 10

QTM_BEGIN_NAMESPACE

QMapMarkerPrivate::QMapMarkerPrivate()
    : pt(QGeoCoordinate()),
      box(QRectF()), icn(QPixmap()),
      txt(QString()), txtRect(QRectF()),
      txtFont(QFont()), fColor(QColor())
{
}

/*!
    \class QMapMarker
    \brief The QMapMarker class is an internal representation of a map marker.
    \ingroup location

    This class is used by QMapView internally to represent a map marker.
*/

/*!
    Constructs a map marker with coordinates defined by \a point.
    The marker will have the prescribed \a text in the given \a font and \a fontColor
    inside the \a textRect.  The marker is represented by the \a icon and be displayed
    at the layer specified by \a layerIndex.
*/
QMapMarker::QMapMarker(const QGeoCoordinate& point,
                       const QString& text, const QFont& font, const QColor& fontColor,
                       const QPixmap& icon, const QRectF& textRect,
                       quint16 layerIndex)
        : QMapObject(*new QMapMarkerPrivate, QMapObject::MarkerObject, layerIndex)
{
    Q_D(QMapMarker);
    d->pt = point;
    d->txt = text;
    d->txtFont = font;
    d->fColor = fontColor;
    d->icn = icon;
    d->txtRect = textRect;

    if (d->icn.isNull()) {
        d->box.setHeight(MARKER_HEIGHT);
        d->box.setWidth(MARKER_WIDTH);
    }
}

/*
    \internal
*/
QMapMarker::QMapMarker(QMapMarkerPrivate &dd, const QGeoCoordinate& point,
                       const QString& text, const QFont& font, const QColor& fontColor,
                       const QPixmap& icon, const QRectF& textRect,
                       quint16 layerIndex)
        : QMapObject(dd, QMapObject::MarkerObject, layerIndex)
{
    Q_D(QMapMarker);
    d->pt = point;
    d->txt = text;
    d->txtFont = font;
    d->fColor = fontColor;
    d->icn = icon;
    d->txtRect = textRect;

    if (d->icn.isNull()) {
        d->box.setHeight(MARKER_HEIGHT);
        d->box.setWidth(MARKER_WIDTH);
    }
}

/*!
    \fn QPixmap QMapMarker::icon() const

    Returns the icon used for displaying the marker.
*/
QPixmap QMapMarker::icon() const
{
    Q_D(const QMapMarker);
    return d->icn;
}

/*!
    \fn QGeoCoordinate QMapMarker::point() const

    Returns the geo coordinate that is marked.
*/
QGeoCoordinate QMapMarker::point() const
{
    Q_D(const QMapMarker);
    return d->pt;
}

QString QMapMarker::text() const
{
    Q_D(const QMapMarker);
    return d->txt;
}

QRectF QMapMarker::textRect() const
{
    Q_D(const QMapMarker);
    return d->txtRect;
}

QFont QMapMarker::font() const
{
    Q_D(const QMapMarker);
    return d->txtFont;
}

void QMapMarker::compMapCoords()
{
    Q_D(QMapMarker);

    if (!d->mapView)
        return;

    if (d->icn.isNull()) {
        d->mapPt = d->mapView->geoToMap(d->pt);
        d->box.moveLeft(d->mapPt.x() - (MARKER_WIDTH / 2));
        d->box.moveTop(d->mapPt.y() - (MARKER_HEIGHT - 1));
        d->intersectingTiles.clear();
        compIntersectingTiles(d->box);
    }
}

void QMapMarker::constructMarker(QPainter* painter)
{
    Q_D(QMapMarker);
    QPointF p1(0,0);
    QPointF p2(0, 0 - MARKER_PIN_LEN);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(p1, p2);
    QRectF ellipse(p2.x() - (MARKER_WIDTH / 2), p1.y() - (MARKER_HEIGHT - 1), MARKER_WIDTH, MARKER_WIDTH);
    pen.setWidth(1);
    painter->setPen(pen);
    QColor color(Qt::green);
    color.setAlpha(127);
    QBrush brush(color);
    painter->setBrush(brush);
    painter->drawEllipse(ellipse);

    if (!d->txt.isNull()) {
        painter->setFont(d->txtFont);
        pen.setColor(d->fColor);
        painter->setPen(pen);
        painter->drawText(ellipse, Qt::AlignCenter | Qt::TextWordWrap, d->txt);
    }
}

void QMapMarker::paint(QPainter* painter, const QRectF& viewPort)
{
    Q_D(QMapMarker);

    if (!d->mapView)
        return;

    if (d->icn.isNull()) {
        quint64 mapWidth = d->mapView->mapWidth();
        painter->save();
        painter->translate(-viewPort.left(), -viewPort.top());
        painter->translate(d->mapPt.x(), d->mapPt.y());
        constructMarker(painter);
        painter->restore();
    }
    //TODO: map marker with provided icon
}

bool QMapMarker::intersects(const QRectF& tileRect) const
{
    Q_D(const QMapMarker);
    return d->box.intersects(tileRect);
}

QTM_END_NAMESPACE
