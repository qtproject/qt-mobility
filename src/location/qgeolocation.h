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

#include "qmobilityglobal.h"
#include <QSharedDataPointer>

class QString;
class QRectF;

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoAddress;

class QGeoLocationPrivate;
class Q_LOCATION_EXPORT QGeoLocation
{
public:
    QGeoLocation();
    QGeoLocation(const QGeoLocation &gl);
    QGeoLocation& operator=(const QGeoLocation &gl);
    ~QGeoLocation();

    bool operator==(const QGeoLocation &other) const;
    bool operator!=(const QGeoLocation &other) const{return !(*this == other);}

    QRectF boundingBox() const;
    void setBoundingBox(const QRectF &boundingBox);

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    QGeoAddress address() const;
    void setAddress(const QGeoAddress &address);

private:
    QSharedDataPointer<QGeoLocationPrivate> d;
};

QTM_END_NAMESPACE

#endif
