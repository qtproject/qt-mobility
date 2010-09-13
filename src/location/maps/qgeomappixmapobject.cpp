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

#include "qgeomappixmapobject.h"
#include "qgeomappixmapobject_p.h"
#include "qgeoboundingbox.h"

#include <QSvgRenderer>
#include <QPainter>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapPixmapObject
    \brief The QGeoMapPixmapObject class is a QGeoMapObject used to draw
    a pixmap on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The pixmap object is rendered such that the upper left corner of
    QGeoMapPixmapObject::pixmap will be drawn QGeoMapPixmapObject::offset
    pixels away from the position of QGeoMapPixmapObject::coordinate on the
    map.
*/

/*!
    Constructs a new pixmap object with the parent \a parent.
*/
QGeoMapPixmapObject::QGeoMapPixmapObject(QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapPixmapObjectPrivate(this, parent)) {}

/*!
    Constructs a new pixmap object which will draw the pixmap \a pixmap at an
    offset of \a offset pixels from the coordinate \a coordinate, with the
    parent \a parent.
*/
QGeoMapPixmapObject::QGeoMapPixmapObject(const QGeoCoordinate &coordinate, const QPoint &offset, const QPixmap &pixmap, QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapPixmapObjectPrivate(this, parent))
{
    Q_D(QGeoMapPixmapObject);

    d->coordinate = coordinate;
    d->pixmap = pixmap;
    d->offset = offset;
}

/*!
    Destroys this pixmap object.
*/
QGeoMapPixmapObject::~QGeoMapPixmapObject()
{
}

/*!
    \property QGeoMapPixmapObject::coordinate
    \brief This property holds the coordinate that specifies where the pixmap
    will be drawn by this pixmap object.

    The default value of this property is an invalid coordinate. While the
    value of this property is invalid the pixmap object will not be displayed.
*/
QGeoCoordinate QGeoMapPixmapObject::coordinate() const
{
    Q_D(const QGeoMapPixmapObject);
    return d->coordinate;
}

void QGeoMapPixmapObject::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoMapPixmapObject);
    if (d->coordinate != coordinate) {
        d->coordinate = coordinate;
        objectUpdated();
        emit coordinateChanged(d->coordinate);
    }
}

/*!
    \property QGeoMapPixmapObject::pixmap
    \brief This property holds the pixmap that will be drawn by this pixmap
    object.

    The default value of this property is a null pixmap. While the value of
    this property is the null pixmap the pixmap object will not be displayed.

    The pixmap will be drawn such that the upper left corner of the pixmap
    will be drawn QGeoMapPixmapObject::offset pixels away from the position of
    QGeoMapPixmapObject::coordinate on the map.
*/
QPixmap QGeoMapPixmapObject::pixmap() const
{
    Q_D(const QGeoMapPixmapObject);
    return d->pixmap;
}

void QGeoMapPixmapObject::setPixmap(const QPixmap &pixmap)
{
    Q_D(QGeoMapPixmapObject);

    if (d->pixmap.isNull() && pixmap.isNull())
        return;

    if ((d->pixmap.isNull() && !pixmap.isNull())
            || (!d->pixmap.isNull() && pixmap.isNull())
            || (d->pixmap.toImage() != pixmap.toImage())) {
        d->pixmap = pixmap;
        objectUpdated();
        emit pixmapChanged(d->pixmap);
    }
}

/*!
    \property QGeoMapPixmapObject::offset
    \brief This property holds the offset in pixels at which the pixmap will be
    drawn by this pixmap object.

    The default value of this property is QPoint(0, 0). If this value is not
    changed the upper left coordinate of the pixmap will be drawn at the
    coordinate specified by QGeoMapPixmapObject::coordinate.

    The offset is in pixels and is independent of the zoom level of the map.
    The offset property is provided so that pixmaps such as arrows can be drawn
    with the point of the arrow placed exactly on the associated coordinate.
*/
QPoint QGeoMapPixmapObject::offset() const
{
    Q_D(const QGeoMapPixmapObject);
    return d->offset;
}

void QGeoMapPixmapObject::setOffset(const QPoint &offset)
{
    Q_D(QGeoMapPixmapObject);
    if (d->offset != offset) {
        d->offset = offset;
        objectUpdated();
        emit offsetChanged(d->offset);
    }
}

