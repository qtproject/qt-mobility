/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtCore/qdebug.h>
#include <QtCore/qmap.h>

#include <multimedia/qmediaservice.h>
#include <multimedia/qmediaserviceprovider.h>
#include <multimedia/qmediaserviceproviderplugin.h>
#include <multimedia/qmediapluginloader_p.h>
#include <multimedia/qmediaplayer.h>

class QMediaServiceProviderHintPrivate : public QSharedData
{
public:
    QMediaServiceProviderHintPrivate(QMediaServiceProviderHint::Type type)
        :type(type), features(0)
    {
    }

    QMediaServiceProviderHintPrivate(const QMediaServiceProviderHintPrivate &other)
        :QSharedData(other),
        type(other.type),
        device(other.device),
        mimeType(other.mimeType),
        codecs(other.codecs),
        features(other.features)
    {
    }

    ~QMediaServiceProviderHintPrivate()
    {
    }

    QMediaServiceProviderHint::Type type;
    QByteArray device;
    QString mimeType;
    QStringList codecs;
    QMediaServiceProviderHint::Features features;
};

/*!
  \class QMediaServiceProviderHint
*/

/*!
    \enum QMediaServiceProviderHint::Feature

    \value LowLatencyPlayback
            The service is expected to play simple audio formats,
            but playback should start without significant delay.
            Such playback service can be used for beeps, ringtones, etc.

    \value RecordingSupport
            The service provides audio or video recording functions.
*/

/*!
    \enum QMediaServiceProviderHint::Type

    \value Null               En empty hint, use the default service.
    \value ContentType        Select media service most suitable for certain content type.
    \value Device             Select media service which supports certain device.
    \value SupportedFeatures  Select media service supporting the set of optional features.
*/


/*!
  Construct an empty media service provider hint.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint()
    :d(new QMediaServiceProviderHintPrivate(Null))
{
}

/*!
  Constructs the content type related media service provider hint.
  Passing this hint to the service provider allows it to choose the service
  suitable to play content with certain mime \a type and \a codecs list.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(const QString &type, const QStringList& codecs)
    :d(new QMediaServiceProviderHintPrivate(ContentType))
{
    d->mimeType = type;
    d->codecs = codecs;
}

/*!
  Constructs the device related media service provider hint.
  Passing this hint to the service provider allows it to choose the service
  able to work with \a device.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(const QByteArray &device)
    :d(new QMediaServiceProviderHintPrivate(Device))
{
    d->device = device;
}

/*!
  Constructs the supported features related media service provider hint.
  Passing this hint to the service provider allows it to choose the service
  providing optional \a features, like recording support or low latency playback.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(QMediaServiceProviderHint::Features features)
    :d(new QMediaServiceProviderHintPrivate(SupportedFeatures))
{
    d->features = features;
}

/*!
  Constructs a copy of \a other.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(const QMediaServiceProviderHint &other)
    :d(other.d)
{
}

/*!
  Destroys the service provider hint.
*/
QMediaServiceProviderHint::~QMediaServiceProviderHint()
{
}

/*!
  Assigns \a other to this service provider hint and returns a reference to this hint.
*/
QMediaServiceProviderHint& QMediaServiceProviderHint::operator=(const QMediaServiceProviderHint &other)
{
    d = other.d;
    return *this;
}

/*!
  Returns true if this hint is equal to \a other; otherwise returns false.
*/
bool QMediaServiceProviderHint::operator == (const QMediaServiceProviderHint &other) const
{
    return (d == other.d) ||
           (d->type == other.d->type &&
            d->device == other.d->device &&
            d->mimeType == other.d->mimeType &&
            d->codecs == other.d->codecs &&
            d->features == other.d->features);
}

/*!
  Returns true if this hint is not equal to \a other; otherwise returns false.
*/
bool QMediaServiceProviderHint::operator != (const QMediaServiceProviderHint &other) const
{
    return !(*this == other);
}

/*!
  Returns true if this hint null.
*/
bool QMediaServiceProviderHint::isNull() const
{
    return d->type == Null;
}

/*!
  Returns hint type.
*/
QMediaServiceProviderHint::Type QMediaServiceProviderHint::type() const
{
    return d->type;
}

/*!
  Returns mime type of media the service is expected to play.
*/
QString QMediaServiceProviderHint::mimeType() const
{
    return d->mimeType;
}

/*!
  Returns the codecs list of media the service is expected to play.
*/
QStringList QMediaServiceProviderHint::codecs() const
{
    return d->codecs;
}

/*!
  Returns the device name, the service is expected to work with.
*/
QByteArray QMediaServiceProviderHint::device() const
{
    return d->device;
}

/*!
  Returns the set of features, the service should provide.
*/
QMediaServiceProviderHint::Features QMediaServiceProviderHint::features() const
{
    return d->features;
}


Q_GLOBAL_STATIC_WITH_ARGS(QMediaPluginLoader, loader,
        (QMediaServiceProviderFactoryInterface_iid, QLatin1String("/mediaservice"), Qt::CaseInsensitive))


