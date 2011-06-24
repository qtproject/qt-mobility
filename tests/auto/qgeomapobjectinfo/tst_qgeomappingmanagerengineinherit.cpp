#include "tst_qgeomapobjectinfoinherit.h"
#include "qgeomappingmanagerengine.h"
#include "qgeomappingmanager.h"
#include "qgeomapdata.h"

QTM_USE_NAMESPACE

QGeoMappingManagerEngineInherit::QGeoMappingManagerEngineInherit():
                                 QGeoMappingManagerEngine( QMap<QString, QVariant>())
{
}

QGeoMapData* QGeoMappingManagerEngineInherit::createMapData()
{
    return ( & (QGeoMapDataInherit ()) );
}
