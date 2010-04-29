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

#ifndef QGEOBOUNDINGBOX_H
#define QGEOBOUNDINGBOX_H

#include "qmobilityglobal.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBoxPrivate;

class Q_LOCATION_EXPORT QGeoBoundingBox {
public:
    QGeoBoundingBox();
    QGeoBoundingBox(const QGeoBoundingBox &other);
    QGeoBoundingBox(const QGeoCoordinate &upperLeft, const QGeoCoordinate &lowerRight);
    ~QGeoBoundingBox();

    QGeoBoundingBox& operator = (const QGeoBoundingBox &other);

    bool isValid() const;

    void setUpperLeft(const QGeoCoordinate &upperLeft);
    QGeoCoordinate upperLeft() const;

    void setUpperRight(const QGeoCoordinate &lowerRight);
    QGeoCoordinate lowerRight() const;

    // contains / intersects methods?

private:
    QGeoBoundingBoxPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoBoundingBox);
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif

