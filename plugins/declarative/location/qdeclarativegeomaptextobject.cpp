
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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomaptextobject_p.h"

#include <QPen>
#include <QBrush>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapText

    \brief The MapText element displays text on a map.
    \inherits QGeoMapTextObject

    \ingroup qml-location-maps

    The string \l text will be drawn \l offset.x and \l offset.y pixels away from the 
    on-screen position of \l coordinate.

    The text will be rendered with font \l font and color \l color, and will 
    be aligned according to \l horizontalAlignment and \l verticalAlignment.

    If \l text is empty or \l coordinate is invalid nothing will be displayed.

    The MapText element is part of the \bold{QtMobility.location 1.1} module.
*/

QDeclarativeGeoMapTextObject::QDeclarativeGeoMapTextObject()
{
    QPen p = pen();
    p.setStyle(Qt::NoPen);
    setPen(p);

    setBrush(QBrush(Qt::black));

    m_coordinate = new QDeclarativeCoordinate(this);

    connect(m_coordinate,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(coordinateLatitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(coordinateLongitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(coordinateAltitudeChanged(double)));

    m_hAlignment = QDeclarativeGeoMapTextObject::AlignHCenter;
    m_vAlignment = QDeclarativeGeoMapTextObject::AlignVCenter;
}

QDeclarativeGeoMapTextObject::~QDeclarativeGeoMapTextObject()
{
}

/*!
    \qmlproperty Coordinate MapText::coordinate

    This property holds the coordinate at which to anchor the text.
*/

void QDeclarativeGeoMapTextObject::setDeclarativeCoordinate(const QDeclarativeCoordinate *coordinate)
{
    if (m_coordinate->coordinate() == coordinate->coordinate())
        return;

    m_coordinate->setCoordinate(coordinate->coordinate());
    setCoordinate(coordinate->coordinate());

    emit declarativeCoordinateChanged(m_coordinate);
}

QDeclarativeCoordinate* QDeclarativeGeoMapTextObject::declarativeCoordinate()
{
    return m_coordinate;
}

void QDeclarativeGeoMapTextObject::coordinateLatitudeChanged(double /*latitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapTextObject::coordinateLongitudeChanged(double /*longitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapTextObject::coordinateAltitudeChanged(double /*altitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

/*!
    \qmlproperty string MapText::text

    This property holds the text to display.

    The default value is an empty string.
*/

/*!
    \qmlproperty string MapText::font.family

    Sets the family name of the font.

    The family name is case insensitive and may optionally include a foundry name, e.g. "Helvetica [Cronyx]".
    If the family is available from more than one foundry and the foundry isn't specified, an arbitrary foundry is chosen.
    If the family isn't available a family will be set using the font matching algorithm.
*/

/*!
    \qmlproperty bool MapText::font.bold

    Sets whether the font weight is bold.
*/

/*!
    \qmlproperty enumeration MapText::font.weight

    Sets the font's weight.

    The weight can be one of:
    \list
    \o Font.Light
    \o Font.Normal - the default
    \o Font.DemiBold
    \o Font.Bold
    \o Font.Black
    \endlist

    \qml
    MapText { text: "Hello"; font.weight: Font.DemiBold }
    \endqml
*/

/*!
    \qmlproperty bool MapText::font.italic

    Sets whether the font has an italic style.
*/

/*!
    \qmlproperty bool MapText::font.underline

    Sets whether the text is underlined.
*/

/*!
    \qmlproperty bool MapText::font.strikeout

    Sets whether the font has a strikeout style.
*/

/*!
    \qmlproperty real MapText::font.pointSize

    Sets the font size in points. The point size must be greater than zero.
*/

/*!
    \qmlproperty int MapText::font.pixelSize

    Sets the font size in pixels.

    Using this function makes the font device dependent.
    Use \c pointSize to set the size of the font in a device independent manner.
*/

/*!
    \qmlproperty real MapText::font.letterSpacing

    Sets the letter spacing for the font.

    Letter spacing changes the default spacing between individual letters in the font.
    A positive value increases the letter spacing by the corresponding pixels; a negative value decreases the spacing.
*/

/*!
    \qmlproperty real MapText::font.wordSpacing

    Sets the word spacing for the font.

    Word spacing changes the default spacing between individual words.
    A positive value increases the word spacing by a corresponding amount of pixels,
    while a negative value decreases the inter-word spacing accordingly.
*/

/*!
    \qmlproperty enumeration MapText::font.capitalization

    Sets the capitalization for the text.

    \list
    \o Font.MixedCase - This is the normal text rendering option where no capitalization change is applied.
    \o Font.AllUppercase - This alters the text to be rendered in all uppercase type.
    \o Font.AllLowercase	 - This alters the text to be rendered in all lowercase type.
    \o Font.SmallCaps -	This alters the text to be rendered in small-caps type.
    \o Font.Capitalize - This alters the text to be rendered with the first character of each word as an uppercase character.
    \endlist

    \qml
    MapText { text: "Hello"; font.capitalization: Font.AllLowercase }
    \endqml
*/


/*!
    \qmlproperty int MapText::offset.x
    \qmlproperty int MapText::offset.y

    These properties hold the offset from the on-screen position of 
    \l coordinate at which the text should be displayed.

    They both default to 0.
*/

/*!
    \qmlproperty color MapText::color

    This property holds the color used to draw the text.

    The default value is black.
*/

void QDeclarativeGeoMapTextObject::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    QBrush m_brush(color);
    setBrush(m_brush);
    emit colorChanged(m_color);
}

QColor QDeclarativeGeoMapTextObject::color() const
{
    return m_color;
}

/*!
    \qmlproperty enumeration MapText::horizontalAlignment
    \qmlproperty enumeration MapText::verticalAlignment

    Sets the horizontal and vertical alignment of the text.

    The alignment is relative to the point \l offset.x and \l offset.y pixels away from 
    the on-screen position of \l coordinate.

    The valid values for \c horizontalAlignment are \c MapText.AlignLeft,
    \c MapText.AlignRight and \c MapText.AlignHCenter.  The valid values 
    for \c verticalAlignment are \c MapText.AlignTop, \c MapText.AlignBottom 
    and \c MapText.AlignVCenter.

    The default values are \c MapText.AlignHCenter and \c MapText.AlignVCenter 
    respectively.
*/

QDeclarativeGeoMapTextObject::HorizontalAlignment QDeclarativeGeoMapTextObject::horizontalAlignment() const
{
    return m_hAlignment;
}

void QDeclarativeGeoMapTextObject::setHorizontalAlignment(QDeclarativeGeoMapTextObject::HorizontalAlignment alignment)
{
    if (m_hAlignment == alignment)
        return;


    m_hAlignment = alignment;

    setAlignment(Qt::Alignment(m_hAlignment | m_vAlignment));

    emit horizontalAlignmentChanged(m_hAlignment);
}


QDeclarativeGeoMapTextObject::VerticalAlignment QDeclarativeGeoMapTextObject::verticalAlignment() const
{
    return m_vAlignment;
}

void QDeclarativeGeoMapTextObject::setVerticalAlignment(QDeclarativeGeoMapTextObject::VerticalAlignment alignment)
{
    if (m_vAlignment == alignment)
        return;

    m_vAlignment = alignment;

    setAlignment(Qt::Alignment(m_hAlignment | m_vAlignment));

    emit verticalAlignmentChanged(m_vAlignment);
}

/*!
    \qmlproperty int MapText::zValue

    This property holds the z-value of the text.

    Map objects are drawn in z-value order, and objects with the 
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapText::visible

    This property holds a boolean corresponding to whether or not the 
    text is visible.
*/

/*!
    \qmlproperty bool MapText::selected

    This property holds a boolean corresponding to whether or not the 
    text is selected.
*/

#include "moc_qdeclarativegeomaptextobject_p.cpp"

QTM_END_NAMESPACE

