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

#ifndef QLOCATION_GEOPLACE_H
#define QLOCATION_GEOPLACE_H

#include <QRectF>

#include "qaddress.h"
#include "qalternativeaddress.h"
#include "qgeocoordinatemaps.h"

QTM_BEGIN_NAMESPACE

/*!
* This class represent a geo location as returned by
* \ref QGeocodingReply::places().
*/
class QGeoLocation
{
    friend class QGeocodingXmlHandler;

public:
    /*!
    * Default constructor.
    */
    QGeoLocation() {}

    /*!
    * @return The bounding box that completely encloses the location.
    *         The x coordinates of the corner points represent longitudes,
              the y coordinates represent latitudes.
    */
    QRectF boundingBox() const {
        return box;
    }
    /*!
    * @return The geo coordinate of this location.
    */
    QGeoCoordinateMaps position() const {
        return pos;
    }
    /*!
    * @return Description of the location.
    */
    QString title() const {
        return ttl;
    }
    /*!
    * @return The MARC language used in the description of this location.
    */
    QString language() const {
        return lang;
    }
    /*!
    * @return The address found.
    */
    QAddress address() const {
        return addr;
    }
    /*!
    * @return Alternatives to the address found.
    */
    QAlternativeAddress alternatives() const {
        return altern;
    }

private:
    QRectF          box;
    QGeoCoordinateMaps  pos;
    QString         ttl;
    QString         lang;
    QAddress        addr;
    QAlternativeAddress altern;
};

QTM_END_NAMESPACE

#endif
