/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include "qsysteminfo.h"

Q_DECLARE_METATYPE(QSystemNetworkInfo::CellNetworkStatus);
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkMode);


class tst_QSystemNetworkInfo : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void tst_getCellNetworkStatus_data();
    void tst_getCellNetworkStatus();

    void tst_networkSignalStrength();
    void tst_cellId();
    void tst_locationAreaCode();

    void tst_currentMCC(); // Mobile Country Code
    void tst_currentMNC(); // Mobile Network Code

    void tst_homeMCC();
    void tst_homeMNC();

    void tst_isLocationEnabled();
    void tst_isWLANAccessible();

    void tst_operatorName();
};
//signal todo:
//    void networkStatusChanged(QSystemNetworkInfo::NetworkMode netmode, QSystemNetworkInfo::CellNetworkStatus netStatus);

void tst_QSystemNetworkInfo::initTestCase()
{
    qRegisterMetaType<QSystemNetworkInfo::CellNetworkStatus>("QSystemNetworkInfo::CellNetworkStatus");
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
}

void tst_QSystemNetworkInfo::tst_getCellNetworkStatus_data()
{


}

void tst_QSystemNetworkInfo::tst_getCellNetworkStatus()
{

}

void tst_QSystemNetworkInfo::tst_networkSignalStrength()
{
    QSystemNetworkInfo ni;
    ni.networkSignalStrength();
}

void  tst_QSystemNetworkInfo::tst_cellId()
{
    QSystemNetworkInfo ni;
    ni.cellId();

}

void  tst_QSystemNetworkInfo::tst_locationAreaCode()
{
    QSystemNetworkInfo ni;
ni.locationAreaCode();
}


void  tst_QSystemNetworkInfo::tst_currentMCC()
{
    QSystemNetworkInfo ni;
ni.currentMCC();
}

void  tst_QSystemNetworkInfo::tst_currentMNC()
{
    QSystemNetworkInfo ni;
ni.currentMNC();
}


void  tst_QSystemNetworkInfo::tst_homeMCC()
{
    QSystemNetworkInfo ni;
ni.homeMCC();
}

void  tst_QSystemNetworkInfo::tst_homeMNC()
{
    QSystemNetworkInfo ni;
ni.homeMNC();
}


void  tst_QSystemNetworkInfo::tst_isLocationEnabled()
{
    QSystemNetworkInfo ni;
ni.isLocationEnabled();
}

void  tst_QSystemNetworkInfo::tst_isWLANAccessible()
{
    QSystemNetworkInfo ni;
ni.isWLANAccessible();
}


void  tst_QSystemNetworkInfo::tst_operatorName()
{
    QSystemNetworkInfo ni;
    QVERIFY(!ni.operatorName().isEmpty());

}

QTEST_MAIN(tst_QSystemNetworkInfo)
#include "tst_qsystemnetworkinfo.moc"
