
#include "qgeocoordinate.h"
#include <qgeoboundingbox.h>
#include "tst_qgeomapobjectinfoinherit.h"
#include "qgeomappingmanagerengine.h"
#include "qgeomappingmanager.h"
#include "qgeomapdata.h"

QTM_USE_NAMESPACE


QGeoMapDataInherit::QGeoMapDataInherit():
                    QGeoMapData(& (QGeoMappingManagerEngineInherit()) )
{
}

QGeoBoundingBox QGeoMapDataInherit::viewport()const
{
    return (QGeoBoundingBox());
}

void QGeoMapDataInherit::fitInViewport(const QGeoBoundingBox& , bool )
{
}

 QPointF QGeoMapDataInherit::coordinateToScreenPosition(const QGeoCoordinate&) const
 {
     return (QPointF());
 }

 QGeoCoordinate QGeoMapDataInherit::screenPositionToCoordinate(const QPointF& ) const
 {
     return (QGeoCoordinate(0.0, 12.0));
 }
