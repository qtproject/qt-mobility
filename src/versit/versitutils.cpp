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

#include "versitutils_p.h"
#include "qmobilityglobal.h"

#include <QMap>
#include <QTextCodec>
#include <QScopedPointer>

QTM_USE_NAMESPACE

QTextCodec* VersitUtils::m_previousCodec = 0;
QList<QByteArrayMatcher>* VersitUtils::m_newlineList = 0;
QByteArray VersitUtils::m_encodingMap[256];

/*!
 * Encode \a ch with \a codec, without adding an byte-order mark
 */
QByteArray VersitUtils::encode(char ch, QTextCodec* codec)
{
    if (codec != m_previousCodec) {
        changeCodec(codec);
    }
    return m_encodingMap[(int)ch];
}

/*!
 * Encode \a ba with \a codec, without adding an byte-order mark.  \a ba is interpreted as ASCII
 */
QByteArray VersitUtils::encode(const QByteArray& ba, QTextCodec* codec)
{
    QTextCodec::ConverterState state(QTextCodec::IgnoreHeader);
    return codec->fromUnicode(QString::fromAscii(ba.data()).data(), ba.length(), &state);
}

/*!
 * Returns the list of DOS, UNIX and Mac newline characters for \a codec.
 */
QList<QByteArrayMatcher>* VersitUtils::newlineList(QTextCodec* codec)
{
    if (m_newlineList != 0 && codec == m_previousCodec) {
        return m_newlineList;
    }
    changeCodec(codec);
    return m_newlineList;
}

/*!
 * Update the cached tables of pregenerated encoded text with \a codec.
 */
void VersitUtils::changeCodec(QTextCodec* codec) {
    // Build m_encodingMap
    QChar qch;
    QTextCodec::ConverterState state(QTextCodec::IgnoreHeader);
    for (int c = 0; c < 256; c++) {
        qch = QLatin1Char(c);
        m_encodingMap[c] = codec->fromUnicode(&qch, 1, &state);
    }

    // Build m_newlineList
    if (m_newlineList != 0)
        delete m_newlineList;
    m_newlineList = new QList<QByteArrayMatcher>;
    m_newlineList->append(QByteArrayMatcher(encode("\r\n", codec)));
    m_newlineList->append(QByteArrayMatcher(encode("\n", codec)));
    m_newlineList->append(QByteArrayMatcher(encode("\r", codec)));

    m_previousCodec = codec;
}
