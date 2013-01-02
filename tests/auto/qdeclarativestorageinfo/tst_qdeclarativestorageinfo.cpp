/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

//TESTED_COMPONENT=src/systeminfo

#include <QtTest/QtTest>
#include "qdeclarativestorageinfo_p.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemStorageInfo::DriveType);
Q_DECLARE_METATYPE(QSystemStorageInfo::StorageState);

class tst_QDeclarativeStorageInfo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void tst_totalDiskSpace();
    void tst_availableDiskSpace();
    void tst_logicalDrives();
    void tst_typeForDrive();
    void tst_getStorageState();

};

void tst_QDeclarativeStorageInfo::initTestCase()
{
    qRegisterMetaType<QSystemStorageInfo::DriveType>("QSystemStorageInfo::VolumeType");
}

void tst_QDeclarativeStorageInfo::tst_totalDiskSpace()
{
    QDeclarativeStorageInfo mi;
    QStringList volList = mi.logicalDrives();
    foreach(QString vol, volList) {
        QVERIFY(mi.totalDiskSpace(vol) > -1);
    }
}

void tst_QDeclarativeStorageInfo::tst_availableDiskSpace()
{
    QDeclarativeStorageInfo mi;
    QStringList volList = mi.logicalDrives();
    foreach(QString vol, volList) {
        QVERIFY(mi.availableDiskSpace(vol) > -1);
    }
}

void tst_QDeclarativeStorageInfo::tst_logicalDrives()
{
    QDeclarativeStorageInfo mi;
    QVERIFY(mi.logicalDrives().count() > -1);
}

void tst_QDeclarativeStorageInfo::tst_typeForDrive()
{
    QDeclarativeStorageInfo mi;
    QStringList volList = mi.logicalDrives();
    foreach(QString vol, volList) {
        QSystemStorageInfo::DriveType type = mi.typeForDrive(vol);

        QVERIFY(type == QSystemStorageInfo::NoDrive
                || type == QSystemStorageInfo::CdromDrive
                || type == QSystemStorageInfo::InternalDrive
                || type == QSystemStorageInfo::RemoteDrive
                || type == QSystemStorageInfo::RemovableDrive
                || type == QSystemStorageInfo::InternalFlashDrive
                || type == QSystemStorageInfo::RamDrive
                );
    }
}

void tst_QDeclarativeStorageInfo::tst_getStorageState()
{
    QDeclarativeStorageInfo mi;
    QStringList volList = mi.logicalDrives();
    foreach(QString vol, volList) {
        QSystemStorageInfo::StorageState state = mi.getStorageState(vol);
        QVERIFY(state == QSystemStorageInfo::UnknownStorageState
                || state == QSystemStorageInfo::NormalStorageState
                || state == QSystemStorageInfo::LowStorageState
                || state == QSystemStorageInfo::VeryLowStorageState
                || state == QSystemStorageInfo::CriticalStorageState);
    }
}


QTEST_MAIN(tst_QDeclarativeStorageInfo)
#include "tst_qdeclarativestorageinfo.moc"
