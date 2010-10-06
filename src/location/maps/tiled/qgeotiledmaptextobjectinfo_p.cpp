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

#include "qgeotiledmaptextobjectinfo_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomaptextobject.h"

#include <QFontMetrics>

QTM_BEGIN_NAMESPACE

QGeoTiledMapTextObjectInfo::QGeoTiledMapTextObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject),
      textItem(0)

{
    text = static_cast<QGeoMapTextObject*>(mapObject);

    connect(text,
            SIGNAL(coordinateChanged(QGeoCoordinate)),
            this,
            SLOT(coordinateChanged(QGeoCoordinate)));
    connect(text,
            SIGNAL(textChanged(QString)),
            this,
            SLOT(textChanged(QString)));
    connect(text,
            SIGNAL(fontChanged(QFont)),
            this,
            SLOT(fontChanged(QFont)));
    connect(text,
            SIGNAL(penChanged(QPen)),
            this,
            SLOT(penChanged(QPen)));
    connect(text,
            SIGNAL(brushChanged(QBrush)),
            this,
            SLOT(brushChanged(QBrush)));
    connect(text,
            SIGNAL(offsetChanged(QPoint)),
            this,
            SLOT(offsetChanged(QPoint)));
    connect(text,
            SIGNAL(alignmentChanged(Qt::Alignment)),
            this,
            SLOT(alignmentChanged(Qt::Alignment)));

    textItem = new QGraphicsSimpleTextItem();
    graphicsItem = textItem;

    penChanged(text->pen());
    brushChanged(text->brush());
    coordinateChanged(text->coordinate());
    fontChanged(text->font());
    textChanged(text->text());

    updateValidity();
    if (valid())
        update();
}

QGeoTiledMapTextObjectInfo::~QGeoTiledMapTextObjectInfo() {}

void QGeoTiledMapTextObjectInfo::updateValidity()
{
    setValid((text->coordinate().isValid() && !text->text().isEmpty()));
}

void QGeoTiledMapTextObjectInfo::coordinateChanged(const QGeoCoordinate &coordinate)
{
    updateValidity();
    if (text->coordinate().isValid())
        textItem->setPos(tiledMapData->coordinateToWorldReferencePosition(text->coordinate()));
    if (valid())
        updateItem();
}

void QGeoTiledMapTextObjectInfo::textChanged(const QString &text)
{
    updateValidity();
    if (!this->text->text().isEmpty()) {
        textItem->setText(this->text->text());
        update();
    }
    if (valid())
        updateItem();
}

void QGeoTiledMapTextObjectInfo::fontChanged(const QFont &font)
{
    textItem->setFont(text->font());
    update();
}

void QGeoTiledMapTextObjectInfo::offsetChanged(const QPoint &offset)
{
    update();
}

void QGeoTiledMapTextObjectInfo::alignmentChanged(Qt::Alignment alignment)
{
    update();
}

void QGeoTiledMapTextObjectInfo::penChanged(const QPen &pen)
{
    textItem->setPen(text->pen());
    updateItem();
}

void QGeoTiledMapTextObjectInfo::brushChanged(const QBrush &brush)
{
    textItem->setBrush(text->brush());
    updateItem();
}

void QGeoTiledMapTextObjectInfo::zoomLevelChanged(qreal zoomLevel)
{
    update();
}

void QGeoTiledMapTextObjectInfo::update()
{
    QFontMetrics metrics(text->font());
    QRect bounds = metrics.boundingRect(text->text());
    QPointF alignmentOffset;

    if (text->alignment() & Qt::AlignLeft) {
        alignmentOffset.setX(0);
    } else if (text->alignment() & Qt::AlignHCenter) {
        alignmentOffset.setX((-bounds.width()) / 2.0);
    } else if (text->alignment() & Qt::AlignRight) {
        alignmentOffset.setX(-bounds.width());
    }

    if (text->alignment() & Qt::AlignTop) {
        alignmentOffset.setY(0);
    } else if (text->alignment() & Qt::AlignVCenter) {
        alignmentOffset.setY((-bounds.height()) / 2.0);
    } else if (text->alignment() & Qt::AlignBottom) {
        alignmentOffset.setY(-bounds.height());
    }

    int zoomFactor = tiledMapData->zoomFactor();
    textItem->resetTransform();
    textItem->setScale(zoomFactor);
    QPointF offset = text->offset();
    offset += alignmentOffset;
    textItem->setTransform(QTransform::fromTranslate(
                               offset.x() * zoomFactor,
                               offset.y() * zoomFactor));

    updateItem();
}

#include "moc_qgeotiledmaptextobjectinfo_p.cpp"

QTM_END_NAMESPACE

