/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <driveinfo.h>
#include "storagedisknotifier_s60.h"



CStorageDiskNotifier* CStorageDiskNotifier::NewL()
    {    
    CStorageDiskNotifier* self = new(ELeave) CStorageDiskNotifier();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;
    }

void CStorageDiskNotifier::ConstructL()
    {    
    User::LeaveIfError(iFs.Connect());
    iStorageDiskNotifyHandler = CDiskNotifyHandler::NewL( *this, iFs );
    SubscribeStorageDiskNotificationL();     
    }

CStorageDiskNotifier::CStorageDiskNotifier()
    {
    }

CStorageDiskNotifier::~CStorageDiskNotifier()
    {        
    iFs.Close();
    delete iStorageDiskNotifyHandler;    
    }

void CStorageDiskNotifier::AddObserver(MStorageSpaceNotifyObserver *observer)
{    
    m_observers.append(observer); 
}

void CStorageDiskNotifier::RemoveObserver(MStorageSpaceNotifyObserver *observer)
{    
    m_observers.removeOne(observer); 
}

void CStorageDiskNotifier::SubscribeStorageDiskNotificationL()
    {    
    // Start listening disk events first
    iStorageDiskNotifyHandler->NotifyDisk();
    
    TDriveList driveList;
    TInt count(0);
    User::LeaveIfError(DriveInfo::GetUserVisibleDrives(
               iFs, driveList, count, KDriveAttInternal | KDriveAttRemovable )); // Get already inserted internal and removable drives    
    for( TInt i = EDriveA; i <= EDriveZ; i++ )
        {
        if (driveList[i])
            {            
            RegisterDiskSpaceEvents(i);
            }
        }    
    }

void CStorageDiskNotifier::RegisterDiskSpaceEvents(TInt aDrive)
    {
    TInt64 volSize(0);        
    TVolumeInfo volInfo;

    if(iFs.Volume(volInfo, aDrive) == KErrNone){
        volSize = volInfo.iSize;
        // get the threshold for the drive 
        TInt64 normalThreshold = (TInt64) volInfo.iFree;
        TInt64 lowThreshold = (TInt64)(volSize * 0.4);               
        TInt64 veryLowThreshold = (TInt64)(volSize * 0.1);        
        TInt64 criticalThreshold = (TInt64)(volSize * 0.02);       
                
        iStorageDiskNotifyHandler->NotifyDiskSpace(normalThreshold, aDrive);          
        iStorageDiskNotifyHandler->NotifyDiskSpace(lowThreshold, aDrive);
        iStorageDiskNotifyHandler->NotifyDiskSpace(veryLowThreshold, aDrive);
        iStorageDiskNotifyHandler->NotifyDiskSpace(criticalThreshold, aDrive);
        }    
    }

void CStorageDiskNotifier::HandleNotifyDiskSpace(TInt /*aError*/, const TDiskSpaceEvent& aEvent)
    {        
    TInt drive = aEvent.iDrive;    
    TChar volumeChar;
    QString volume;    
    if (iFs.DriveToChar(drive, volumeChar) == KErrNone){
        volume = QChar(volumeChar).toAscii(); 
        }
    foreach (MStorageSpaceNotifyObserver *observer, m_observers)
        observer->DiskSpaceChanged(volume);    
    }

void CStorageDiskNotifier::HandleNotifyDisk(TInt /*aError*/, const TDiskEvent& aEvent)
    {       
    TVolumeInfo volInfo;
    switch(aEvent.iType)
        {
        case EDiskAdded:
        case EDiskStatusChanged:                                      
             if ( iFs.Volume( volInfo, aEvent.iDrive ) == KErrNone)
                 {                 
                 TRAP_IGNORE( RegisterDiskSpaceEvents(aEvent.iDrive) );
                 }
             else
                 {                 
                 iStorageDiskNotifyHandler->CancelNotifyDiskSpace(aEvent.iDrive);                
                 }  
             break;
        case EDiskError:
        case EDiskRemoved:              
             iStorageDiskNotifyHandler->CancelNotifyDiskSpace(aEvent.iDrive);             
             break;
        }    
    }

