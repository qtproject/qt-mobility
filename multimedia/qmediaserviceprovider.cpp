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

Q_GLOBAL_STATIC_WITH_ARGS(QMediaPluginLoader, loader,
        (QMediaServiceProviderFactoryInterface_iid, QLatin1String("/mediaservice"), Qt::CaseInsensitive))


class QPluginServiceProvider : public QMediaServiceProvider
{
    QMap<QMediaService*, QMediaServiceProviderPlugin*> pluginMap;

public:
    QMediaService* requestService(const QByteArray &type, const QList<QByteArray> &)
    {
        QString key(type);
        QMediaServiceProviderPlugin *plugin =
            qobject_cast<QMediaServiceProviderPlugin*>(loader()->instance(key));

        if (plugin != 0) {
            QMediaService *service = plugin->create(key);
            if (service != 0)
                pluginMap.insert(service, plugin);

            return service;
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

        foreach(QObject *obj, loader()->instances(serviceType)) {
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

