/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
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
    \since 1.1

    \ingroup maps-mapping-objects

    The text is drawn at a given coordinate on the map.

    The text object is drawn such that the rendered size of the text object is
    independent of the zoom level of the map.
*/

/*!
    Constructs a new text object.
*/
QGeoMapTextObject::QGeoMapTextObject()
    : d_ptr(new QGeoMapTextObjectPrivate())
{
    setUnits(QGeoMapObject::PixelUnit);
}

/*!
    Constructs a new text object which will display the text \a text with font
    \a font at the coordinate \a coordinate on the map.

    The text will be displayed at an on-screen offset from the coordinate of \a
    offset pixels, and will be aligned according to \a alignment.
    \since 1.1
*/
QGeoMapTextObject::QGeoMapTextObject(const QGeoCoordinate &coordinate,
                                     const QString &text,
                                     const QFont &font,
                                     const QPoint &offset,
                                     Qt::Alignment alignment)
    : d_ptr(new QGeoMapTextObjectPrivate())
{
    setOrigin(coordinate);
    d_ptr->text = text;
    d_ptr->font = font;
    d_ptr->offset = offset;
    d_ptr->alignment = alignment;
    setUnits(QGeoMapObject::PixelUnit);
}

/*!
    Destroys this text object.
*/
QGeoMapTextObject::~QGeoMapTextObject()
{
    delete d_ptr;
}

/*!
    \reimp
    \since 1.1
*/
QGeoMapObject::Type QGeoMapTextObject::type() const
{
    return QGeoMapObject::TextType;
}

/*!
    \property QGeoMapTextObject::coordinate
    \brief the coordinate at which this text object
    will be rendered.

    The default value of this property is an invalid coordinate. While the
    value of this property is invalid the text object will not be displayed.

    If QGeoMapTextObject::offset and QGeoMapTextObject::alignment are not set
    the text will be drawn so that it is centered both horizontally and
    vertically around the position of QGeoMapTextObject::coordinate on the
    screen.
    \since 1.1
*/
QGeoCoordinate QGeoMapTextObject::coordinate() const
{
    return origin();
}

void QGeoMapTextObject::setCoordinate(const QGeoCoordinate &coordinate)
{
    if (origin() != coordinate) {
        setOrigin(coordinate);
        emit coordinateChanged(coordinate);
    }
}

/*!
    \property QGeoMapTextObject::text
    \brief the text that will be displayed by this text
    object.

    The default value of this property is an empty string.
    \since 1.1
*/
QString QGeoMapTextObject::text() const
{
    return d_ptr->text;
}

void QGeoMapTextObject::setText(const QString &text)
{
    if (d_ptr->text != text) {
        d_ptr->text = text;
        emit textChanged(text);
    }
}

/*!
    \property QGeoMapTextObject::font
    \brief the font that will be used to render the text
    for this text object.

    The default value of this property is the application's default font.

    It is not necessary to account for the zoom level of the map, since text
    objects are scaled such that they appear to be independent of the zoom
    level.
    \since 1.1
*/
QFont QGeoMapTextObject::font() const
{
    return d_ptr->font;
}

void QGeoMapTextObject::setFont(const QFont &font)
{
    if (d_ptr->font != font) {
        d_ptr->font = font;
        emit fontChanged(font);
    }
}

/*!
    \property QGeoMapTextObject::pen
    \brief the pen that will be used to draw this object.

    The pen is used to draw an outline around the text. The text itself is
    filled using the QGeoMapTextObject::brush property.

    The pen will be treated as a cosmetic pen, which means that the width
    of the pen will be independent of the zoom level of the map.
    \since 1.1
*/
QPen QGeoMapTextObject::pen() const
{
    return d_ptr->pen;
}

void QGeoMapTextObject::setPen(const QPen &pen)
{
    QPen newPen = pen;
    newPen.setCosmetic(true);

    if (d_ptr->pen == newPen)
        return;

    d_ptr->pen = newPen;
    emit penChanged(pen);
}

/*!
    \property QGeoMapTextObject::brush
    \brief the brush that will be used to draw this object.

    The brush is used to fill in the glyphs of the text.

    The outline around the perimeter of the glyphs is drawn using the
    QGeoMapTextObject::pen property.
    \since 1.1
*/
QBrush QGeoMapTextObject::brush() const
{
    return d_ptr->brush;
}

