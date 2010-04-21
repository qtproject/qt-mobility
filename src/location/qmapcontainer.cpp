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

#include "qmapcontainer.h"
#include "qmapobject_p.h"

QTM_BEGIN_NAMESPACE

QMapContainer::QMapContainer()
{
}

QMapContainer::~QMapContainer()
{
}

bool QMapContainer::removeMapObject(QMapObject* mapObject)
{
    return mapObjects.remove(mapObject);
}


void QMapContainer::addMapObject(QMapObject* mapObject)
{
    mapObjects.insert(mapObject);
    mapObject->compMapCoords();
}

void QMapContainer::reconstructObjects(QHash<quint64, QList<QMapObject*> > &tileToObjects)
{
    QSetIterator<QMapObject*> it(mapObjects);

    while (it.hasNext()) {
        QMapObject* obj = it.next();
        obj->compMapCoords();
        //addMapObjectToTiles
        for (int i = 0; i < obj->d_ptr->intersectingTiles.count(); i++) {
            if (!tileToObjects.contains(obj->d_ptr->intersectingTiles[i]))
                tileToObjects[obj->d_ptr->intersectingTiles[i]] = QList<QMapObject*>();

            tileToObjects[obj->d_ptr->intersectingTiles[i]].append(obj);
        }

    }
}

QTM_END_NAMESPACE
