/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtCore/qdebug.h>
#include <QtCore/qmap.h>

#include <multimedia/qmediaservice.h>
#include <multimedia/qmediaserviceprovider.h>
#include <multimedia/qmediaserviceproviderplugin.h>
#include <multimedia/qmediapluginloader_p.h>

class QMediaServiceProviderHintPrivate : public QSharedData
{
public:
    QMediaServiceProviderHintPrivate(QMediaServiceProviderHint::Type type)
        :type(type)
    {
    }

    QMediaServiceProviderHintPrivate(const QMediaServiceProviderHintPrivate &other)
        :QSharedData(other),
        type(other.type),
        device(other.device),
        mimeType(other.mimeType),
        codecs(other.codecs)
    {
    }

    ~QMediaServiceProviderHintPrivate()
    {
    }

    QMediaServiceProviderHint::Type type;
    QByteArray device;
    QString mimeType;
    QStringList codecs;
    QList<QByteArray> controls;
};

/*!
  \class QMediaServiceProviderHint
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
  Constructs the supported controls related media service provider hint.
  Passing this hint to the service provider allows it to choose the service
  providing optional controls.
*/
QMediaServiceProviderHint::QMediaServiceProviderHint(const QList<QByteArray> &controls)
    :d(new QMediaServiceProviderHintPrivate(SupportedControls))
{
    d->controls = controls;
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
            d->codecs == other.d->codecs);
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
            case QMediaServiceProviderHint::SupportedControls:
                plugin = plugins[0];
                break;
            case QMediaServiceProviderHint::Device: {
                    foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                        QMediaServiceProviderSupportedDevicesInterface *iface =
                                qobject_cast<QMediaServiceProviderSupportedDevicesInterface*>(plugin);

                        if (!iface) {
                            // the plugin may support the device,
                            // but this choice still can be overridden
                            plugin = currentPlugin;
                        } else {
                            if (iface->devices().contains(hint.device())) {
                                plugin = currentPlugin;
                                break;
                            }
                        }
                    }
                }
                break;
            case QMediaServiceProviderHint::ContentType: {
                    SupportEstimate estimate = NotSupported;
                    foreach (QMediaServiceProviderPlugin *currentPlugin, plugins) {
                        SupportEstimate currentEstimate = MaybeSupported;
                        QMediaServiceProviderSupportedFormatsInterface *iface =
                                qobject_cast<QMediaServiceProviderSupportedFormatsInterface*>(plugin);

                        if (iface)
                            currentEstimate = iface->canPlay(hint.mimeType(), hint.codecs());

                        if (currentEstimate > estimate) {
                            estimate = currentEstimate;
                            plugin = currentPlugin;

                            if (currentEstimate == PreferedService)
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

    QMediaServiceProvider::SupportEstimate canPlay(const QByteArray &serviceType,
                                                   const QString &mimeType,
                                                   const QStringList& codecs) const
    {
        bool found = false;
        QMediaServiceProvider::SupportEstimate supportEstimate = NotSupported;

        QList<QObject*> instances = loader()->instances(serviceType);

        if (instances.isEmpty())
            return NotSupported;

        foreach(QObject *obj, instances) {
            QMediaServiceProviderSupportedFormatsInterface *iface =
                    qobject_cast<QMediaServiceProviderSupportedFormatsInterface*>(obj);

            if (iface) {
                found = true;
                supportEstimate = qMax(supportEstimate, iface->canPlay(mimeType, codecs));
            }
        }

        //don't return PreferedService
        supportEstimate = qMin(supportEstimate, ProbablySupported);

        return found ? supportEstimate : MaybeSupported;
    }

    QList<QByteArray> devices(const QByteArray &serviceType) const
    {
        QList<QByteArray> res;

        foreach(QObject *obj, loader()->instances(serviceType)) {
            QMediaServiceProviderSupportedDevicesInterface *iface =
                    qobject_cast<QMediaServiceProviderSupportedDevicesInterface*>(obj);

            if (iface) {
                res.append(iface->devices());
            }
        }

        return res;
    }

    QString deviceDescription(const QByteArray &serviceType, const QByteArray &device)
    {
        foreach(QObject *obj, loader()->instances(serviceType)) {
            QMediaServiceProviderSupportedDevicesInterface *iface =
                    qobject_cast<QMediaServiceProviderSupportedDevicesInterface*>(obj);

            if (iface) {
                if (iface->devices().contains(device))
                    return iface->deviceDescription(device);
            }
        }

        return QString();
    }
};

Q_GLOBAL_STATIC(QPluginServiceProvider, pluginProvider);

/*!
  \enum QMediaServiceProvider::SupportEstimate

  \item NotSupported
  \item MaybeSupported
  \item ProbablySupported
  \item PreferedService
*/

/*!
  \fn QMediaServiceProvider::canPlay(const QString &type, const QStringList& codecs) const

  Returns how confident the available service is that it can play media resources of the given mime \a type
  with content encoded with \a codecs. If multiple services with the same \a serviceType are
  available, the result from the most confident one is returned.
*/
QMediaServiceProvider::SupportEstimate QMediaServiceProvider::canPlay(const QByteArray &serviceType,
                                                                      const QString &mimeType,
                                                                      const QStringList& codecs) const
{
    Q_UNUSED(serviceType);
    Q_UNUSED(mimeType);
    Q_UNUSED(codecs);

    return MaybeSupported;
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
  Returns the description of \a device related to \a service type,
  suitable to be displayed to user.
*/
QString QMediaServiceProvider::deviceDescription(const QByteArray &serviceType, const QByteArray &device)
{
    Q_UNUSED(serviceType);
    Q_UNUSED(device);
    return QString();
}


QMediaServiceProvider *QMediaServiceProvider::defaultServiceProvider()
{
    return pluginProvider();
}

