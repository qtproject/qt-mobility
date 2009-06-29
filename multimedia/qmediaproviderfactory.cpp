
#include "qmediaproviderfactory_p.h"
#include "qmediaserviceproviderplugin.h"

#include "qmediaserviceprovider.h"

#include <private/qfactoryloader_p.h>

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
        (QMediaServiceProviderFactoryInterface_iid, QLatin1String("/mediaservice"), Qt::CaseInsensitive))


QMediaServiceProvider* QMediaProviderFactory::defaultServiceProvider(QString const &key)
{
    return qobject_cast<QMediaServiceProvider*>(loader()->instance(key));
}

