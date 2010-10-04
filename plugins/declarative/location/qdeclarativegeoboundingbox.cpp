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
***************************************************************************/

#include <qnumeric.h>
#include "qdeclarativegeoboundingbox_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass BoundingBox QDeclarativeGeoBoundingBox
    \brief The QGeoBoundingBox class defines a rectangular geographic area.
    \ingroup qml-location

    This element is part of the \bold{QtMobility.location 1.1} module.

    The QGeoBoundingBox class defines a rectangular geographic area.

    \snippet TODO

    \sa Landmark, Place, Address, {QGeoBoundingBox}
*/

QDeclarativeGeoBoundingBox::QDeclarativeGeoBoundingBox(QObject* parent) :
        QObject(parent)
{
}

QDeclarativeGeoBoundingBox::QDeclarativeGeoBoundingBox(const QGeoBoundingBox& box, QObject* parent) :
    QObject(parent),
    m_declarativeBottomLeft(box.bottomLeft()),
    m_declarativeBottomRight(box.bottomRight()),
    m_declarativeTopLeft(box.topLeft()),
    m_declarativeTopRight(box.topRight()),
    m_declarativeCenter(box.center()),
    m_box(box)
{
}

void QDeclarativeGeoBoundingBox::setBox(const QGeoBoundingBox& box)
{
    m_declarativeBottomLeft.setCoordinate(box.bottomLeft());
    m_declarativeBottomRight.setCoordinate(box.bottomRight());
    m_declarativeTopLeft.setCoordinate(box.topLeft());
    m_declarativeTopRight.setCoordinate(box.topRight());
    m_declarativeCenter.setCoordinate(box.center());
    setHeight(box.height());
    setWidth(box.width());
    m_box = box;
}

QGeoBoundingBox QDeclarativeGeoBoundingBox::box()
{
    return m_box;
}


QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::bottomLeft()
{
    return &m_declarativeBottomLeft;
}

/*!
  \qmlproperty Coordinate BoundingBox::bottomLeft

  This property holds the bottom left coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setBottomLeft(QDeclarativeCoordinate *coordinate)
{
    if (m_declarativeBottomLeft.coordinate() == coordinate->coordinate())
        return;
    m_declarativeBottomLeft.setCoordinate(coordinate->coordinate());
    emit bottomLeftChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::bottomRight()
{
    return &m_declarativeBottomRight;
}

/*!
  \qmlproperty Coordinate BoundingBox::bottomRight

  This property holds the bottom right coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setBottomRight(QDeclarativeCoordinate *coordinate)
{
    if (m_declarativeBottomRight.coordinate() == coordinate->coordinate())
        return;
    m_declarativeBottomRight.setCoordinate(coordinate->coordinate());
    emit bottomRightChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::topLeft()
{
    return &m_declarativeTopLeft;
}

/*!
  \qmlproperty Coordinate BoundingBox::topLeft

  This property holds the top left coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setTopLeft(QDeclarativeCoordinate *coordinate)
{
    if (m_declarativeTopLeft.coordinate() == coordinate->coordinate())
        return;
    m_declarativeTopLeft.setCoordinate(coordinate->coordinate());
    emit topLeftChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::topRight()
{
    return &m_declarativeTopRight;
}

/*!
  \qmlproperty Coordinate BoundingBox::topRight

  This property holds the top right coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setTopRight(QDeclarativeCoordinate *coordinate)
{
    if (m_declarativeTopRight.coordinate() == coordinate->coordinate())
        return;
    m_declarativeTopRight.setCoordinate(coordinate->coordinate());
    emit topRightChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::center()
{
    return &m_declarativeCenter;
}

/*!
  \qmlproperty Coordinate BoundingBox::center

  This property holds the center coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}

  */

void QDeclarativeGeoBoundingBox::setCenter(QDeclarativeCoordinate *coordinate)
{
    if (m_declarativeCenter.coordinate() == coordinate->coordinate())
        return;
    m_declarativeCenter.setCoordinate(coordinate->coordinate());
    emit centerChanged();
}

double QDeclarativeGeoBoundingBox::height()
{
    return m_height;
}

/*!
  \qmlproperty double BoundingBox::height

  This property holds the height of this bounding box (in degrees).

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setHeight(const double height)
{
    if ((!qIsNaN(height) || !qIsNaN(m_box.height())) &&
            m_box.height() == height)
        return;
    m_box.setHeight(height);
    emit heightChanged();
}

double QDeclarativeGeoBoundingBox::width()
{
    return m_width;
}

/*!
  \qmlproperty double BoundingBox::width

  This property holds the width of this bounding box (in degrees).

  \sa {QGeoBoundingBox}

  */

void QDeclarativeGeoBoundingBox::setWidth(const double width)
{
    if ((!qIsNaN(width) || !qIsNaN(m_box.width())) &&
            m_box.width() == width)
        return;
    m_box.setWidth(width);
    emit widthChanged();
}

#include "moc_qdeclarativegeoboundingbox_p.cpp"

QTM_END_NAMESPACE
