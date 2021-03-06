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
***************************************************************************/

#include <qnumeric.h>
#include "qdeclarativegeoboundingbox_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass BoundingBox QDeclarativeGeoBoundingBox
    \brief The QGeoBoundingBox class defines a rectangular geographic area.
    \ingroup qml-location-plugin
    \since Mobility 1.2

    The BoundingBox class defines a rectangular geographic area.
    This element is part of the \bold{QtMobility.location 1.1} module.
    For behavioral details, please see \l QGeoBoundingBox.

    \sa Landmark, Place, Address, {QGeoBoundingBox}
*/

QDeclarativeGeoBoundingBox::QDeclarativeGeoBoundingBox(QObject* parent) :
        QObject(parent), m_height(qQNaN()), m_width(qQNaN())
{
}

QDeclarativeGeoBoundingBox::QDeclarativeGeoBoundingBox(const QGeoBoundingBox& box, QObject* parent) :
    QObject(parent),
    m_declarativeBottomLeft(box.bottomLeft()),
    m_declarativeBottomRight(box.bottomRight()),
    m_declarativeTopLeft(box.topLeft()),
    m_declarativeTopRight(box.topRight()),
    m_declarativeCenter(box.center()),
    m_box(box),
    m_height(box.height()),
    m_width(box.width())
{
}

void QDeclarativeGeoBoundingBox::setBox(const QGeoBoundingBox& box)
{
    m_box = box;
    synchronizeDeclarative();
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
  \since Mobility 1.2

  This property holds the bottom left coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setBottomLeft(QDeclarativeCoordinate *coordinate)
{
    if (m_box.bottomLeft() == coordinate->coordinate())
        return;
    m_box.setBottomLeft(coordinate->coordinate());
    synchronizeDeclarative();
    emit bottomLeftChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::bottomRight()
{
    return &m_declarativeBottomRight;
}

/*!
  \qmlproperty Coordinate BoundingBox::bottomRight
  \since Mobility 1.2

  This property holds the bottom right coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setBottomRight(QDeclarativeCoordinate *coordinate)
{
    if (m_box.bottomRight() == coordinate->coordinate())
        return;
    m_box.setBottomRight(coordinate->coordinate());
    synchronizeDeclarative();
    emit bottomRightChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::topLeft()
{
    return &m_declarativeTopLeft;
}

/*!
  \qmlproperty Coordinate BoundingBox::topLeft
  \since Mobility 1.2

  This property holds the top left coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setTopLeft(QDeclarativeCoordinate *coordinate)
{
    if (m_box.topLeft() == coordinate->coordinate())
        return;
    m_box.setTopLeft(coordinate->coordinate());
    synchronizeDeclarative();
    emit topLeftChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::topRight()
{
    return &m_declarativeTopRight;
}

/*!
  \qmlproperty Coordinate BoundingBox::topRight
  \since Mobility 1.2

  This property holds the top right coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setTopRight(QDeclarativeCoordinate *coordinate)
{
    if (m_box.topRight() == coordinate->coordinate())
        return;
    m_box.setTopRight(coordinate->coordinate());
    synchronizeDeclarative();
    emit topRightChanged();
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingBox::center()
{
    return &m_declarativeCenter;
}

/*!
  \qmlproperty Coordinate BoundingBox::center
  \since Mobility 1.2

  This property holds the center coordinate of this bounding box.

  Note: this property's changed() signal is currently emitted only if the
  whole element changes, not if only the contents of the element change.

  \sa {QGeoBoundingBox}

  */

void QDeclarativeGeoBoundingBox::setCenter(QDeclarativeCoordinate *coordinate)
{
    if (m_box.center() == coordinate->coordinate())
        return;
    m_box.setCenter(coordinate->coordinate());
    synchronizeDeclarative();
    emit centerChanged();
}

double QDeclarativeGeoBoundingBox::height()
{
    return m_height;
}

/*!
  \qmlproperty double BoundingBox::height
  \since Mobility 1.2

  This property holds the height of this bounding box (in degrees).

  \sa {QGeoBoundingBox}
  */

void QDeclarativeGeoBoundingBox::setHeight(const double height)
{
    m_box.setHeight(height);
    synchronizeDeclarative();
}

double QDeclarativeGeoBoundingBox::width()
{
    return m_width;
}

/*!
  \qmlproperty double BoundingBox::width
  \since Mobility 1.2

  This property holds the width of this bounding box (in degrees).

  \sa {QGeoBoundingBox}

  */

void QDeclarativeGeoBoundingBox::setWidth(const double width)
{
    m_box.setWidth(width);
    synchronizeDeclarative();
}

void QDeclarativeGeoBoundingBox::synchronizeDeclarative()
{
    m_declarativeBottomLeft.setCoordinate(m_box.bottomLeft());
    m_declarativeBottomRight.setCoordinate(m_box.bottomRight());
    m_declarativeTopLeft.setCoordinate(m_box.topLeft());
    m_declarativeTopRight.setCoordinate(m_box.topRight());
    m_declarativeCenter.setCoordinate(m_box.center());
    // Check not to compare two Not a Numbers, which by definition is 'false'.
    if ((!qIsNaN(m_width) || !qIsNaN(m_box.width())) && m_width != m_box.width()) {
        m_width = m_box.width();
        emit widthChanged();
    }
    if ((!qIsNaN(m_height) || !qIsNaN(m_box.height())) && m_height != m_box.height()) {
        m_height = m_box.height();
        emit heightChanged();
    }
}

#include "moc_qdeclarativegeoboundingbox_p.cpp"

QTM_END_NAMESPACE
