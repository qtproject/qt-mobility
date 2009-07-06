/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qioctlwifiengine_win_p.h"
#include "qnetworkconfiguration_p.h"

#include <QtCore/qstringlist.h>

#include <QtNetwork/qnetworkinterface.h>

#include <wtypes.h>
#include <ntddndis.h>
#undef interface

Q_GLOBAL_STATIC(QIoctlWifiEngine, ioctlWifiEngine)

QT_BEGIN_NAMESPACE

QIoctlWifiEngine::QIoctlWifiEngine(QObject *parent)
:   QNetworkSessionEngine(parent)
{
    // poll for changes in available WLANs
    connect(&pollTimer, SIGNAL(timeout()), this, SIGNAL(configurationsChanged()));
    pollTimer.start(10000);
}

QIoctlWifiEngine::~QIoctlWifiEngine()
{
}

QList<QNetworkConfigurationPrivate *> QIoctlWifiEngine::getConfigurations(bool *ok)
{
    if (ok)
        *ok = false;

    QList<QNetworkConfigurationPrivate *> foundConfigurations;

    unsigned long oid;
    char buffer[0x10000];
    DWORD bytesWritten;

    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces()) {
        HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(interface.name()).utf16(), 0,
                                   FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (handle == INVALID_HANDLE_VALUE)
            continue;

        // Get active access point
        oid = OID_802_11_SSID;
        bytesWritten = 0;
        bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                      buffer, sizeof(buffer), &bytesWritten, 0);
        if (!result) {
            int error = GetLastError();
            if (error != ERROR_NOT_SUPPORTED)
                qWarning("%s: DeviceIoControl failed with error %d\n", __FUNCTION__, error);
            CloseHandle(handle);
            continue;
        }

        NDIS_802_11_SSID *ssid = reinterpret_cast<NDIS_802_11_SSID *>(buffer);

        QByteArray activeSsid(reinterpret_cast<char *>(ssid->Ssid), ssid->SsidLength);

        oid = OID_802_11_BSSID_LIST;
        bytesWritten = 0;
        result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                 buffer, sizeof(buffer), &bytesWritten, 0);
        if (!result) {
            int error = GetLastError();
            if (error != ERROR_NOT_SUPPORTED)
                qWarning("%s: DeviceIoControl failed with error %d\n", __FUNCTION__, error);
            CloseHandle(handle);
            continue;
        }

        NDIS_802_11_BSSID_LIST *bssIdList = reinterpret_cast<NDIS_802_11_BSSID_LIST *>(buffer);

        char *bssIdBuffer = reinterpret_cast<char *>(bssIdList->Bssid);

        int bssIdCount = bssIdList->NumberOfItems;
        while (bssIdBuffer < buffer + bytesWritten && bssIdCount > 0) {
            NDIS_WLAN_BSSID *bssId = reinterpret_cast<NDIS_WLAN_BSSID *>(bssIdBuffer);

            QByteArray ssid(reinterpret_cast<char *>(bssId->Ssid.Ssid), bssId->Ssid.SsidLength);

            QNetworkConfigurationPrivate *cpPriv = new QNetworkConfigurationPrivate;

            cpPriv->name = ssid;
            cpPriv->isValid = true;
            cpPriv->id = QString::number(qHash(QLatin1String("IOWLAN:") + cpPriv->name));

            /*
                TODO

                This is currently wrong, need to find out how to determine if the WLAN is
                Undefined, Defined or Discovered from the system.

                Currently marking all found WLANs as Discovered.
            */
            if (!activeSsid.isEmpty() && ssid == activeSsid)
                cpPriv->state = QNetworkConfiguration::Active;
            else
                cpPriv->state = QNetworkConfiguration::Discovered;

            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;

            foundConfigurations.append(cpPriv);

            bssIdBuffer += bssId->Length;
            ++bssIdCount;
        }

        CloseHandle(handle);
    }

    if (ok)
        *ok = true;

    pollTimer.start(10000);

    return foundConfigurations;
}

