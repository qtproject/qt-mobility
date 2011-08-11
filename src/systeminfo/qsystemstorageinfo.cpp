/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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

Q_GLOBAL_STATIC(QSystemStorageInfoPrivate, storageInfoPrivateSingleton)

#ifdef QT_SIMULATOR
QSystemStorageInfoPrivate *getSystemStorageInfoPrivate() { return storageInfoPrivateSingleton(); }
#endif // QT_SIMULATOR

/*!
    \class QSystemStorageInfo
    \ingroup systeminfo
    \inmodule QtSystemInfo
    \brief The QSystemStorageInfo class provides access to disk storage information from the system.
    \since 1.1


    \reentrant

    \note Most functions in this class are reentrant on all platforms. The exceptions are listed below.

    \warning On Symbian this class does not support QObject::moveToThread().
*/

/*!
    \enum QSystemStorageInfo::DriveType
    This enum describes the type of drive or volume

    \value NoDrive               Drive type undetermined.
    \value InternalDrive         Is internal mass storage drive like a hard drive.
    \value RemovableDrive        Is a removable disk like MMC.
    \value RemoteDrive           Is a network drive.
    \value CdromDrive            Is a cd rom drive.
    \value InternalFlashDrive    Is an internal flash disk, or Phone Memory. Since 1.2
    \value RamDrive              Is a virtual drive made in RAM memory. Since 1.2
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
    \fn void QSystemStorageInfo::logicalDriveChanged(bool added, const QString &drive)

    This signal gets emitted when a new \a drive storage has been added or removed. If \a added is true,
    it means a new drive is found, otherwise a drive is removed.
    \since 1.1
*/

/*!
    \fn void QSystemStorageInfo::storageStateChanged(const QString &drive, QSystemStorageInfo::StorageState state)

    This signal gets emitted when the storage state of a \a drive has changed to \a state. Note that the
    polling time may be different for different platforms.
    \since 1.2
*/

/*!
    Constructs a QSystemStorageInfo with the given \a parent.
*/
QSystemStorageInfo::QSystemStorageInfo(QObject *parent)
    : QObject(parent)
{
#ifdef Q_OS_SYMBIAN
    d = new QSystemStorageInfoPrivate();
#else
    d = storageInfoPrivateSingleton();
#endif
    qRegisterMetaType<QSystemStorageInfo::DriveType>("QSystemStorageInfo::DriveType");
    qRegisterMetaType<QSystemStorageInfo::StorageState>("QSystemStorageInfo::StorageState");
}

/*!
    Destroys the QSystemStorageInfo object.
*/
QSystemStorageInfo::~QSystemStorageInfo()
{
#ifdef Q_OS_SYMBIAN
    delete d;
#endif
}

/*!
    Returns the amount of total space on the \a drive, in bytes.
*/
qlonglong QSystemStorageInfo::totalDiskSpace(const QString &drive)
{
    return d->totalDiskSpace(drive);
}

/*!
    Returns the amount of available free space on the \a drive, in bytes.
*/
qlonglong QSystemStorageInfo::availableDiskSpace(const QString &drive)
{
    return d->availableDiskSpace(drive);
}

/*!
    \property QSystemStorageInfo::logicalDrives
    \brief The  list of logical drives.

    Returns a QStringList of drives or volumes, or an empty list if no drives are found.

    \warning On Symbian this function is not reentrant and must be used from main thread only.
*/
QStringList QSystemStorageInfo::logicalDrives()
{
    return storageInfoPrivateSingleton()->logicalDrives();
}

/*!
    Returns the type of the give \a drive.
*/
QSystemStorageInfo::DriveType QSystemStorageInfo::typeForDrive(const QString &drive)
{
    return d->typeForDrive(drive);
}

/*!
    Returns the URI, or unique identifier for the given \a drive.
    \since 1.2
*/
QString QSystemStorageInfo::uriForDrive(const QString &drive)
{
    return d->uriForDrive(drive);
}

/*!
    Returns the storage state of the given \a drive.
    \since 1.2
*/
QSystemStorageInfo::StorageState QSystemStorageInfo::getStorageState(const QString &drive)
{
    return d->getStorageState(drive);
}

/*!
    \internal
*/
void QSystemStorageInfo::connectNotify(const char *signal)
{
     if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDriveChanged(bool, const QString &))))) {
         connect(d, SIGNAL(logicalDriveChanged(bool,QString)),
                 this, SIGNAL(logicalDriveChanged(bool,QString)), Qt::UniqueConnection);
     }
     if (QLatin1String(signal) == SIGNAL(logicalDriveChanged(bool,QString))) {
         connect(d, SIGNAL(storageStateChanged(const QString &,QSystemStorageInfo::StorageState)),
                 this, SIGNAL(storageStateChanged(const QString &,QSystemStorageInfo::StorageState)),Qt::UniqueConnection);
     }
}

#include "moc_qsystemstorageinfo.cpp"

QTM_END_NAMESPACE
