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


#ifndef VERSITUTILS_P_H
#define VERSITUTILS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"

#include <QByteArray>
#include <QByteArrayMatcher>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QMultiHash>

QTM_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT VersitUtils
{
public:
    static QByteArray encode(const QByteArray& ba, QTextCodec* codec);
    static QByteArray encode(char ch, QTextCodec* codec);
    static QList<QByteArrayMatcher>* newlineList(QTextCodec* codec);
    static void changeCodec(QTextCodec* codec);

private:
    // These are caches for performance:
    // The previous codec that encode(char, QTextCodec) was called with
    static QTextCodec* m_previousCodec;
    // The QByteArray corresponding to each char from 0-255, encoded with m_previousCodec
    static QByteArray m_encodingMap[256];
    // List of different newline delimeters, encoded with m_previousCodec
    static QList<QByteArrayMatcher>* m_newlineList;
};

QTM_END_NAMESPACE

#endif // VERSITUTILS_P_H
