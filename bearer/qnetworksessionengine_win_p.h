/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact:  Nokia Corporation (qt-info@nokia.com)**
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
** will be met:  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNETWORKSESSIONENGINE_WIN_P_H
#define QNETWORKSESSIONENGINE_WIN_P_H

#ifdef Q_OS_WIN
enum NDIS_MEDIUM {
    NdisMedium802_3 = 0,
};

enum NDIS_PHYSICAL_MEDIUM {
    NdisPhysicalMediumWirelessLan = 1,
    NdisPhysicalMediumBluetooth = 10,
    NdisPhysicalMediumWiMax = 12,
};

#define OID_GEN_MEDIA_SUPPORTED 0x00010103
#define OID_GEN_PHYSICAL_MEDIUM 0x00010202

#define IOCTL_NDIS_QUERY_GLOBAL_STATS \
    CTL_CODE(FILE_DEVICE_PHYSICAL_NETCARD, 0, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#endif

#endif // QNETWORKSESSIONENGINE_WIN_P_H
