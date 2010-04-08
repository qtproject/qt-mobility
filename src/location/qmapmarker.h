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

#ifndef QLOCATION_MAPMARKER_H
#define QLOCATION_MAPMARKER_H

#include "qmapobject.h"

QTM_BEGIN_NAMESPACE

class QMapMarkerPrivate;
class Q_LOCATION_EXPORT QMapMarker : public QMapObject
{
    friend class QMapView;

public:
    QMapMarker(const QGeoCoordinate& point,
               const QString& text = QString(),
               const QFont& font = QFont("Arial", 10, QFont::Bold),
               const QColor& fontColor = QColor(Qt::white),
               const QPixmap& icon = QPixmap(),
               const QRectF& textRect = QRectF(),
               quint16 layerIndex = 0);

    QPixmap icon() const;
    QGeoCoordinate point() const;
    QString text() const;
    QRectF textRect() const;
    QFont font() const;

protected:
    virtual bool intersects(const QRectF& tileRect) const;
    virtual void compMapCoords();
    virtual void paint(QPainter* painter, const QRectF& viewPort);

    void constructMarker(QPainter* painter, const QPointF& point);
    void constructIconMarker(QPainter* painter, const QPointF& point);

    QMapMarker(QMapMarkerPrivate &dd, const QGeoCoordinate& point,
               const QString& text, const QFont& font, const QColor& fontColor,
               const QPixmap& icon, const QRectF& textRect,
               quint16 layerIndex);
private:
    Q_DECLARE_PRIVATE(QMapMarker);
};

QTM_END_NAMESPACE

#endif
