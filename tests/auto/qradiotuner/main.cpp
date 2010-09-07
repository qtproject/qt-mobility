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
#include <QtCore/qcoreapplication.h>
#include <QtTest/QtTest>

#include "tst_qradiotuner.h"

#ifdef Q_OS_SYMBIAN
#include "tst_qradiotuner_s60.h"
#endif

int main(int argc, char**argv)
{
    QApplication app(argc,argv);
    int ret;
    tst_QRadioTuner test_api;
    ret = QTest::qExec(&test_api, argc, argv);
#ifdef Q_OS_SYMBIAN
    tst_QRadioTuner_s60 test_s60;
    ret = QTest::qExec(&test_s60, argc, argv);
#endif
    return ret;
}
