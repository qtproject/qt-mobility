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

#ifndef QGEOMAPOBJECT_H
#define QGEOMAPOBJECT_H

#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QGeoBoundingBox;
class QGeoMapObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapObject
{
public:
    enum Type {
        RectangleType,
        CircleType,
        PolylineType,
        PolygonType,
        MarkerType,
    };

    QGeoMapObject();
    virtual ~QGeoMapObject();

    Type type() const;

    void setZIndex(int zIndex);
    int zIndex() const;

    void setVisible(bool visible);
    bool isVisible() const;

    QGeoBoundingBox boundingBox() const;

    // either expose bounding box for hit testing
    // or have contains(QGeoCoordinate) and containedBy(QGeoBoundingBox)

protected:
    QGeoMapObject(QGeoMapObjectPrivate *dd);

    void setBoundingBox(const QGeoBoundingBox& boundingBox);

    QGeoMapObjectPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QGeoMapObject)
    Q_DISABLE_COPY(QGeoMapObject)
};

QTM_END_NAMESPACE

#endif
