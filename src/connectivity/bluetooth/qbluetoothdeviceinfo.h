/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QBLUETOOTHDEVICEINFO_H
#define QBLUETOOTHDEVICEINFO_H

#include "qmobilityglobal.h"

#include <QString>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothDeviceInfoPrivate;
class QBluetoothAddress;
class QBluetoothUuid;

class Q_CONNECTIVITY_EXPORT QBluetoothDeviceInfo
{
public:
    enum MajorDeviceClass {
        MiscellaneousDevice = 0,
        ComputerDevice = 1,
        PhoneDevice = 2,
        LANAccessDevice = 3,
        AudioVideoDevice = 4,
        PeripheralDevice = 5,
        ImagingDevice = 6,
        WearableDevice = 7,
        ToyDevice = 8,
        UncategorizedDevice = 9,
    };

    enum MinorMiscellaneousClass {
        UncategorizedMiscellaneous = 0,
    };

    enum MinorComputerClass {
        UncategorizedComputer = 0,
        DesktopComputer = 1,
        ServerComputer = 2,
        LaptopComputer = 3,
        HandheldClamShellComputer = 4,
        HandheldComputer = 5,
        WearableComputer = 6,
    };

    enum MinorPhoneClass {
        UncategorizedPhone = 0,
        CellularPhone = 1,
        CordlessPhone = 2,
        SmartPhone = 3,
        WiredModemOrVoiceGatewayPhone = 4,
        CommonIsdnAccessPhone = 5,
    };

    enum MinorNetworkClass {
        NetworkFullService = 0x00,
        NetworkLoadFactorOne = 0x08,
        NetworkLoadFactorTwo = 0x10,
        NetworkLoadFactorThree = 0x18,
        NetworkLoadFactorFour = 0x20,
        NetworkLoadFactorFive = 0x28,
        NetworkLoadFactorSix = 0x30,
        NetworkNoService = 0x38,
    };

    enum MinorAudioVideoClass {
        UncategorizedAudioVideoDevice = 0,
        WearableHeadsetDevice = 1,
        HandsFreeDevice = 2,
        // reserved = 3,
        Microphone = 4,
        Loudspeaker = 5,
        Headphones = 6,
        PortableAudioDevice = 7,
        CarAudio = 8,
        SetTopBox = 9,
        HiFiAudioDevice = 10,
        Vcr = 11,
        VideoCamera = 12,
        Camcorder = 13,
        VideoMonitor = 14,
        VideoDisplayAndLoudspeaker = 15,
        VideoConferencing = 17,
        // reserved = 18,
        GamingDevice = 19,
    };

    enum MinorPeripheralClass {
        UncategorizedPeripheral = 0,
        KeyboardPeripheral = 0x10,
        PointingDevicePeripheral = 0x20,
        KeyboardWithPointingDevicePeripheral = 0x30,

        JoystickPeripheral = 0x01,
        GamepadPeripheral = 0x02,
        RemoteControlPeripheral = 0x03,
        SensingDevicePeripheral = 0x04,
        DigitizerTabletPeripheral = 0x05,
        CardReaderPeripheral = 0x06,
    };

    enum MinorImagingClass {
        UncategorizedImagingDevice = 0,
        ImageDisplay = 0x04,
        ImageCamera = 0x08,
        ImageCamera2 = 0x0C,
        ImageScanner = 0x10,
        ImageScanner2 = 0x14,
        ImageScanner3 = 0x18,
        ImageScanner4 = 0x1C,
        ImagePrinter = 0x20,
        ImagePrinter2 = 0x24,
        ImagePrinter3 = 0x28,
        ImagePrinter4 = 0x2C,
        ImagePrinter5 = 0x30,
        ImagePrinter6 = 0x34,
        ImagePrinter7 = 0x38,
        ImagePrinter8 = 0x3C,
    };

    enum MinorWearableClass {
        UncategorizedWearableDevice = 0,
        WearableWristWatch = 1,
        WearablePager = 2,
        WearableJacket = 3,
        WearableHelmet = 4,
        WearableGlasses = 5,
    };

    enum MinorToyClass {
        UncategorizedToy = 0,
        ToyRobot = 1,
        ToyVehicle = 2,
        ToyDoll = 3,
        ToyController = 4,
        ToyGame = 5,
    };

    enum ServiceClass {
        NoService = 0x0000,
        PositioningService = 0x0001,
        NetworkingService = 0x0002,
        RenderingService = 0x0004,
        CapturingService = 0x0008,
        ObjectTransferService = 0x0010,
        AudioService = 0x0020,
        TelephonyService = 0x0040,
        InformationService = 0x0080,
        AllServices = 0x07ff,
    };
    Q_DECLARE_FLAGS(ServiceClasses, ServiceClass)

    enum DataCompleteness {
        DataComplete,
        DataIncomplete,
        DataUnavailable,
    };

    QBluetoothDeviceInfo();
    QBluetoothDeviceInfo(const QBluetoothAddress &address, const QString &name, quint32 classOfDevice);
    QBluetoothDeviceInfo(const QBluetoothDeviceInfo &other);
    ~QBluetoothDeviceInfo();

    bool isValid() const;

    QBluetoothDeviceInfo &operator=(const QBluetoothDeviceInfo &other);

    QBluetoothAddress address() const;
    QString name() const;

    ServiceClasses serviceClasses() const;
    MajorDeviceClass majorDeviceClass() const;
    quint8 minorDeviceClass() const;

//    bool matchesMinorClass(MinorComputerClass minor) const;
//    bool matchesMinorClass(MinorPhoneClass minor) const;
//    bool matchesMinorClass(MinorNetworkClass minor) const;
//    bool matchesMinorClass(MinorAudioVideoClass minor) const;
//    bool matchesMinorClass(MinorPeripheralClass minor) const;
//    bool matchesMinorClass(MinorImagingClass minor) const;

    void setServiceUuids(const QList<QBluetoothUuid> &uuids, DataCompleteness completeness);
    QList<QBluetoothUuid> serviceUuids(DataCompleteness *completeness = 0) const;
    DataCompleteness serviceUuidsCompleteness() const;

    void setManufacturerSpecificData(const QByteArray &data);
    QByteArray manufacturerSpecificData(bool *available = 0) const;

protected:
    QBluetoothDeviceInfoPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QBluetoothDeviceInfo)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
