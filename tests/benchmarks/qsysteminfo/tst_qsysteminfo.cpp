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
#include <QtTest/QtTest>
#include "qsysteminfo.h"
#include <QDebug>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemInfo::Version);
Q_DECLARE_METATYPE(QSystemInfo::Feature);

class tst_QSystemInfo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void tst_createTime();

    void tst_currentLanguage();
    void tst_availableLanguages();

    void tst_versions_data();
    void tst_versions();

    void tst_hasFeatures_data();
    void tst_hasFeatures();

};

void tst_QSystemInfo::initTestCase()
{
    qRegisterMetaType<QSystemInfo::Version>("QSystemInfo::Version");
    qRegisterMetaType<QSystemInfo::Feature>("QSystemInfo::Feature");
}

void tst_QSystemInfo::tst_createTime()
{
    QBENCHMARK {
        QSystemInfo si;
    }
}

void tst_QSystemInfo::tst_currentLanguage()
{
    QSystemInfo si;
    QBENCHMARK {
        QString s = si.currentLanguage();
    }

}

void tst_QSystemInfo::tst_availableLanguages()
{
    QSystemInfo si;
    QBENCHMARK {
        QStringList available = si.availableLanguages();
    }
}

void tst_QSystemInfo::tst_versions_data()
{
    QTest::addColumn<QSystemInfo::Version>("version");
    QTest::addColumn<QString>("parameter");

    QTest::newRow("Os") << QSystemInfo::Os << "";
    QTest::newRow("QtCore") << QSystemInfo::QtCore << "";
    QTest::newRow("Firmware") << QSystemInfo::Firmware << "";

}

void tst_QSystemInfo::tst_versions()
{
    QBENCHMARK {
        QFETCH(QSystemInfo::Version, version);
        QFETCH(QString, parameter);
        QSystemInfo si;
        QString vers = si.version(version, parameter);
    }
}

void tst_QSystemInfo::tst_hasFeatures_data()
{
    QTest::addColumn<QSystemInfo::Feature>("feature");

    QTest::newRow("Bluetooth") << QSystemInfo::BluetoothFeature;
    QTest::newRow("CameraFeature") << QSystemInfo::CameraFeature;
    QTest::newRow("FmradioFeature") << QSystemInfo::FmradioFeature;
    QTest::newRow("IrFeature") << QSystemInfo::IrFeature;
    QTest::newRow("LedFeature") << QSystemInfo::LedFeature;
    QTest::newRow("MemcardFeature") << QSystemInfo::MemcardFeature;
    QTest::newRow("UsbFeature") << QSystemInfo::UsbFeature;
    QTest::newRow("VibFeature") << QSystemInfo::VibFeature;
    QTest::newRow("WlanFeature") << QSystemInfo::WlanFeature;
    QTest::newRow("SimFeature") << QSystemInfo::SimFeature;
    QTest::newRow("LocationFeature") << QSystemInfo::LocationFeature;
    QTest::newRow("VideoOutFeature") << QSystemInfo::VideoOutFeature;
    QTest::newRow("HapticsFeature") << QSystemInfo::HapticsFeature;
}

void tst_QSystemInfo::tst_hasFeatures()
{
    QBENCHMARK {
        QFETCH(QSystemInfo::Feature, feature);
        QSystemInfo si;        
        si.hasFeatureSupported(feature);

    }
}


QTEST_MAIN(tst_QSystemInfo)
#include "tst_qsysteminfo.moc"
