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

#ifndef QGEOMAPPINGMANAGER_DUMMY_H
#define QGEOMAPPINGMANAGER_DUMMY_H

#include <QGeoMappingManager>
#include <QTimer>

QTM_USE_NAMESPACE

class QGeoMapReplyDummy : public QGeoMapReply
{
    Q_OBJECT
public:
    QGeoMapReplyDummy(const QSize &size,QObject *parent = 0) : QGeoMapReply(parent)
    {
        QPixmap pixmap(":/tiles/5/png8.png");
        setMapImage(pixmap = pixmap.scaled(size));
        QTimer::singleShot(0, this, SIGNAL(finished()));    
    }
    ~QGeoMapReplyDummy() {}

};

class QGeoMappingManagerDummy: public QGeoMappingManager
{
Q_OBJECT
public:
    QGeoMappingManagerDummy();
    ~QGeoMappingManagerDummy();

    QGeoMapReply* requestMap(const QGeoCoordinate &center, int zoomLevel, const QSize &size,
        const QGeoMapRequestOptions &requestOptions);

    QGeoMapReply* requestTile(int row, int col, int zoomLevel, const QSize &size,
        const QGeoMapRequestOptions &requestOptions);

    QGeoMapReply* requestTile(const QGeoCoordinate &onTile, int zoomLevel, const QSize &size,
        const QGeoMapRequestOptions &requestOptions);

    void setZoomLevel(int zoomLevel) {};
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option) {};
    void setCenter(const QGeoCoordinate &center) {};
    QGeoCoordinate center() const {};
    void pan(int startX, int startY, int endX, int endY) {};
    QGeoBoundingBox viewBounds() const {};
    QPointF coordinateToScreenPosition(const QGeoCoordinate &coordinate) const {};
    QGeoCoordinate screenPositionToCoordinate(QPointF screenPosition) const {};

private slots:
    void mappingFinished();
    void mappingError(QGeoMapReply::Error error, const QString &errorString);

};

#endif // QGEOMAPPINGMANAGER_DUMMY_H
