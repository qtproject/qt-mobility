/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtCore/qcoreapplication.h>
#include <QtTest/QtTest>

#include "tst_qradiotuner.h"

#ifdef Q_OS_SYMBIAN
#ifdef HAS_OPENMAXAL_RADIOTUNER_BACKEND
#include "tst_qradiotuner_xa.h"
#else
#include "tst_qradiotuner_s60.h"
#endif
#endif

int main(int argc, char**argv)
{
    QApplication app(argc,argv);
    int ret;
    tst_QRadioTuner test_api;
    ret = QTest::qExec(&test_api, argc, argv);
#ifdef Q_OS_SYMBIAN
#ifdef HAS_OPENMAXAL_RADIOTUNER_BACKEND
    char *new_argv[3];
    QString str = "C:\\data\\" + QFileInfo(QCoreApplication::applicationFilePath()).baseName() + ".log";
    QByteArray   bytes  = str.toAscii();
    char arg1[] = "-o";
    new_argv[0] = argv[0];
    new_argv[1] = arg1;
    new_argv[2] = bytes.data();
    tst_QXARadio_xa test_xa;
    ret = QTest::qExec(&test_xa, 3, new_argv);
#else
    tst_QRadioTuner_s60 test_s60;
    ret = QTest::qExec(&test_s60, argc, argv);
#endif
#endif
    return ret;
}
