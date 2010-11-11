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

#include <qndefnfctextrecord.h>

#include <QtCore/QTextCodec>

#include <QtCore/QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QNdefNfcTextRecord
    \brief The QNdefNfcTextRecord class provides an NFC RTD-Text

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    RTD-Text encapsulates a user displayable text record.
*/

/*!
    \enum QNdefNfcTextRecord::Encoding

    This enum describes the text encoding standard used.

    \value Utf8     The text is encoded with UTF-8.
    \value Utf16    The text is encoding with UTF-16.
*/

/*!
    Returns the locale of the text record.
*/
QLocale QNdefNfcTextRecord::locale() const
{
    const QByteArray p = payload();

    if (p.isEmpty())
        return QLocale();

    quint8 status = p.at(0);

    quint8 codeLength = status & 0x3f;

    return QLocale(QString::fromAscii(p.constData() + 1, codeLength));
}

/*!
    Sets the locale of the text record to \a locale.
*/
void QNdefNfcTextRecord::setLocale(const QLocale &locale)
{
    QByteArray p = payload();

    quint8 status = p.isEmpty() ? 0 : p.at(0);

    quint8 codeLength = status & 0x3f;

    quint8 newStatus = (status & 0xd0) | locale.name().length();

    p[0] = newStatus;
    p.replace(1, codeLength, locale.name().toAscii());

    setPayload(p);
}

/*!
    Returns the contents of the text record as a string.
*/
QString QNdefNfcTextRecord::text() const
{
    const QByteArray p = payload();

    if (p.isEmpty())
        return QString();

    quint8 status = p.at(0);

    bool utf16 = status & 0x80;
    quint8 codeLength = status & 0x3f;

    QTextCodec *codec = QTextCodec::codecForName(utf16 ? "UTF-16BE" : "UTF-8");

    return codec->toUnicode(p.constData() + 1 + codeLength, p.length() - 1 - codeLength);
}

/*!
    Sets the contents of the text record to \a text.
*/
void QNdefNfcTextRecord::setText(const QString text)
{
    if (payload().isEmpty())
        setLocale(QLocale::system());

    QByteArray p = payload();

    quint8 status = p.at(0);

    bool utf16 = status & 0x80;
    quint8 codeLength = status & 0x3f;

    p.truncate(1 + codeLength);

    QTextCodec *codec = QTextCodec::codecForName(utf16 ? "UTF-16BE" : "UTF-8");

    p += codec->fromUnicode(text);

    setPayload(p);
}

/*!
    Returns the encoding of the contents.
*/
QNdefNfcTextRecord::Encoding QNdefNfcTextRecord::encoding() const
{
    if (payload().isEmpty())
        return Utf8;

    QByteArray p = payload();

    quint8 status = p.at(0);

    bool utf16 = status & 0x80;

    if (utf16)
        return Utf16;
    else
        return Utf8;
}

/*!
    Sets the enconding of the contents to \a encoding.
*/
void QNdefNfcTextRecord::setEncoding(Encoding encoding)
{
    QByteArray p = payload();

    quint8 status = p.isEmpty() ? 0 : p.at(0);

    QString string = text();

    if (encoding == Utf8)
        status &= ~0x80;
    else
        status |= 0x80;

    p[0] = status;

    setPayload(p);

    setText(string);
}

QTM_END_NAMESPACE
