#include"tst_qgeomapobjectinfoinherit.h"
#include"qgeomapdata.h"
#include"qgeomapobject.h"

QGeoMapObjectInfoInherit::QGeoMapObjectInfoInherit() :
    geomapdata(&QGeoMapDataInherit()),
    mapobject(&QGeoMapObject()),
    QGeoMapObjectInfo(geomapdata, mapobject)
{
    windowSize = QSizeF(20.22, 13.00);
    zoomLevel = 3.0;
    coordinate = QGeoCoordinate(30.22, 45.99);
    zValue = 5;
    visible = true;
    selected = true;
    origin = QGeoCoordinate(0.0, 88.23);
}
