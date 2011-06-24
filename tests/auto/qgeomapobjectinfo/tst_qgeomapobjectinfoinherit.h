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
