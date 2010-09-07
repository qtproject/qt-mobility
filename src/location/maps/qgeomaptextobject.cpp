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

#include "qgeomaptextobject.h"
#include "qgeomaptextobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapTextObject
    \brief The QGeoMapTextObject class is a QGeoMapObject used to draw
    text on a map.

    \inmodule QtLocation

    \ingroup maps-mapping-objects

    The text is drawn at a given coordinate on the map.

    The text object is drawn such that the rendered size of the text object is
    independent of the zoom level of the map.

    \note Anchor and/or alignment options didn't make it into the beta release
    but will be present in the final 1.1 release.
*/

/*!
    Constructs a new text object with the parent \a parent.
*/
QGeoMapTextObject::QGeoMapTextObject(QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapTextObjectPrivate(this, parent)) {}

/*!
    Constructs a new text object which will display the text \a text with font
    \a font at the coordinate \a coordinate on the map, with the parent \a
    parent.
*/
QGeoMapTextObject::QGeoMapTextObject(const QGeoCoordinate &coordinate,
                                     const QString &text,
                                     const QFont &font,
                                     QGeoMapObject *parent)
        : QGeoMapObject(new QGeoMapTextObjectPrivate(this, parent))
{
    Q_D(QGeoMapTextObject);

    d->coordinate = coordinate;
    d->text = text;
    d->font = font;
}

/*!
    Destroys this text object.
*/
QGeoMapTextObject::~QGeoMapTextObject()
{
}

/*!
    \property QGeoMapTextObject::coordinate
    \brief This property holds the coordinate at which this text object
    will be rendered.

    The default value of this property is an invalid coordinate. While the
    value of this property is invalid the text object will not be displayed.

    The text will be drawn such that the upper left corner of the bounding box
    of the text will appear at the position of the coordinate property.

    \note Anchor and/or alignment options didn't make it into the beta release
    but will be present in the final 1.1 release.
*/
QGeoCoordinate QGeoMapTextObject::coordinate() const
{
    Q_D(const QGeoMapTextObject);
    return d->coordinate;
}

void QGeoMapTextObject::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoMapTextObject);
    if (d->coordinate != coordinate) {
        d->coordinate = coordinate;
        objectUpdated();
        emit coordinateChanged(d->coordinate);
    }
}

/*!
    \property QGeoMapTextObject::text
    \brief This property holds the text that will be displayed by this text
    object.

    The default value of this property is an empty string.
*/
QString QGeoMapTextObject::text() const
{
    Q_D(const QGeoMapTextObject);
    return d->text;
}

void QGeoMapTextObject::setText(const QString &text)
{
    Q_D(QGeoMapTextObject);
    if (d->text != text) {
        d->text = text;
        objectUpdated();
        emit textChanged(d->text);
    }
}

/*!
    \property QGeoMapTextObject::font
    \brief This property holds the font that will be used to render the text
    for this text object.

    The default value of this property is the application's default font.

    It is not necessary to account for the zoom level of the map, since text
    objects are scaled such that they appear to be independent of the zoom
    level.
*/
QFont QGeoMapTextObject::font() const
{
    Q_D(const QGeoMapTextObject);
    return d->font;
}

void QGeoMapTextObject::setFont(const QFont &font)
{
    Q_D(QGeoMapTextObject);
    if (d->font != font) {
        d->font = font;
        objectUpdated();
        emit fontChanged(d->font);
    }
}

/*!
    \property QGeoMapTextObject::pen
    \brief This property holds the pen that will be used to draw this object.

    The pen is used to draw an outline around the text. The text itself is
    filled using the QGeoMapTextObject::brush property.

    The pen will be treated as a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
*/
QPen QGeoMapTextObject::pen() const
{
    Q_D(const QGeoMapTextObject);
    return d->pen;
}

void QGeoMapTextObject::setPen(const QPen &pen)
{
    Q_D(QGeoMapTextObject);

    QPen newPen = pen;
    newPen.setCosmetic(true);

    if (d->pen == newPen)
        return;

    d->pen = newPen;
    objectUpdated();
    emit penChanged(d->pen);
}

/*!
    \property QGeoMapTextObject::brush
    \brief This property holds the brush that will be used to draw this object.

    The brush is used to fill in the glyphs of the text.

    The outline around the perimeter of the glyphs is drawn using the
    QGeoMapTextObject::pen property.
*/
QBrush QGeoMapTextObject::brush() const
{
    Q_D(const QGeoMapTextObject);
    return d->brush;
}

void QGeoMapTextObject::setBrush(const QBrush &brush)
{
    Q_D(QGeoMapTextObject);
    if (d->brush != brush) {
        d->brush = brush;
        objectUpdated();
        emit brushChanged(d->brush);
    }
}

/*******************************************************************************
*******************************************************************************/

QGeoMapTextObjectPrivate::QGeoMapTextObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent)
        : QGeoMapObjectPrivate(impl, parent, QGeoMapObject::TextType)
{
    pen.setCosmetic(true);
}

QGeoMapTextObjectPrivate::~QGeoMapTextObjectPrivate() {}

#include "moc_qgeomaptextobject.cpp"

QTM_END_NAMESPACE

