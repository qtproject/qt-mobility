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

#include "qgeotiledmapgroupobjectinfo_p.h"

#include "qgeotiledmapobjectinfo_p.h"
#include "qgeotiledmapdata.h"
#include "qgeotiledmapdata_p.h"

#include "qgeomapobject.h"
#include "qgeomapgroupobject.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsPathItem>

QTM_BEGIN_NAMESPACE

QGeoTiledMapGroupObjectInfo::QGeoTiledMapGroupObjectInfo(QGeoTiledMapData *mapData, QGeoMapObject *mapObject)
    : QGeoTiledMapObjectInfo(mapData, mapObject)
{
    group = static_cast<QGeoMapGroupObject*>(mapObject);

    connect(group,
            SIGNAL(childAdded(QGeoMapObject*)),
            this,
            SLOT(childAdded(QGeoMapObject*)));
    connect(group,
            SIGNAL(childRemoved(QGeoMapObject*)),
            this,
            SLOT(childRemoved(QGeoMapObject*)));

    pathItem = new QGraphicsPathItem();
    graphicsItem = pathItem;
    pathItem->setPos(0.0, 0.0);
    updateItem();
}

QGeoTiledMapGroupObjectInfo::~QGeoTiledMapGroupObjectInfo()
{
    for (int i = 0; i < group->childObjects().size(); ++i) {
        QGeoTiledMapObjectInfo* info
                = static_cast<QGeoTiledMapObjectInfo*>(group->childObjects().at(i)->info());
        delete info->graphicsItem;
        info->graphicsItem = 0;
    }
}

void QGeoTiledMapGroupObjectInfo::childAdded(QGeoMapObject *childObject)
{
    QGeoTiledMapObjectInfo* info = static_cast<QGeoTiledMapObjectInfo*>(childObject->info());
    if (info && info->graphicsItem) {
        info->graphicsItem->setParentItem(graphicsItem);
        updateItem();
    }
}

void QGeoTiledMapGroupObjectInfo::childRemoved(QGeoMapObject *childObject)
{
    QGeoTiledMapObjectInfo* info = static_cast<QGeoTiledMapObjectInfo*>(childObject->info());
    if (info && info->graphicsItem && info->graphicsItem->scene()) {
        info->graphicsItem->scene()->removeItem(info->graphicsItem);
        updateItem();
    }
}

#include "moc_qgeotiledmapgroupobjectinfo_p.cpp"

QTM_END_NAMESPACE
