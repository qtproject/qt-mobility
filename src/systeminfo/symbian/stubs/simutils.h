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

#ifndef __SIMUTILS_H__
#define __SIMUTILS_H__

//NOTE: This file is a stub containing only the constants that are needed to
//build QSystemDeviceInfo against public S60 SDKs.

const TInt KSimUtilsEnumerationFirstValue = 100;

/**
* Indicates the current SIM card status.
*/
const TUint32 KPSSimStatus = 0x00000031;
enum TPSSimStatus {
    ESimStatusUninitialized = KSimUtilsEnumerationFirstValue,
    ESimUsable,         // The Sim card is fully usable.
    ESimReadable,       // The SIM card is not fully usable, but the emergency number can be read.
    ESimNotReady,       // The Sim card is present but not ready or usable.
    ESimNotPresent,     // The Sim card is not present.
    ESimNotSupported    // SIM/RUIM is not supported. Some CDMA phones do not support a SIM/RUIM at all.
    };

#endif // __SIMUTILS_H__
