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
#include "qsysteminfo_simulator_p.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemStorageInfo::DriveType);
Q_DECLARE_METATYPE(QSystemStorageInfo::StorageState);

/**
 * Starts an event loop that runs until the given signal is received.
 * Optionally the event loop can return earlier on a timeout.
 *
 * \return \p true if the requested signal was received
 *         \p false on timeout
 */
static bool waitForSignal(QObject *obj, const char *signal, int timeout = 0)
{
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer timer;
    QSignalSpy timeoutSpy(&timer, SIGNAL(timeout()));
    if (timeout > 0) {
        QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
    }
    loop.exec();
    return timeoutSpy.isEmpty();
}

class ChangeStorageThread : public QThread
{
public:
    void run()
    {
        QMutexLocker locker(&mutex);
        SystemInfoConnection si;
        QSystemStorageInfoPrivate *st = si.storageInfoPrivate();

//        if (addIt) {
//            if (!remove) {
//                st->addDrive(volName, driveType, totalSpace, availableSpace,"XXX");
//            } else {
//                st->removeDrive(volName);
//            }
//        } else {
//            st->setTotalSpace(volName,totalSpace);
//            st->setAvailableSpace(volName,availableSpace);
//            st->setStorageState(volName,state);
//        }

        this->exit();

    }
    QString volName;
    quint64 totalSpace;
    quint64 availableSpace;
    QSystemStorageInfo::StorageState state;
    QSystemStorageInfo::DriveType driveType;
    bool addIt;
    bool remove;
    QMutex mutex;
};

class tst_QSystemStorageInfo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void tst_totalDiskSpace();
    void tst_availableDiskSpace();
    void tst_logicalDrives();
    void tst_typeForDrive();

    void tst_getStorageState();

    void tst_storageStateChanged_data();
    void tst_storageStateChanged();

    void tst_logicalDriveChanged_data();
    void tst_logicalDriveChanged();

    void logicalDriveChanged(bool added,const QString &vol);
    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state); //1.2

private:
    void newDrives();
    QSystemStorageInfo::StorageState sstate;
    QSystemStorageInfo::DriveType driveType;
    bool driveAdded;
    QString dName;
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
                || type == QSystemStorageInfo::RemovableDrive
                || type == QSystemStorageInfo::InternalFlashDrive
                || type == QSystemStorageInfo::RamDrive
                );
    }
}