class QPluginServiceProvider : public QMediaServiceProvider
{
    QMap<QMediaService*, QMediaServiceProviderPlugin*> pluginMap;

public:
    QMediaService* requestService(const QByteArray &type, const QMediaServiceProviderHint &hint)
    {
        QString key(type);

        QList<QMediaServiceProviderPlugin *>plugins;
        foreach (QObject *obj, loader()->instances(key)) {
            QMediaServiceProviderPlugin *plugin =
                qobject_cast<QMediaServiceProviderPlugin*>(obj);
            if (plugin)
                plugins << plugin;
        }

        if (!plugins.isEmpty()) {
            QMediaServiceProviderPlugin *plugin = 0;

            switch (hint.type()) {
            case QMediaServiceProviderHint::Null:
                plugin = plugins[0];
                break;
            case QMediaServiceProviderHint::SupportedFeatures:
                plugin = plugins[0];
                break;
            case QMediaServiceProviderHint::Device: {
                    foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                        QMediaServiceSupportedDevicesInterface *iface =
                                qobject_cast<QMediaServiceSupportedDevicesInterface*>(currentPlugin);

                        if (!iface) {
                            // the plugin may support the device,
                            // but this choice still can be overridden
                            plugin = currentPlugin;
                        } else {
                            if (iface->devices(type).contains(hint.device())) {
                                plugin = currentPlugin;
                                break;
                            }
                        }
                    }
                }
                break;
            case QMediaServiceProviderHint::ContentType: {
                    QtMedia::SupportEstimate estimate = QtMedia::NotSupported;
                    foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                        QtMedia::SupportEstimate currentEstimate = QtMedia::MaybeSupported;
                        QMediaServiceSupportedFormatsInterface *iface =
                                qobject_cast<QMediaServiceSupportedFormatsInterface*>(currentPlugin);

                        if (iface)
                            currentEstimate = iface->hasSupport(hint.mimeType(), hint.codecs());

                        if (currentEstimate > estimate) {
                            estimate = currentEstimate;
                            plugin = currentPlugin;

                            if (currentEstimate == QtMedia::PreferedService)
                                break;
                        }
                    }
                }
                break;
            }

            if (plugin != 0) {
                QMediaService *service = plugin->create(key);
                if (service != 0)
                    pluginMap.insert(service, plugin);

                return service;
            }
        }

        qWarning() << "defaultServiceProvider::requestService(): no service found for -" << key;
        return 0;
    }

    void releaseService(QMediaService *service)
    {
        if (service != 0) {
            QMediaServiceProviderPlugin *plugin = pluginMap.take(service);

            if (plugin != 0)
                plugin->release(service);
        }
    }

    QtMedia::SupportEstimate hasSupport(const QByteArray &serviceType,
                                     const QString &mimeType,
                                     const QStringList& codecs,
                                     int flags) const
    {
        QList<QObject*> instances = loader()->instances(serviceType);

        if (instances.isEmpty())
            return QtMedia::NotSupported;

        bool allServicesProvideInterface = true;
        QtMedia::SupportEstimate supportEstimate = QtMedia::NotSupported;

        foreach(QObject *obj, instances) {
            QMediaServiceSupportedFormatsInterface *iface =
                    qobject_cast<QMediaServiceSupportedFormatsInterface*>(obj);

            //if low latency playback was asked, skip services known
            //not to provide low latency playback
            if (flags & QMediaPlayer::LowLatency) {
                QMediaServiceFeaturesInterface *iface =
                        qobject_cast<QMediaServiceFeaturesInterface*>(obj);

                if (iface && !(iface->supportedFeatures(serviceType) & QMediaServiceProviderHint::LowLatencyPlayback))
                    continue;
            }

            if (iface)
                supportEstimate = qMax(supportEstimate, iface->hasSupport(mimeType, codecs));
            else
                allServicesProvideInterface = false;
        }

        //don't return PreferedService
        supportEstimate = qMin(supportEstimate, QtMedia::ProbablySupported);

        //Return NotSupported only if no services are available of serviceType
        //or all the services returned NotSupported, otherwise return at least MaybeSupported
        return allServicesProvideInterface ? supportEstimate : QtMedia::MaybeSupported;
    }

    QList<QByteArray> devices(const QByteArray &serviceType) const
    {
        QList<QByteArray> res;

        foreach(QObject *obj, loader()->instances(serviceType)) {
            QMediaServiceSupportedDevicesInterface *iface =
                    qobject_cast<QMediaServiceSupportedDevicesInterface*>(obj);

            if (iface) {
                res.append(iface->devices(serviceType));
            }
        }

        return res;
    }

    QString deviceDescription(const QByteArray &serviceType, const QByteArray &device)
    {
        foreach(QObject *obj, loader()->instances(serviceType)) {
            QMediaServiceSupportedDevicesInterface *iface =
                    qobject_cast<QMediaServiceSupportedDevicesInterface*>(obj);

            if (iface) {
                if (iface->devices(serviceType).contains(device))
                    return iface->deviceDescription(serviceType, device);
            }
        }

        return QString();
    }
};

