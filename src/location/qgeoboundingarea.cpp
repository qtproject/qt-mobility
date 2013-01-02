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

#include "qgeoboundingarea.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoBoundingArea
    \brief The QGeoBoundingArea class defines a geographic area.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps

    This class is the base class for classes which specify a geographic
    area.

    For the sake of consistency, subclasses should describe the specific
    details of the associated areas in terms of QGeoCoordinate instances
    and distances in metres.
*/

/*!
    \enum QGeoBoundingArea::AreaType

    Describes the type of a bounding area.

    \value BoxType A box shaped bounding area.

    \value CircleType A circular bounding area.
*/

/*!
    Destroys this bounding area.
*/
QGeoBoundingArea::~QGeoBoundingArea() {}

/*!
\fn virtual QGeoBoundingArea::AreaType QGeoBoundingArea::type() const

Returns the type of this area.
\since 1.1
*/

/*!
\fn bool QGeoBoundingArea::isValid() const

Returns whether this bounding area is valid.

An area is considered to be invalid if some of the data that is required to
unambiguously describe the area has not been set or has been set to an
unsuitable value.
\since 1.1
*/

/*!
\fn bool QGeoBoundingArea::isEmpty() const

Returns whether this bounding area is empty.

An empty area is a region which has a geometrical area of 0.
\since 1.1
*/

/*!
\fn bool QGeoBoundingArea::contains(const QGeoCoordinate &coordinate) const

Returns whether the coordinate \a coordinate is contained within this
area.
\since 1.1
*/

QTM_END_NAMESPACE
