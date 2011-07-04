/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/location

#ifndef TST_QGEOMAPOBJECTINFO_H
#define TST_QGEOMAPOBJECTINFO_H

#include <QSizeF>
#include <QSignalSpy>
#include "qgeomapobjectinfo.h"
#include "qgeocoordinate.h"
#include "qgeomapobject.h"

#include <QtTest/QtTest>

#include "testhelper.h"

#include "qgraphicsgeomap.h"
#include <QGraphicsItem>
#include <QSignalSpy>
#include <qgeoboundingbox.h>
#include "qgeomappingmanagerengine.h"
#include "qgeomapdata.h"
#include "tst_qgeomapdata.h"

QTM_USE_NAMESPACE


class QGeoMapObjectInfoInherit : public QGeoMapObjectInfo
{
    QGeoMapData* geomapdata ;
    QGeoMapObject* mapobject ;

    QSizeF windowSize;
    qreal zoomLevel;
    QGeoCoordinate coordinate;
    int zValue;
    bool visible;
    bool selected;
    QGeoCoordinate origin;

public :
    QGeoMapObjectInfoInherit();

    void emitWindowSizeChanged () {  emit QBENCHMARK{windowSizeChanged(windowSize);} }
    void emitZoomLevelChanged () { QBENCHMARK {emit zoomLevelChanged(zoomLevel);} }
    void emitCenterChanged () { QBENCHMARK {emit centerChanged(coordinate);} }
    void emitZValueChanged () { QBENCHMARK {emit zValueChanged(zValue);} }
    void emitVisibleChanged () { QBENCHMARK {emit visibleChanged(visible);} }
    void emitSelectedChanged () { QBENCHMARK {emit selectedChanged(selected);} }
    void emitOriginChanged () { QBENCHMARK {emit originChanged(origin);} }
    void emitUnitsChanged () { QBENCHMARK {emit unitsChanged(QGeoMapObject::PixelUnit);} }
    void emitTransformTypeChanged () { QBENCHMARK {emit transformTypeChanged(QGeoMapObject::BilinearTransform);} }

    void callMapData() { QBENCHMARK {mapData();} }
    void callMapObject() { QBENCHMARK {mapObject();} }
};




#endif // TST_QGEOMAPOBJECTINFO_H
