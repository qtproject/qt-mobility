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

QGeoTiledMapTextObjectInfo::QGeoTiledMapTextObjectInfo(QGeoMapData *mapData, QGeoMapObject *mapObject)
        : QGeoTiledMapObjectInfo(mapData, mapObject),
        textItem(0)

{
    text = static_cast<QGeoMapTextObject*>(mapObject);
}

QGeoTiledMapTextObjectInfo::~QGeoTiledMapTextObjectInfo() {}

void QGeoTiledMapTextObjectInfo::objectUpdated()
{
    if (!text->coordinate().isValid()) {
        if (textItem) {
            delete textItem;
            textItem = 0;
            graphicsItem = 0;
        }
        return;
    }

    if (!textItem)
        textItem = new QGraphicsSimpleTextItem();

    textItem->setText(text->text());
    textItem->setFont(text->font());
    textItem->setBrush(text->brush());

    QFontMetrics metrics(text->font());
    QRect bounds = metrics.boundingRect(text->text());

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

    QPointF position = tiledMapData->coordinateToWorldPixel(text->coordinate());
    textItem->setPos(position);

    mapUpdated();

    graphicsItem = textItem;

    updateItem();
}

void QGeoTiledMapTextObjectInfo::mapUpdated()
{
    if (textItem) {
        int zoomFactor = tiledMapData->zoomFactor();
        textItem->resetTransform();
        textItem->setScale(zoomFactor);
        QPointF offset = text->offset();
        offset += alignmentOffset;
        textItem->setTransform(QTransform::fromTranslate(
                                   offset.x() * zoomFactor,
                                   offset.y() * zoomFactor));
    }
}

QTM_END_NAMESPACE

