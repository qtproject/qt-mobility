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
#include <nfctag.h>
#include "qnearfieldutility_symbian.h"
#include "qnearfieldtagtype1_symbian_p.h"

QTM_BEGIN_NAMESPACE

static const int ReadAllBytes = 124;
static const int ReadSegmentBytes = 131;
static const int BlockOpreationBytes = 11;
static const int WriteBlockBytes = 14; // 8 bytes data + 4 bytes UI + 2 bytes CRC

/*!
    \class QNearFieldTagType1Symbian
    \brief The QNearFieldTagType1Symbian class provides symbian backend implementation for communicating with an NFC Tag
           Type 1 tag.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

/*!
    Constructs a new tag type 1 near field target with \a tag and \a parent.
*/
QNearFieldTagType1Symbian::QNearFieldTagType1Symbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTagType1(parent), QNearFieldTagImpl(tag)
{
}

/*!
    Destructor
*/
QNearFieldTagType1Symbian::~QNearFieldTagType1Symbian()
{
    delete mTag;
}

QByteArray QNearFieldTagType1Symbian::uid() const
{
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readIdentification()
{
    CNearFieldTagType1 * tagType1 = mTag->CastToTagType1();

    if (tagType1)
    {
        QByteArray result;
        CommonUtil::TDesC82QByteArray(tagType1->ReadIdentification(), result);
        return result;
    }
    else
    {
        return QByteArray();
    }
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readAll()
{
    CNearFieldTagType1 * tagType1 = mTag->CastToTagType1();
    QByteArray result;

    if (tagType1)
    {
        TBuf8<ReadAllBytes> response;
        int error = tagType1->ReadAll(response);
        if (error == KErrNone)
        {
            CommonUtil::TDesC82QByteArray(response, result);
        }
    }

    return result;
}

/*!
    \reimp
*/
quint8 QNearFieldTagType1Symbian::readByte(quint8 address)
{
    CNearFieldTagType1 * tagType1 = mTag->CastToTagType1();
    quint8 result = 0;

    if (tagType1)
    {
        // TODO: ask Aaron to add error code define
        int error = tagType1->ReadByte(address, result);
    }

    return result;
}

/*!
    \reimp
*/
bool QNearFieldTagType1Symbian::writeByte(quint8 address, quint8 data, WriteMode mode)
{
    CNearFieldTagType1 * tagType1 = mTag->CastToTagType1();
    int error = KErrNotSupported;

    if (tagType1)
    {
        error = (mode == EraseAndWrite) ? tagType1->WriteByteErase(address, data) 
                                        : tagType1->WriteByteNoErase(address, data);
    }
    
    return (error == KErrNone);
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readSegment(quint8 segmentAddress)
{
    CNearFieldTagType1 * tagType1 = mTag->CastToTagType1();
    QByteArray result;

    if (tagType1)
    {
        TBuf8<ReadSegmentBytes> response;
        if (KErrNone == tagType1->ReadSegment(segmentAddress, response))
        {
            CommonUtil::TDesC82QByteArray(response, result);
        }
    }
    
    return result;
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::readBlock(quint8 blockAddress)
{
    CNearFieldTagType1 * tagType1 = mTag->CastToTagType1();
    QByteArray result;
    if (tagType1)
    {
        TBuf8<BlockOpreationBytes> response;
        if (KErrNone == tagType1->ReadBlock(blockAddress, response))
        {
            CommonUtil::TDesC82QByteArray(response, result);
        }
    }
    return result;
}

/*!
    \reimp
*/
bool QNearFieldTagType1Symbian::writeBlock(quint8 blockAddress, const QByteArray &data,
                        WriteMode mode)
{
    int error = KErrNotSupported;
    CNearFieldTagType1 * tagType1 = mTag->CastToTagType1();
    
    if (tagType1)
    {
        TBuf8<WriteBlockBytes> cmdData;
        CommonUtil::QByteArray2TDes8(data, cmdData);
        error = (mode == EraseAndWrite) ? tagType1->WriteBlockErase(blockAddress, cmdData)
                                        : tagType1->WriteBlockNoErase(blockAddress, cmdData);
    }
    return (error == KErrNone);
}
    
bool QNearFieldTagType1Symbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

QList<QNdefMessage> QNearFieldTagType1Symbian::ndefMessages()
{
    return _ndefMessages();
}

void QNearFieldTagType1Symbian::setNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages);
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType1Symbian::sendCommand(const QByteArray &command)
{
#if 0
    QByteArray result;
    if (!command.isEmpty())
    {
        quint8 commandCode = command.at(0);
        switch(commandCode)
        {
            case 0x00:
            {
                result = readAll();
                break;
            }
            case 0x78:
            {
                result = readIdentification();
                break;
            }
            case 0x01:
            {
                result.append(readByte(command.at(1)));
                break;
            }
            case 0x53:
            {
                // TODO: need add parameters to write operation so that we can get entire
                // response of the command.
                result


            

#endif            
}

/*!
    \reimp
*/
QList<QByteArray> QNearFieldTagType1Symbian::sendCommands(const QList<QByteArray> &commands)
{
}

#include "moc_qnearfieldtagtype1_symbian_p.cpp"

QTM_END_NAMESPACE
