
#include "pulseaudioplayerservice.h"
#include "pulseaudioserviceproviderplugin.h"


QStringList PulseaudioServiceProviderPlugin::keys() const
{
    return QStringList() << Q_MEDIASERVICE_MEDIAPLAYER;
}

QMediaService* PulseaudioServiceProviderPlugin::create(const QString &key)
{
    if (key == QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER))
        return new PulseAudioPlayerService;

    return 0;
}

void PulseaudioServiceProviderPlugin::release(QMediaService *service)
{
    delete service;
}

QMediaServiceProviderHint::Features PulseaudioServiceProviderPlugin::supportedFeatures(const QByteArray &service) const
{
    if (service == QByteArray(Q_MEDIASERVICE_MEDIAPLAYER))
        return QMediaServiceProviderHint::LowLatencyPlayback;

    return 0;
}

QtMedia::SupportEstimate PulseaudioServiceProviderPlugin::hasSupport(const QString &mimeType, const QStringList& codecs) const
{
    Q_UNUSED(codecs);

    if (mimeType == QLatin1String("audio/x-wav"))
        return QtMedia::ProbablySupported;

    return QtMedia::NotSupported;
}

QStringList PulseaudioServiceProviderPlugin::supportedMimeTypes() const
{
    return QStringList() << QLatin1String("audio/x-wav");
}


Q_EXPORT_PLUGIN2(pulseaudio_serviceplugin, PulseaudioServiceProviderPlugin);
