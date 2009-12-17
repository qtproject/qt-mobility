#ifndef PULSEAUDIOSERVICEPROVIDERPLUGIN_H
#define PULSEAUDIOSERVICEPROVIDERPLUGIN_H

#include <qmediaserviceproviderplugin.h>

QTM_USE_NAMESPACE

class PulseaudioServiceProviderPlugin :
        public QMediaServiceProviderPlugin,
        public QMediaServiceFeaturesInterface,
        public QMediaServiceSupportedFormatsInterface
{
Q_OBJECT
public:
    QStringList keys() const;
    QMediaService* create(QString const& key);
    void release(QMediaService *service);

    QMediaServiceProviderHint::Features supportedFeatures(const QByteArray &service) const;

    QtMedia::SupportEstimate hasSupport(const QString &mimeType, const QStringList& codecs) const;
    QStringList supportedMimeTypes() const;
};

#endif // PULSEAUDIOSERVICEPROVIDERPLUGIN_H
