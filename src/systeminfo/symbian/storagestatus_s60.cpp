/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "storagestatus_s60.h"
#include <QChar>
#include <QString>
#ifndef SYMBIAN_3_1
#include <driveinfo.h>
#endif //SYMBIAN_3_1
#include <f32file.h>


CMMCStorageStatus::CMMCStorageStatus() : CActive(EPriorityStandard),
m_previousDriveList(TDriveList())
{
    CActiveScheduler::Add(this);
    if (iFs.Connect() == KErrNone) {
#ifndef SYMBIAN_3_1
        m_previousDriveList.Copy(PopulateDriveList());
#endif //SYMBIAN_3_1
        startMonitoring();
    }
}

CMMCStorageStatus::~CMMCStorageStatus()
{
    Cancel();
    iFs.Close();
}

void CMMCStorageStatus::addObserver(MStorageStatusObserver *observer)
{
    m_observers.append(observer);
}

void CMMCStorageStatus::removeObserver(MStorageStatusObserver *observer)
{
    m_observers.removeOne(observer);
}

void CMMCStorageStatus::DoCancel()
{
    iFs.NotifyChangeCancel();
}
#ifndef SYMBIAN_3_1
TDriveList CMMCStorageStatus::PopulateDriveList()
{
    TDriveList driveList;
    TInt driveCount = 0;
    if (DriveInfo::GetUserVisibleDrives(iFs, driveList, driveCount,
            KDriveAttExclude | KDriveAttRemote | KDriveAttRom | KDriveAttInternal) != KErrNone) {
        return TDriveList();
    }

    for (int i = 0; i < KMaxDrives; i++) {
        if (driveList[i] == KDriveAbsent)
            continue;

        TUint driveStatus;
        if (DriveInfo::GetDriveStatus(iFs, i, driveStatus) == KErrNone) {

            if (!(driveStatus & DriveInfo::EDriveRemovable)) {
                driveList[i] = KDriveAbsent;
                continue;
            }

            TDriveInfo driveInfo;
            if (iFs.Drive(driveInfo, i) == KErrNone) {
                if (driveInfo.iType == EMediaNotPresent) {
                    driveList[i] = KDriveAbsent;
                }
            }
        }
    }
    return driveList;
}

void CMMCStorageStatus::CompareDriveLists(const TDriveList &aDriveList)
{
    if (!(aDriveList.Length() > KMaxDrives - 1) || !(m_previousDriveList.Length() > KMaxDrives - 1))
        return;

    for (int i = 0; i < KMaxDrives; i++) {

        if (aDriveList[i] == KDriveAbsent && m_previousDriveList[i] == KDriveAbsent) {
            continue;
        } else if (aDriveList[i] > KDriveAbsent && m_previousDriveList[i] > KDriveAbsent) {
            continue;
        } else if (aDriveList[i] == KDriveAbsent && m_previousDriveList[i] > KDriveAbsent) {
            TChar volumeChar;
            QString volume;
            bool driveInserted = false;
            if (iFs.DriveToChar(i, volumeChar) == KErrNone)
                volume = QChar(volumeChar).toAscii();

            foreach (MStorageStatusObserver *observer, m_observers)
                observer->storageStatusChanged(driveInserted, volume);
            break;
        } else if (aDriveList[i] > KDriveAbsent && m_previousDriveList[i] == KDriveAbsent) {
            TChar volumeChar;
            QString volume;
            bool driveInserted = true;
            if (iFs.DriveToChar(i, volumeChar) == KErrNone)
                volume = QChar(volumeChar).toAscii();

            foreach (MStorageStatusObserver *observer, m_observers)
                observer->storageStatusChanged(driveInserted, volume);
            break;
        }
    }
    m_previousDriveList.Copy(aDriveList);
}
#endif //SYMBIAN_3_1

void CMMCStorageStatus::RunL()
{
#ifdef SYMBIAN_3_1
    TDriveInfo driveInfo;
    TDriveNumber driveLetter = EDriveE;  // Have to use hardcoded MMC drive letter for 3.1
    if (iFs.Drive(driveInfo, driveLetter) == KErrNone) {
        bool driveInserted = false;

        switch (driveInfo.iType) {
        case EMediaNotPresent:
            driveInserted = false;
            break;
        default:
            driveInserted = true;
            break;
        }

        TChar volumeChar;
        QString volume;
        if (RFs::DriveToChar(driveLetter, volumeChar) == KErrNone)
            volume = QChar(volumeChar).toAscii();

        foreach (MStorageStatusObserver *observer, m_observers)
            observer->storageStatusChanged(driveInserted, volume);
    }
#else // SYMBIAN_3_1
    CompareDriveLists(PopulateDriveList());
#endif // SYMBIAN_3_1
    startMonitoring();
}

void CMMCStorageStatus::startMonitoring()
{
    iFs.NotifyChange(ENotifyDisk, iStatus);
    SetActive();
}