Q_GLOBAL_STATIC(QPluginServiceProvider, pluginProvider);

/*!
    \class QMediaServiceProvider
    \preliminary
    \brief The QMediaServiceProvider class provides an abstract allocator for media services.
*/

/*!
    \fn QMediaServiceProvider::requestService(const QByteArray &type, const QMediaServiceProviderHint &hint)

    Requests an instance of a \a type service which best matches the given \a hint.

    Returns a pointer to the requested service, or a null pointer if there is no suitable service.

    The returned service must be released with releaseService when it is finished with.
*/

/*!
    \fn QMediaServiceProvider::releaseService(QMediaService *service)

    Releases a media \a service requested with requestService().
*/

/*!
    \fn QtMedia::SupportEstimate QMediaServiceProvider::hasSupport(const QByteArray &serviceType, const QString &mimeType, const QStringList& codecs, int flags) const

    Returns how confident the available service is that it can play media resources of the given
    \a mimeType with content encoded with \a codecs. If multiple services with the same
    \a serviceType are available, the result from the most confident one is returned.
*/
QtMedia::SupportEstimate QMediaServiceProvider::hasSupport(const QByteArray &serviceType,
                                                        const QString &mimeType,
                                                        const QStringList& codecs,
                                                        int flags) const
{
    Q_UNUSED(serviceType);
    Q_UNUSED(mimeType);
    Q_UNUSED(codecs);
    Q_UNUSED(flags);

    return QtMedia::MaybeSupported;
}

/*!
  Returns the list of devices related to \a service type.
*/
QList<QByteArray> QMediaServiceProvider::devices(const QByteArray &service) const
{
    Q_UNUSED(service);
    return QList<QByteArray>();
}

/*!
  Returns the description of \a device related to \a serviceType,
  suitable to be displayed to user.
*/
QString QMediaServiceProvider::deviceDescription(const QByteArray &serviceType, const QByteArray &device)
{
    Q_UNUSED(serviceType);
    Q_UNUSED(device);
    return QString();
}

/*!
    Returns a default provider of media services.
*/
QMediaServiceProvider *QMediaServiceProvider::defaultServiceProvider()
{
    return pluginProvider();
}

/*!
    \class QMediaServiceProviderPlugin
    \preliminary
    \brief The QMediaServiceProviderPlugin interface provides an interface for QMediaService
    plug-ins.

    A media service provider plug-in may implement one or more of
    QMediaServiceSupportedFormatsInterface, QMediaServiceSupportedDevicesInterface,
    and QMediaServiceFeaturesInterface to identify the features it supports.
*/

/*!
    \fn QMediaServiceProviderPlugin::keys() const

    Returns a list of keys for media services a plug-in can create.
*/

/*!
    \fn QMediaServiceProviderPlugin::create(const QString &key)

    Constructs a new instance of the QMediaService identified by \a key.

    The QMediaService returned must be destroyed with release().
*/

/*!
    \fn QMediaServiceProviderPlugin::release(QMediaService *service)

    Destroys a media \a service constructed with create().
*/


/*!
    \class QMediaServiceSupportedFormatsInterface
    \brief The QMediaServiceSupportedFormatsInterface interface identifies if a media service
    plug-in supports a media format.

    A QMediaServiceProviderPlugin may implement this interface.
*/

/*!
    \fn QMediaServiceSupportedFormatsInterface::~QMediaServiceSupportedFormatsInterface()

    Destroys a media service supported formats interface.
*/

/*!
    \fn QMediaServiceSupportedFormatsInterface::hasSupport(const QString &mimeType, const QStringList& codecs) const

    Returns the level of support a media service plug-in has for a \a mimeType and set of \a codecs.
*/

/*!
    \class QMediaServiceSupportedDevicesInterface
    \brief The QMediaServiceSupportedDevicesInterface interface identifies the devices supported
    by a media service plug-in.

    A QMediaServiceProviderPlugin may implement this interface.
*/

/*!
    \fn QMediaServiceSupportedDevicesInterface::~QMediaServiceSupportedDevicesInterface()

    Destroys a media service supported devices interface.
*/

/*!
    \fn QMediaServiceSupportedDevicesInterface::devices(const QByteArray &service) const

    Returns a list of devices supported by a plug-in \a service.
*/

/*!
    \fn QMediaServiceSupportedDevicesInterface::deviceDescription(const QByteArray &service, const QByteArray &device)

    Returns a description of a \a device supported by a plug-in \a service.
*/

/*!
    \class QMediaServiceFeaturesInterface
    \brief The QMediaServiceFeaturesInterface interface identifies features supported by a media
    service plug-in.

    A QMediaServiceProviderPlugin may implement this interface.
*/

/*!
    \fn QMediaServiceFeaturesInterface::~QMediaServiceFeaturesInterface()

    Destroys a media service features interface.
*/
/*!
    \fn QMediaServiceFeaturesInterface::supportedFeatures(const QByteArray &service) const

    Returns a set of features supported by a plug-in \a service.
*/
