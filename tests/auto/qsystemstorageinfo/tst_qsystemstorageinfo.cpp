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

//TESTED_COMPONENT=src/systeminfo

#include <QtTest/QtTest>
#include "qsysteminfo.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemStorageInfo::DriveType);

class tst_QSystemStorageInfo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void tst_totalDiskSpace();
    void tst_availableDiskSpace();
    void tst_logicalDrives();
    void tst_typeForDrive();

};

void tst_QSystemStorageInfo::initTestCase()
{
    qRegisterMetaType<QSystemStorageInfo::DriveType>("QSystemStorageInfo::VolumeType");
}

void tst_QSystemStorageInfo::tst_totalDiskSpace()
{
    QSystemStorageInfo mi;
    QStringList volList = mi.logicalDrives();
    foreach(QString vol, volList) {
        QVERIFY(mi.totalDiskSpace(vol) > -1);
    }
}

void tst_QSystemStorageInfo::tst_availableDiskSpace()
{
    QSystemStorageInfo mi;
    QStringList volList = mi.logicalDrives();
    foreach(QString vol, volList) {
        QVERIFY(mi.availableDiskSpace(vol) > -1);
    }
}

void tst_QSystemStorageInfo::tst_logicalDrives()
{
    QSystemStorageInfo mi;
    QVERIFY(mi.logicalDrives().count() > -1);
}

void tst_QSystemStorageInfo::tst_typeForDrive()
{
    QSystemStorageInfo mi;
    QStringList volList = mi.logicalDrives();
    foreach(QString vol, volList) {
        QSystemStorageInfo::DriveType type = mi.typeForDrive(vol);

        QVERIFY(type == QSystemStorageInfo::NoDrive
                || type == QSystemStorageInfo::CdromDrive
                || type == QSystemStorageInfo::InternalDrive
                || type == QSystemStorageInfo::RemoteDrive
                || type == QSystemStorageInfo::RemovableDrive);
    }
}


QTEST_MAIN(tst_QSystemStorageInfo)
#include "tst_qsystemstorageinfo.moc"
