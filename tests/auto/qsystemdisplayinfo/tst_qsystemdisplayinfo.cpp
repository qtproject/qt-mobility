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
class tst_QSystemDisplayInfo : public QObject
{
    Q_OBJECT

private slots:
    void tst_displayBrightness();
    void tst_colorDepth();

};

void tst_QSystemDisplayInfo::tst_displayBrightness()
{
    QSystemDisplayInfo di;
    QVERIFY(di.displayBrightness(0) > -2);
}

void tst_QSystemDisplayInfo::tst_colorDepth()
{
    QSystemDisplayInfo di;
    int depth = di.colorDepth(0);
    qWarning() << __FUNCTION__ << depth;

    QVERIFY(depth == 0
            || depth == 8
            || depth == 16
            || depth == 24
            || depth == 32
            || depth == 64);
}


QTEST_MAIN(tst_QSystemDisplayInfo)
#include "tst_qsystemdisplayinfo.moc"
