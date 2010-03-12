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

#include "tst_versitutils.h"
#include <QByteArray>
#include <QtTest/QtTest>
#include <QTextCodec>

#include "versitutils_p.h"

QTM_USE_NAMESPACE

void tst_VersitUtils::testBackSlashEscape()
{
    // Empty string
    QString input;
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString());

    // Nothing to escape in the string
    input = QString::fromAscii("Nothing to escape");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("Nothing to escape"));

    // Line break in the beginning
    input = QString::fromAscii("\r\n input");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\n input"));

    // Line break in the end
    input = QString::fromAscii("input\r\n");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\n"));

    // Semicolon in the beginning
    input = QString::fromAscii(";input");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\;input"));

    // Semicolon in the end
    input = QString::fromAscii("input;");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\;"));

    // Comma in the beginning
    input = QString::fromAscii(",input");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\,input"));

    // Comma in the end
    input = QString::fromAscii("input,");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\,"));

    // Backslash in the beginning
    input = QString::fromAscii("\\input");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\\\input"));

    // Backslash in the end
    input = QString::fromAscii("input\\");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\\\"));

    // Line break, semicolon, backslash and comma in the middle of the string
    input = QString::fromAscii("Escape these \r\n ; , \\ ");
    VersitUtils::backSlashEscape(input);
    QCOMPARE(input, QString::fromAscii("Escape these \\n \\; \\, \\\\ "));

}

void tst_VersitUtils::testRemoveBackSlashEscaping()
{
    // Empty string
    QString input;
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input,QString());

    // Nothing to escape in the string
    input = QString::fromAscii("Nothing to escape");
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input,QString::fromAscii("Nothing to escape"));

    // Line break, semicolon, backslash and comma in the string
    input = QString::fromAscii("These should be unescaped \\n \\N \\; \\, \\\\");
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input, QString::fromAscii("These should be unescaped \r\n \r\n ; , \\"));

    // Don't remove escaping within quotes
    input = QString::fromAscii("\"Quoted \\n \\N \\; \\,\"");
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input, QString::fromAscii("\"Quoted \\n \\N \\; \\,\""));
}

QTEST_MAIN(tst_VersitUtils)

