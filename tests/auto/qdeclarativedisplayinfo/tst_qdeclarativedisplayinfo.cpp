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
#include <QDesktopWidget>

#include "qdeclarativedisplayinfo_p.h"
#include <QDesktopWidget>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QSystemDisplayInfo::DisplayOrientation);

class tst_QDeclarativeDisplayInfo : public QObject
{
    Q_OBJECT

private slots:
    void tst_displayBrightness();
    void tst_colorDepth();
    void tst_orientation();
    void tst_contrast();
    void tst_dpiWidth();
    void tst_dpiHeight();
    void tst_physicalHeight();
    void tst_physicalWidth();
    void tst_backlightStatus();

};

void tst_QDeclarativeDisplayInfo::tst_displayBrightness()
{
    QDeclarativeDisplayInfo di;
    QVERIFY(di.displayBrightness() > -2);
    di.setCurrentScreen(999);
    QVERIFY(di.displayBrightness() == -1);
}

void tst_QDeclarativeDisplayInfo::tst_colorDepth()
{
    QDeclarativeDisplayInfo di;
    int depth = di.colorDepth();
    QDesktopWidget wid;
    if(wid.screenCount() > 1) {
        QVERIFY(depth == 0
                || depth == 8
                || depth == 16
                || depth == 24
                || depth == 32
                || depth == 64);
        }
    di.setCurrentScreen(999);
    QVERIFY(di.colorDepth() == -1);
}

void tst_QDeclarativeDisplayInfo::tst_orientation()
{
    QSystemDisplayInfo::DisplayOrientation orient = QSystemDisplayInfo::Unknown;
    QDeclarativeDisplayInfo di;
    orient = di.orientation();
    QRect availableGeometry = QApplication::desktop()->availableGeometry();

    if(availableGeometry.width() > availableGeometry.height()) {
        QVERIFY(orient == QDeclarativeDisplayInfo::Landscape
                || orient == QDeclarativeDisplayInfo::InvertedLandscape);
    } else {
        QVERIFY(orient == QDeclarativeDisplayInfo::Portrait
                || orient == QDeclarativeDisplayInfo::InvertedPortrait);
    }
}

void tst_QDeclarativeDisplayInfo::tst_contrast()
{
    QDeclarativeDisplayInfo di;
    QVERIFY(di.contrast() >= 0
            && di.contrast()< 1.0 );
    // contrast levels are between 0 and 1.0
}

void tst_QDeclarativeDisplayInfo::tst_dpiWidth()
{
    QDeclarativeDisplayInfo di;
    QVERIFY(di.dpiWidth() > -1);
}

void tst_QDeclarativeDisplayInfo::tst_dpiHeight()
{
    QDeclarativeDisplayInfo di;
    QVERIFY(di.dpiHeight() > -1);
}

void tst_QDeclarativeDisplayInfo::tst_physicalHeight()
{
    QDeclarativeDisplayInfo di;
    QVERIFY(di.physicalHeight() > -1);
}

void tst_QDeclarativeDisplayInfo::tst_physicalWidth()
{
    QDeclarativeDisplayInfo di;
    QVERIFY(di.physicalWidth() > -1);
}

void tst_QDeclarativeDisplayInfo::tst_backlightStatus()
{

}



QTEST_MAIN(tst_QDeclarativeDisplayInfo)
#include "tst_qdeclarativedisplayinfo.moc"