QString QIoctlWifiEngine::getInterfaceFromId(const QString &id)
{
    unsigned long oid = OID_802_11_SSID;
    NDIS_802_11_SSID ssid;
    DWORD bytesWritten;

    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces()) {
        HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(interface.name()).utf16(), 0,
                                   FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (handle == INVALID_HANDLE_VALUE)
            continue;

        // Get active access point
        bytesWritten = 0;
        bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                      &ssid, sizeof(ssid), &bytesWritten, 0);
        if (!result) {
            int error = GetLastError();
            if (error != ERROR_NOT_SUPPORTED)
                qWarning("%s: DeviceIoControl failed with error %d\n", __FUNCTION__, error);
            CloseHandle(handle);
            continue;
        }

        CloseHandle(handle);

        if (ssid.SsidLength == 0)
            continue;

        QByteArray currentSsid =
            QByteArray::fromRawData(reinterpret_cast<char *>(ssid.Ssid), ssid.SsidLength);

        if (qHash(QLatin1String("IOWLAN:") + currentSsid) == id.toUInt())
            return interface.name();
    }

    return QString();
}

bool QIoctlWifiEngine::hasIdentifier(const QString &id)
{
    unsigned long oid;
    char buffer[0x10000];
    DWORD bytesWritten;

    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces()) {
        HANDLE handle = CreateFile((TCHAR *)QString("\\\\.\\%1").arg(interface.name()).utf16(), 0,
                                   FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (handle == INVALID_HANDLE_VALUE)
            continue;

        // Get active access point
        oid = OID_802_11_SSID;
        bytesWritten = 0;
        bool result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                      buffer, sizeof(buffer), &bytesWritten, 0);
        if (!result) {
            int error = GetLastError();
            if (error != ERROR_NOT_SUPPORTED)
                qWarning("%s: DeviceIoControl failed with error %d\n", __FUNCTION__, error);
            CloseHandle(handle);
            continue;
        }

        NDIS_802_11_SSID *ssid = reinterpret_cast<NDIS_802_11_SSID *>(buffer);

        QByteArray activeSsid(reinterpret_cast<char *>(ssid->Ssid), ssid->SsidLength);

        oid = OID_802_11_BSSID_LIST;
        bytesWritten = 0;
        result = DeviceIoControl(handle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid),
                                 buffer, sizeof(buffer), &bytesWritten, 0);
        if (!result) {
            int error = GetLastError();
            if (error != ERROR_NOT_SUPPORTED)
                qWarning("%s: DeviceIoControl failed with error %d\n", __FUNCTION__, error);
            CloseHandle(handle);
            continue;
        }

        NDIS_802_11_BSSID_LIST *bssIdList = reinterpret_cast<NDIS_802_11_BSSID_LIST *>(buffer);

        char *bssIdBuffer = reinterpret_cast<char *>(bssIdList->Bssid);

        int bssIdCount = bssIdList->NumberOfItems;
        while (bssIdBuffer < buffer + bytesWritten && bssIdCount > 0) {
            NDIS_WLAN_BSSID *bssId = reinterpret_cast<NDIS_WLAN_BSSID *>(bssIdBuffer);

            QByteArray ssid(reinterpret_cast<char *>(bssId->Ssid.Ssid), bssId->Ssid.SsidLength);

            if (qHash(QLatin1String("IOWLAN:") + ssid) == id.toUInt()) {
                CloseHandle(handle);
                return true;
            }

			bssIdBuffer += bssId->Length;
			--bssIdCount;
        }

        CloseHandle(handle);
    }

    return false;
}

QString QIoctlWifiEngine::bearerName(const QString &)
{
    return QLatin1String("WLAN");
}

void QIoctlWifiEngine::connectToId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QIoctlWifiEngine::disconnectFromId(const QString &id)
{
    emit connectionError(id, OperationNotSupported);
}

void QIoctlWifiEngine::requestUpdate()
{
}

QIoctlWifiEngine *QIoctlWifiEngine::instance()
{
    return ioctlWifiEngine();
}

QT_END_NAMESPACE
