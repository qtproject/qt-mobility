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

#ifndef QGEOMAPMARKEROBJECT_H
#define QGEOMAPMARKEROBJECT_H

#include "qgeomapobject.h"
#include "qgeocoordinate.h"

#include <QPixmap>
#include <QPoint>

QTM_BEGIN_NAMESPACE

class QGeoMapMarkerObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapMarkerObject : public QGeoMapObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QPixmap icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QPoint anchor READ anchor WRITE setAnchor NOTIFY anchorChanged)

public:
    QGeoMapMarkerObject(const QGeoCoordinate &coordinate, const QPoint &anchor = QPoint(0, 0), const QPixmap &icon = QPixmap(), QGeoMapObject *parent = 0);
    ~QGeoMapMarkerObject();

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    QPixmap icon() const;
    void setIcon(const QPixmap &icon);

    QPoint anchor() const;
    void setAnchor(const QPoint &anchor);

signals:
    void coordinateChanged(const QGeoCoordinate &coordinate);
    void iconChanged(const QPixmap &pixmap);
    void anchorChanged(const QPoint &anchor);

private:
    Q_DECLARE_PRIVATE(QGeoMapMarkerObject)
    Q_DISABLE_COPY(QGeoMapMarkerObject)
};

QTM_END_NAMESPACE

#endif
