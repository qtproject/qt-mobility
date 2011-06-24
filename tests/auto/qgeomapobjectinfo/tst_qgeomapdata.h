#ifndef QGEOMAPOBJECTINHERIT_H
#define QGEOMAPOBJECTINHERIT_H

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
#include "tst_qgeomapobjectinfoinherit.h"
#include "qgeomappingmanagerengineinherit.h"

QTM_USE_NAMESPACE

class QGeoMapDataInherit : public QGeoMapData
{
    QGeoMappingManagerEngineInherit manager;
 public :
    QGeoMapDataInherit();
    QGeoBoundingBox viewport() const;
    void fitInViewport(const QGeoBoundingBox&, bool) ;
    QPointF coordinateToScreenPosition(const QGeoCoordinate&) const;
    QGeoCoordinate screenPositionToCoordinate(const QPointF& ) const;
};

#endif // QGEOMAPOBJECTINHERIT_H