void tst_QSystemStorageInfo::tst_getStorageState()
{
    QSystemStorageInfo mi;
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

void tst_QSystemStorageInfo::tst_storageStateChanged_data()
{
    QTest::addColumn<QSystemStorageInfo::DriveType>("drivetype");
    QTest::addColumn<QSystemStorageInfo::StorageState>("storagestate");
    QTest::addColumn<QString>("drivename");

    QTest::newRow("NoDrive") << QSystemStorageInfo::NoDrive
                             << QSystemStorageInfo::UnknownStorageState
                             << "NoDrive";
    QTest::newRow("InternalDrive") << QSystemStorageInfo::InternalDrive
                                      << QSystemStorageInfo::NormalStorageState
                                      << "InternalDrive";
    QTest::newRow("RemovableDrive") << QSystemStorageInfo::RemovableDrive
                                       << QSystemStorageInfo::LowStorageState
                                           << "RemovableDrive";
    QTest::newRow("RemoteDrive") << QSystemStorageInfo::RemoteDrive
                                    << QSystemStorageInfo::VeryLowStorageState
                                       << "RemoteDrive";
    QTest::newRow("CdromDrive") << QSystemStorageInfo::CdromDrive
                                   << QSystemStorageInfo::CriticalStorageState
                                       << "CdromDrive";
    QTest::newRow("InternalFlashDrive") << QSystemStorageInfo::InternalFlashDrive
                                        << QSystemStorageInfo::UnknownStorageState
                                           << "InternalFlashDrive";
    QTest::newRow("RamDrive") << QSystemStorageInfo::RamDrive
                                 << QSystemStorageInfo::UnknownStorageState
                                 << "RamDrive";
    SystemInfoConnection si;
    QSystemStorageInfo sti;
    QSystemStorageInfoPrivate *st = si.storageInfoPrivate();
    QStringList curDrives = sti.logicalDrives();
    foreach (const QString &name, curDrives)
        st->removeDrive(name);

}

void tst_QSystemStorageInfo::newDrives()
{
    QFETCH(QString, drivename);
    QFETCH(QSystemStorageInfo::DriveType, drivetype);

    QSystemStorageInfo sti;
    QStringList curDrives = sti.logicalDrives();
    SystemInfoConnection si;
    QSystemStorageInfoPrivate *st = si.storageInfoPrivate();

    st->addDrive(drivename, drivetype, 100, 90,"YYY");
}


void tst_QSystemStorageInfo::tst_storageStateChanged()
{
    QFETCH(QSystemStorageInfo::DriveType, drivetype);
    QFETCH(QString, drivename);

    QSystemStorageInfo sti;
    QStringList curDrives = sti.logicalDrives();
    SystemInfoConnection si;
    QSystemStorageInfoPrivate *st = si.storageInfoPrivate();
    driveAdded = true;
    dName = drivename;
    st->addDrive(drivename, drivetype, 100, 50,"YYY");
    connect(&sti,SIGNAL(storageStateChanged(const QString &, QSystemStorageInfo::StorageState)),
            this,SLOT(storageStateChanged(const QString &, QSystemStorageInfo::StorageState)));

    sstate = QSystemStorageInfo::CriticalStorageState;
    st->setAvailableSpace(drivename,1);
    QVERIFY(sti.availableDiskSpace(drivename) == 1);
    sstate = QSystemStorageInfo::NormalStorageState;
    st->setAvailableSpace(drivename,99);
    QVERIFY(sti.availableDiskSpace(drivename) == 99);
    sstate = QSystemStorageInfo::NormalStorageState;
    st->setAvailableSpace(drivename,41);
    QVERIFY(sti.availableDiskSpace(drivename) == 41);
    sstate = QSystemStorageInfo::LowStorageState;
    st->setAvailableSpace(drivename,40);
    QVERIFY(sti.availableDiskSpace(drivename) == 40);
    sstate = QSystemStorageInfo::VeryLowStorageState;
    st->setAvailableSpace(drivename,9);
    QVERIFY(sti.availableDiskSpace(drivename) == 9);

    driveAdded = false;
    st->removeDrive(drivename);
}

void tst_QSystemStorageInfo::tst_logicalDriveChanged_data()
{
    tst_storageStateChanged_data();
}

void tst_QSystemStorageInfo::tst_logicalDriveChanged()
{
    QFETCH(QSystemStorageInfo::DriveType, drivetype);
    QFETCH(QString, drivename);

    QSystemStorageInfo sti;
    QStringList curDrives = sti.logicalDrives();
    SystemInfoConnection si;
    QSystemStorageInfoPrivate *st = si.storageInfoPrivate();

    connect(&sti,SIGNAL(logicalDriveChanged(bool,const QString &)),
            this,SLOT(logicalDriveChanged(bool,const QString &)));

    driveAdded = true;
    dName = drivename;
    st->addDrive(drivename, drivetype, 100, 50,"YYY");

    driveAdded = false;
    st->removeDrive(drivename);
}

void tst_QSystemStorageInfo::logicalDriveChanged(bool added,const QString &vol)
{
    QVERIFY(added == driveAdded);
    QVERIFY(vol == dName);
}

void tst_QSystemStorageInfo::storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state)
{
    QVERIFY(vol == dName);
    QVERIFY(state == sstate);
}


QTEST_MAIN(tst_QSystemStorageInfo)
#include "tst_qsystemstorageinfo.moc"
