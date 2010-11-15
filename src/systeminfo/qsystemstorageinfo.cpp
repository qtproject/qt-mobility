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

#include "qsystemstorageinfo.h"
#include "qsysteminfocommon_p.h"
#include <QMetaType>

QTM_BEGIN_NAMESPACE

        Q_GLOBAL_STATIC(QSystemStorageInfoPrivate, storageInfoPrivate)

#ifdef QT_SIMULATOR
QSystemStorageInfoPrivate *getSystemStorageInfoPrivate() { return storageInfoPrivate(); }
#endif

/*!
   \class QSystemStorageInfo
   \ingroup systeminfo
   \inmodule QtSystemInfo
        \brief The QSystemStorageInfo class provides access to memory and disk information from the system.
*/

/*!
    \enum QSystemStorageInfo::DriveType
    This enum describes the type of drive or volume

    \value NoDrive               Drive type undetermined.
    \value InternalDrive         Is internal mass storage drive like a harddrive.
    \value RemovableDrive        Is a removable disk like MMC.
    \value RemoteDrive           Is a network drive.
    \value CdromDrive            Is a cd rom drive.
    \value InternalFlashDrive    Is an internal flash disk, or Phone Memory.
    \value RamDrive              Is a virtual drive made in RAM memory.
*/

/*!
    \enum QSystemStorageInfo::StorageState
    This enum describes the state of the storage level of drive or volume.

    \value UnknownStorageState         Storage level indicates an error, offline or unknown.
    \value NormalStorageState          Storage level indicates normal.
    \value LowStorageState             Storage level indicates below 40%.
    \value VeryLowStorageState         Storage level indicates below 10%.
    \value CriticalStorageState        Storage level indicates below 2%.

*/

/*!
     \fn void QSystemStorageInfo::logicalDriveChanged(bool ,const QString &)

     This signal gets emitted when new storage has been added or removed from the system.
*/

/*!
     \fn void QSystemStorageInfo::storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state)

     This signal gets emitted when a volume has changed from one StorageState to another, \vol
     being the volume name, and \state being the new state..
*/


/*!
    Constructs a QSystemStorageInfo with the given \a parent.
*/
QSystemStorageInfo::QSystemStorageInfo(QObject *parent)
   : QObject(parent), d(storageInfoPrivate())
{
    qRegisterMetaType<QSystemStorageInfo::DriveType>("QSystemStorageInfo::DriveType");
    qRegisterMetaType<QSystemStorageInfo::StorageState>("QSystemStorageInfo::StorageState");

    connect(d,SIGNAL(logicalDriveChanged(bool,const QString &)),
           this,SIGNAL(logicalDriveChanged(bool,const QString &)),Qt::UniqueConnection);

    connect(d,SIGNAL(storageStateChanged(const QString &,QSystemStorageInfo::StorageState)),
           this,SIGNAL(storageStateChanged(const QString &,QSystemStorageInfo::StorageState)),Qt::UniqueConnection);
}

/*!
 Destroys the QSystemStorageInfo object.
*/
QSystemStorageInfo::~QSystemStorageInfo()
{
}

/*!
   Returns the amount of total space on the \a volumeDrive,
   in bytes.
*/
qlonglong QSystemStorageInfo::totalDiskSpace(const QString &volumeDrive)
{
   return storageInfoPrivate()->totalDiskSpace(volumeDrive);
}

/*!
   Returns the amount of available free space on the \a volumeDrive,
in bytes.
*/
qlonglong QSystemStorageInfo::availableDiskSpace(const QString &volumeDrive)
{
   return storageInfoPrivate()->availableDiskSpace(volumeDrive);
}

/*!
 \property QSystemStorageInfo::logicalDrives
 \brief The logical drives.

   Returns a QStringList of volumes or partitions, or an empty list if no drives are found.
*/
QStringList QSystemStorageInfo::logicalDrives()
{
   return storageInfoPrivate()->logicalDrives();
}

/*!
 Returns the type of volume \a driveVolume
*/
QSystemStorageInfo::DriveType QSystemStorageInfo::typeForDrive(const QString &driveVolume)
{
   return storageInfoPrivate()->typeForDrive(driveVolume);
}

/*!
  Returns the uri, or unique identifier fir \a driveVolume.
  */
QString QSystemStorageInfo::uriForDrive(const QString &driveVolume)
{
    return storageInfoPrivate()->uriForDrive(driveVolume);
}


/*!
 Returns the storage state of volume \a driveVolume
*/
QSystemStorageInfo::StorageState QSystemStorageInfo::getStorageState(const QString &driveVolume)
{
   return storageInfoPrivate()->getStorageState(driveVolume);
}

#include "moc_qsystemstorageinfo.cpp"

QTM_END_NAMESPACE
