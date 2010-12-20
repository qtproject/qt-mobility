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

#include "qgeomapobjectinfo.h"
#include "qgeoboundingbox.h"
#include "qgeomapdata.h"
#include "qgeomapobject.h"

QTM_BEGIN_NAMESPACE

QGeoMapObjectInfo::QGeoMapObjectInfo(QGeoMapData *mapData, QGeoMapObject *mapObject)
{}

QGeoMapObjectInfo::~QGeoMapObjectInfo()
{}

static void deprecatedWarning()
{
    qWarning("Use of QGeoMapObjectInfo is deprecated and will be removed "
             "in the next major release.");
}

void QGeoMapObjectInfo::init()
{
    deprecatedWarning();
}

QGeoBoundingBox QGeoMapObjectInfo::boundingBox() const
{
    deprecatedWarning();
    return QGeoBoundingBox();
}

bool QGeoMapObjectInfo::contains(const QGeoCoordinate &coordinate) const
{
    deprecatedWarning();
    return false;
}

void QGeoMapObjectInfo::windowSizeChanged(const QSizeF &windowSize)
{
    Q_UNUSED(windowSize);
    deprecatedWarning();
}

void QGeoMapObjectInfo::zoomLevelChanged(qreal zoomLevel)
{
    Q_UNUSED(zoomLevel);
    deprecatedWarning();
}

void QGeoMapObjectInfo::centerChanged(const QGeoCoordinate &coordinate)
{
    Q_UNUSED(coordinate);
    deprecatedWarning();
}

void QGeoMapObjectInfo::zValueChanged(int zValue)
{
    Q_UNUSED(zValue);
    deprecatedWarning();
}

void QGeoMapObjectInfo::visibleChanged(bool visible)
{
    Q_UNUSED(visible);
    deprecatedWarning();
}

void QGeoMapObjectInfo::selectedChanged(bool selected)
{
    Q_UNUSED(selected);
    deprecatedWarning();
}

QGeoMapData* QGeoMapObjectInfo::mapData()
{
    deprecatedWarning();
    return 0;
}

QGeoMapObject* QGeoMapObjectInfo::mapObject()
{
    deprecatedWarning();
    return 0;
}

QTM_END_NAMESPACE
