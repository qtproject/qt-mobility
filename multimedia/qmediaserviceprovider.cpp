
#include "qmediaserviceprovider.h"
#include "qmediaserviceproviderfactory_p.h"

QMediaServiceProvider* defaultServiceProvider(const char *type)
{
    return QMediaServiceProviderFactory::defaultServiceProvider(QLatin1String(type));
}

