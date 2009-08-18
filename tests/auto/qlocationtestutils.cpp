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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qlocationtestutils_p.h"

#if defined(Q_OS_SYMBIAN)
    #include <e32std.h>
#endif

void QLocationTestUtils::uheap_mark()
{
#if defined(Q_OS_SYMBIAN)
    __UHEAP_MARK;
#endif
}

void QLocationTestUtils::uheap_mark_end()
{
#if defined(Q_OS_SYMBIAN)
    __UHEAP_MARKEND;
#endif
}

QString QLocationTestUtils::addNmeaChecksumAndBreaks(const QString &sentence)
{
    Q_ASSERT(sentence[0] == '$' && sentence[sentence.length()-1] == '*');

    // XOR byte value of all characters between '$' and '*'
    int result = 0;
    for (int i=1; i<sentence.length()-1; i++)
        result ^= sentence[i].toAscii();
    QString sum;
    sum.sprintf("%02x", result);
    return sentence + sum + "\r\n";
}