void QGeoMapPixmapObject::becomeStandardMarker(MapShapeType shapeType, const QString & text, const QPen & pen, const QPen & textPen, const QBrush & brush)
{
    QString svgTemplate;
    switch (shapeType) {
        case SHAPE_BALLOON:
            svgTemplate = "<svg>"
                "<path "
                    "style=\"fill:#000000;fill-opacity:.4\" "
                    "d=\"m 18.948,33.432051 c 0,1.41694 -2.238,2.567949 -5,2.567949 -2.762,0 -5,-1.151009 -5,-2.567949 0,-1.416939 2.238,-2.567948 5,-2.567948 2.762,-0.002 5,1.149994 5,2.567948\" "
                "/>"
                "<path "
                    "style=\"fill:#666\" "
                    "d=\"M 14 0 C 6.28 0 0 6.3348796 0 14.125 C 0 16.209809 0.4425 18.209911 1.3125 20.09375 L 1.84375 21.125 C 2.96275 23.084964 12.2565 32.190555 13.3125 33.21875 L 14 33.875 L 14.6875 33.21875 C 15.7435 32.188525 25.026 23.109909 26.125 21.1875 L 26.65625 20.15625 C 27.54825 18.225721 28 16.204734 28 14.125 C 28.001 6.3348796 21.72 0 14 0 z M 14 0.90625 C 21.175 0.90625 27 6.7640687 27 14 C 27 15.982294 26.55825 17.873541 25.78125 19.5625 L 25.28125 20.53125 C 24.22125 22.386669 14 32.375 14 32.375 C 14 32.375 3.77875 22.387684 2.71875 20.53125 L 2.21875 19.5625 C 1.43975 17.875571 1 15.982294 1 14 C 1 6.7640687 6.823 0.90625 14 0.90625 z \" "
                "/>"
                "<path "
                    "style=\"fill:#fff\" "
                    "d=\"M 14 0.90625 C 6.823 0.90625 1 6.7640687 1 14 C 1 15.982294 1.43975 17.875571 2.21875 19.5625 L 2.71875 20.53125 C 3.77875 22.387684 14 32.375 14 32.375 C 14 32.375 24.22125 22.386669 25.28125 20.53125 L 25.78125 19.5625 C 26.55825 17.873541 27 15.982294 27 14 C 27 6.7640687 21.175 0.90625 14 0.90625 z M 14 1.90625 C 20.616 1.90625 26 7.3294234 26 14 C 25.910217 16.115917 25.491329 18.196844 24.40625 20.03125 C 23.776565 21.248455 17.937 27.075427 14 30.9375 C 10.063 27.075427 4.206579 21.245609 3.59375 20.0625 C 2.614452 18.171896 1.90079 16.186259 2 14 C 2 7.3294234 7.383 1.90625 14 1.90625 z \" "
                "/>"
                "<path "
                    "style=\"fill:%brush%;fill-opacity:%brushOpacity%\" "
                    "d=\"m 13.996268,1.9105251 c -6.617,0 -12.001,5.4261873 -12.001,12.0967639 -0.09921,2.186259 0.631702,4.174018 1.611,6.064622 0.612829,1.183109 6.453,7.017707 10.39,10.87978 3.937,-3.862073 9.777315,-9.70013 10.407,-10.917335 1.085079,-1.834406 1.503217,-3.91115 1.593,-6.027067 0,-6.6705766 -5.384,-12.0967639 -12,-12.0967639 l 0,0 z\" "
                "/>"
                "<path style=\"fill:#ffffff;fill-opacity:0.7\" "
                    "d=\"M 20.968528,6.089997 C 17.785074,3.5240085 13.725682,2.5379787 11.919627,3.8924751 11.436514,4.2548035 11.151528,4.750748 11.073071,5.3665525 c 4.1092,0.6284223 7.111478,2.6511761 8.521564,4.4853881 2.388133,3.1064364 1.967904,3.9515754 1.967904,3.9515754 0.774274,-0.0387 1.422004,-0.242256 1.904023,-0.603757 1.806055,-1.354497 0.684944,-4.5441591 -2.498034,-7.109762 l 0,0 z\" "
                "/>"
                "<text "
                    "x=\"14\" y=\"19\" "
                    "font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" "
                    "fill=\"%textPen%\" "
                    "textContent = \"%textContent%\""
                ">"
                    "%text%"
                "</text>"
            "</svg>";
            setOffset(QPoint(-14,-34));
            break;

        case SHAPE_STAR:
            svgTemplate = "<svg><path style=\"fill:%brush%;fill-opacity:%brushOpacity%\" d=\"M 16.00663,.0000019037492 C 7.1692812,.0000019037492 0,7.1692841 0,16.006632 0,24.843981 7.1692812,32 16.00663,32 24.84398,32 32,24.843981 32,16.006632 32,7.1692841 24.84398,.0000019037492 16.00663,.0000019037492 z M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><path style=\"fill:%pen%;fill-opacity:%penOpacity%\" d=\"M 15.847492,0.39784686 L 19.136344,11.484461 30.766679,10.993786 21.178615,17.544966 25.236635,28.379612 16.00663,21.324494 6.8959804,28.512225 10.794861,17.624536 1.1007042,11.232493 12.73104,11.537507 15.847492,0.39784686 z\" /><text x=\"16\" y=\"20\" font-size=\"10pt\" font-family=\"arial\" font-weight=\"bold\" text-anchor=\"middle\" fill=\"%textPen%\" textContent = \"%textContent%\">%text%</text></svg>";
            setOffset(QPoint(-16,-32));
            break;
    }

    svgTemplate.replace("%text%", text, Qt::CaseSensitive);
    svgTemplate.replace("%pen%", pen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%penOpacity%", QString::number(pen.color().alpha()/255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textPen%", textPen.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brush%", brush.color().name(), Qt::CaseSensitive);
    svgTemplate.replace("%brushOpacity%", QString::number(brush.color().alpha()/255.0), Qt::CaseSensitive);
    svgTemplate.replace("%textContent%", "", Qt::CaseSensitive);

    QSvgRenderer renderer(svgTemplate.toAscii());
    QPixmap pixmap(renderer.defaultSize());
    pixmap.fill(QColor(0,0,0,0));
    QPainter painter(&pixmap);
    renderer.render(&painter);

    setPixmap(pixmap);
}


/*******************************************************************************
*******************************************************************************/

QGeoMapPixmapObjectPrivate::QGeoMapPixmapObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::PixmapType) {}

QGeoMapPixmapObjectPrivate::~QGeoMapPixmapObjectPrivate() {}

#include "moc_qgeomappixmapobject.cpp"

QTM_END_NAMESPACE

