/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSYSTEMSTORAGEINFO_H
#define QSYSTEMSTORAGEINFO_H

#include <QObject>
#include "qmobilityglobal.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemStorageInfoPrivate;

class Q_SYSINFO_EXPORT QSystemStorageInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList logicalDrives READ logicalDrives NOTIFY logicalDriveChanged)
    Q_ENUMS(DriveType)
    Q_ENUMS(StorageState)

public:
    explicit QSystemStorageInfo(QObject *parent = 0);
    ~QSystemStorageInfo();

    enum DriveType {
        NoDrive = 0,
        InternalDrive,
        RemovableDrive,
        RemoteDrive,
        CdromDrive,
        InternalFlashDrive, //1.2
        RamDrive //1.2
    };

    enum StorageState {
        UnknownStorageState = 0,
        NormalStorageState,
        LowStorageState, //40%
        VeryLowStorageState, //10%
        CriticalStorageState, //2%
    }; //1.2

    Q_INVOKABLE qlonglong totalDiskSpace(const QString &drive);
    Q_INVOKABLE qlonglong availableDiskSpace(const QString &drive);
    static QStringList logicalDrives();

    Q_INVOKABLE QSystemStorageInfo::DriveType typeForDrive(const QString &drive);

    Q_INVOKABLE QString uriForDrive(const QString &drive); //1.2
    Q_INVOKABLE QSystemStorageInfo::StorageState getStorageState(const QString &drive); //1.2

Q_SIGNALS:
    void logicalDriveChanged(bool added,const QString &drive);
    void storageStateChanged(const QString &drive, QSystemStorageInfo::StorageState state); //1.2

private:
       QSystemStorageInfoPrivate *d;
       void connectNotify(const char *signal);
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif // QSYSTEMSTORAGEINFO_H
