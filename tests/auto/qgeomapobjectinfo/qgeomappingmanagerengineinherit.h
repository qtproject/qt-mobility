#ifndef QGEOMAPPINGMANAGERENGINEINHERIT_H
#define QGEOMAPPINGMANAGERENGINEINHERIT_H

#include "qgeomappingmanagerengine.h"
#include "qgeomapdata.h"
#include "tst_qgeomapdata.h"

QTM_USE_NAMESPACE

class QGeoMappingManagerEngineInherit : public QGeoMappingManagerEngine
{
   public :
     QGeoMappingManagerEngineInherit();
     QGeoMapData* createMapData();
};


#endif // QGEOMAPPINGMANAGERENGINEINHERIT_H
