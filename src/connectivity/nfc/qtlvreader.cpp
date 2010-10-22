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

#include "qtlvreader_p.h"

#include "qnearfieldtagtype1.h"

#include <QtCore/QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QTlvReader
    \brief The QTlvReader class provides a TLV parser.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    \internal

    QTlvReader can parse TLV data from either a QByteArray or directly from a QNearFieldTarget.

    TLV stands for Tag, Length, Value and is a common structure for storing data in near field
    targets.

    The following code shows how to use QTlvReader to extract NDEF messages from a near field
    target.

    \code
    QTlvReader reader(target);
    while (!reader.atEnd()) {
        reader.readNext();

        if (reader.tag() == 0x03)
            QNdefMessage ndefMessage = QNdefMessage::fromByteArray(reader.data());
    }
    \endcode
*/

/*!
    Constructs a new TLV reader for \a target.
*/
QTlvReader::QTlvReader(QNearFieldTarget *target)
:   m_target(target), m_index(-1)
{
    if (qobject_cast<QNearFieldTagType1 *>(m_target)) {
        addReservedMemory(0, 12);    // skip uid, cc
        addReservedMemory(96, 16);   // skip reserved block D, lock block E
    }
}

/*!
    Constructs a new TLV reader for \a data.
*/
QTlvReader::QTlvReader(const QByteArray &data)
:   m_target(0), m_data(data), m_index(-1)
{
}

/*!
    Add reserved memory area from \a offset of \a length bytes.  The parser will skip over reserved
    memory areas.
*/
void QTlvReader::addReservedMemory(int offset, int length)
{
    m_reservedMemory.insert(offset, length);
}

/*!
    Returns true if the TLV reader is at the end of the list of TLVs; otherwise returns false.
*/
bool QTlvReader::atEnd() const
{
    if (m_index == -1)
        return false;

    return (m_index == m_data.length()) || (tag() == 0xfe);
}

/*!
    Moves to the next TLV.
*/
void QTlvReader::readNext()
{
    if (atEnd())
        return;

    // Move to next TLV
    if (m_index == -1)
        m_index = 0;
    else if (tag() == 0x00 || tag() == 0xfe)
        ++m_index;
    else
        m_index = m_index + 1 + 1 + length();

    // Ensure that tag byte is available
    readMoreData(m_index);

    switch (tag()) {
    case 0x01: { // Lock Control TLV
        const QByteArray tlvData = data();

        quint8 position = tlvData.at(0);
        int pageAddr = position >> 4;
        int byteOffset = position & 0x0f;

        quint8 size = tlvData.at(1);

        quint8 pageControl = tlvData.at(2);
        int bytesPerPage = pageControl & 0x0f;

        if (bytesPerPage) {
            int byteAddress = pageAddr * (1 << (bytesPerPage - 1)) + byteOffset;

            addReservedMemory(byteAddress, (size ? size : 256) / 8);
        }

        break;
    }
    case 0x02: { // Reserved Memory Control TLV
        const QByteArray tlvData = data();

        quint8 position = tlvData.at(0);
        int pageAddr = position >> 4;
        int byteOffset = position & 0x0f;

        quint8 size = tlvData.at(1);

        quint8 pageControl = tlvData.at(2);
        int bytesPerPage = pageControl & 0x0f;

        if (bytesPerPage) {
            int byteAddress = pageAddr * (1 << (bytesPerPage - 1)) + byteOffset;

            addReservedMemory(byteAddress, size ? size : 256);
        }

        break;
    }
    }
}

/*!
    Returns the tag of the current TLV.
*/
quint8 QTlvReader::tag() const
{
    return m_data.at(m_index);
}

/*!
    Returns the length of the data of the current TLV.
*/
int QTlvReader::length()
{
    readMoreData(m_index + 1);

    int length = m_data.at(m_index + 1);
    if (length == 0xff) {
        readMoreData(m_index + 3);
        length = (m_data.at(m_index + 2) << 8) | m_data.at(m_index + 3);
        if (length < 0xff || length == 0xffff) {
            qWarning("Invalid 3 byte length");
            return 0;
        }
    }

    return length;
}

/*!
    Returns the data of the current TLV.
*/
QByteArray QTlvReader::data()
{
    int tlvLength = length();

    readMoreData(m_index + 1 + 1 + tlvLength - 1);

    return m_data.mid(m_index + 1 + 1, tlvLength);
}

/*!
    Reads more data until \a sparseOffset is available in m_data.
*/
void QTlvReader::readMoreData(int sparseOffset)
{
    while (sparseOffset >= m_data.length()) {
        // read the next segment
        int absOffset = absoluteOffset(m_data.length());
        quint8 segment = absOffset / 120;

        QByteArray data;

        if (QNearFieldTagType1 *tag = qobject_cast<QNearFieldTagType1 *>(m_target)) {
            data = segment ? tag->readSegment(segment) : tag->readAll().mid(2);
        }

        int startOffset = absOffset - (segment * 120);

        int length = -1;
        foreach (int offset, m_reservedMemory.keys()) {
            if (offset <= absOffset)
                continue;

            if (offset < (segment + 1) * 120)
                length = offset - absOffset;
        }

        //qDebug() << "appending" << length << "bytes";
        m_data.append(data.mid(startOffset, length));
    }
}

/*!
    Returns the absoluate offset for \a sparseOffset, taking all reserved memory into account.
*/
int QTlvReader::absoluteOffset(int sparseOffset) const
{
    int absoluteOffset = sparseOffset;
    foreach (int offset, m_reservedMemory.keys()) {
        if (offset <= absoluteOffset)
            absoluteOffset += m_reservedMemory.value(offset);
    }

    return absoluteOffset;
}

QTM_END_NAMESPACE
