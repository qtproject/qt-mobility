
#include "qmediaserviceprovider.h"
#include "qmediaproviderfactory_p.h"

QMediaServiceProvider *defaultServiceProvider(const char *type)
{
    return QMediaProviderFactory::defaultServiceProvider(QLatin1String(type));
}