void QGeoMapTextObject::setBrush(const QBrush &brush)
{
    if (d_ptr->brush != brush) {
        d_ptr->brush = brush;
        emit brushChanged(brush);
    }
}

/*!
    \property QGeoMapTextObject::offset
    \brief the offset in pixels from the screen position of
    QGeoMapTextObject::coordinate at which the text will be rendered when
    drawing this text object.

    The default value of this property is QPoint(0,0).
    \since 1.1
*/
QPoint QGeoMapTextObject::offset() const
{
    return QPoint(d_ptr->offset.x(), d_ptr->offset.y());
}

void QGeoMapTextObject::setOffset(const QPoint &off)
{
    QPointF offset = off;
    if (d_ptr->offset != off) {
        d_ptr->offset = off;
        emit offsetChanged(off);
    }
}

/*!
    \property QGeoMapTextObject::alignment
    \brief the alignment options used to align the
    text when drawing this text object.

    The default value of this property will align the text so that it is
    centered both horizontally and vertically around the point that is
    QGeoMapTextObject::offset pixels away from the position of
    QGeoMapTextObject::coordinate on the screen.

    Using
    \code
    textObject->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    \endcode
    will place the text so that the point halway up the left edge of
    the text is at the anchor point defined by
    QGeoMapTextObject::offset and QGeoMapTextObject::coordinate.

    The alignment property is a flag, so using
    \code
    textObject->setAlignment(Qt::AlignLeft);
    \endcode
    may alter the vertical alignment as well.

    The Qt::AlignVertical_Mask and Qt::AlignHorizontal_Mask enum
    values can be used to alter one component of the alignment
    independent of the other.
    \code
    textObject->setAlignment(Qt::AlignLeft | (textObject->alignment() & Qt::AlignVertical_Mask));
    \endcode

    The alignment does not take the width of QGeoMapTextObject::pen into
    consideration.
    \since 1.1
*/
Qt::Alignment QGeoMapTextObject::alignment() const
{
    return d_ptr->alignment;
}

void QGeoMapTextObject::setAlignment(Qt::Alignment alignment)
{
    if (d_ptr->alignment != alignment) {
        d_ptr->alignment = alignment;
        emit alignmentChanged(d_ptr->alignment);
    }
}

/*!
\fn void QGeoMapTextObject::coordinateChanged(const QGeoCoordinate &coordinate)

    This signal is emitted when the coordinate at which this text
    object will be drawn has changed.

    The new value is \a coordinate.
    \since 1.1
*/

/*!
\fn void QGeoMapTextObject::textChanged(const QString &text)

    This signal is emitted when the text to be drawn by this text object
    has changed.

    The new value is \a text.
    \since 1.1
*/

/*!
\fn void QGeoMapTextObject::fontChanged(const QFont &font)

    This signal is emitted when the font use to draw this text object
    has changed.

    The new value is \a font.
    \since 1.1
*/

/*!
\fn void QGeoMapTextObject::penChanged(const QPen &pen)

    This signal is emitted when the pen used to draw the outline of the
    letters for this text object has changed.

    The new value is \a pen.
    \since 1.1
*/

/*!
\fn void QGeoMapTextObject::brushChanged(const QBrush &brush)

    This signal is emitted when the brush used to fill the inside of the
    letters for this text object has changed.

    The new value is \a brush.
    \since 1.1
*/

/*!
\fn void QGeoMapTextObject::offsetChanged(const QPoint &offset)

    This signal is emitted when the on screen offset from the coordinate at
    which this text object will be drawn has changed.

    The new value is \a offset.
    \since 1.1
*/

/*!
\fn void QGeoMapTextObject::alignmentChanged(Qt::Alignment alignment)

    This signal is emitted when the alignment of this text object has changed.

    The new value is \a alignment.
    \since 1.1
*/

/*******************************************************************************
*******************************************************************************/

QGeoMapTextObjectPrivate::QGeoMapTextObjectPrivate() :
    offset(0, 0)
{
    pen.setCosmetic(true);
    alignment = Qt::AlignCenter;
}

QGeoMapTextObjectPrivate::~QGeoMapTextObjectPrivate() {}

#include "moc_qgeomaptextobject.cpp"

QTM_END_NAMESPACE

