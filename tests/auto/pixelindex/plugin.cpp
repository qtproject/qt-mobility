#include "pixelindexengine.h"
#include <qgeoserviceproviderfactory.h>
#include <QObject>

#include <QtPlugin>

class PixelIndexPlugin: public QObject, public QGeoServiceProviderFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QGeoServiceProviderFactory)
public:
    PixelIndexPlugin();
    ~PixelIndexPlugin();

    QString providerName() const;
    int providerVersion() const;

    QGeoMappingManagerEngine* createMappingManagerEngine(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) const;

};

PixelIndexPlugin::PixelIndexPlugin()
{}

PixelIndexPlugin::~PixelIndexPlugin()
{}

QString PixelIndexPlugin::providerName() const
{
    return "pixelindex.plugin";
}

int PixelIndexPlugin::providerVersion() const
{
    return 1;
}

QGeoMappingManagerEngine* PixelIndexPlugin::createMappingManagerEngine(const QMap<QString, QVariant> &parameters,
    QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);
    return new PixelIndexEngine(parameters);
}

Q_EXPORT_PLUGIN2(qtgeoservices_pixelindexplugin, PixelIndexPlugin)

#include "plugin.moc"
