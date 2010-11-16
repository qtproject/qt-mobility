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
#include "qnearfieldtagtype3_symbian_p.h"
#include <nfctype3connection.h>
#include <QtEndian>

QTM_BEGIN_NAMESPACE

QNearFieldTagType3Symbian::QNearFieldTagType3Symbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTagType3(parent), QNearFieldTagImpl(tag)
{
}

QNearFieldTagType3Symbian::~QNearFieldTagType3Symbian()
{
}

QByteArray QNearFieldTagType3Symbian::uid() const
{
}

bool QNearFieldTagType3Symbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

QList<QNdefMessage> QNearFieldTagType3Symbian::ndefMessages()
{
    return _ndefMessages();
}

void QNearFieldTagType3Symbian::setNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages); 
}

QByteArray QNearFieldTagType3Symbian::sendCommand(const QByteArray &command)
{
}

QList<QByteArray> QNearFieldTagType3Symbian::sendCommands(const QList<QByteArray> &commands)
{
}

quint16 QNearFieldTagType3Symbian::systemCode()
{
}

QList<quint16> QNearFieldTagType3Symbian::services()
{
}

int QNearFieldTagType3Symbian::serviceMemorySize(quint16 serviceCode)
{
}

QByteArray QNearFieldTagType3Symbian::serviceData(quint16 serviceCode)
{
}

void QNearFieldTagType3Symbian::writeServiceData(quint16 serviceCode, const QByteArray &data)
{
}

QMap<quint16, QByteArray> QNearFieldTagType3Symbian::check(const QMap<quint16, QList<unsigned int> > &serviceBlockList)
{
    quint8 numberOfBlocks;
    QByteArray command;
    command.append(0x06); // command code
    command.append(serviceBlockList2CmdParam(serviceBlockList, numberOfBlocks));
    if (command.count() > 1)
    {
        return  checkResponse2ServiceBlockList(serviceBlockList ,_sendCommand(command, 100*1000, 12+16*numberOfBlocks)); 
    }
    else
    {
        return QMap<quint16, QByteArray>();
    }
}

void QNearFieldTagType3Symbian::update(const QMap<quint16, QList<unsigned int> > &serviceBlockList,
                                       const QByteArray &data)  
{
}

const QByteArray& QNearFieldTagType3Symbian::getIDm()
{
    if (mIDm.isEmpty())
    {
    // this is the first time to get IDm
        CNearFieldTag * tag = mTag->CastToTag();

        if (tag)
        {
            CNfcType3Connection * connection = static_cast<CNfcType3Connection *>(tag->TagConnection());
            TBuf8<8> IDm;
            TInt error = connection->GetIDm(IDm);
            if (KErrNone == error)
            {
                mIDm = QNFCNdefUtility::FromTDesCToQByteArray(IDm);  
            }
        }
    }
    return mIDm;
}

QByteArray QNearFieldTagType3Symbian::serviceBlockList2CmdParam(const QMap<quint16, QList<unsigned int> > &serviceBlockList, quint8& numberOfBlocks)
{
    QByteArray command;
    command.append(getIDm());
    numberOfBlocks = 0;

    if (command.isEmpty())
    {
        // can't get IDm
        return command;
    }

    quint8 numberOfServices = serviceBlockList.keys().count();

    if ((numberOfServices > 16) || (numberOfServices < 1))
    {
        // out of range of services number
        return QByteArray();
    }
    else
    {
        command.append(numberOfServices);
    } 

    quint8 serviceCodeListOrder = 0;
    QByteArray serviceCodeList;
    QByteArray blockList;
    foreach(const quint16 serviceCode, serviceBlockList.keys())
    {
        serviceCodeList.append(reinterpret_cast<const char *>(&serviceCode), sizeof(quint16));
        
        numberOfBlocks += serviceBlockList.value(serviceCode).count();
        if (numberOfBlocks > 12)
        {
            // out of range of block number
            return QByteArray();
        }

        foreach(const unsigned int blockNumber, serviceBlockList.value(serviceCode))
        {
            if (blockNumber > 255)
            {
                // 3 bytes format
                blockList.append(0x00 | (serviceCodeListOrder & 0x0F));
                quint16 blkNum = blockNumber;
                blkNum = qToLittleEndian(blkNum);
                blockList.append(reinterpret_cast<const char *>(&blkNum), sizeof(quint16));
            }
            else // 2 bytes format
            {
                blockList.append(0x80 | (serviceCodeListOrder & 0x0F));
                quint8 blkNum = blockNumber; 
                blockList.append(blkNum);
            }
        }

        serviceCodeList.append(serviceCodeListOrder++);
    }

    if (numberOfBlocks < 1)
    {
        // out of range of block number
        return QByteArray();
    }        

    command.append(serviceCodeList);
    command.append(numberOfBlocks);
    command.append(blockList);
    return command;
}

QMap<quint16, QByteArray> QNearFieldTagType3Symbian::checkResponse2ServiceBlockList(const QMap<quint16, QList<unsigned int> > &serviceBlockList, const QByteArray& response)
{
    QMap<quint16, QByteArray> result;
    // at least, the response should contain resp code + IDM + status flags
    if (response.count() < 11)
    {
        return result;
    }
    
    if ((response.at(0) != 0x07) || (response.mid(1,8) != getIDm()) || (response.at(10) != 0))
    {
        return result;
    }

    quint32 index = 12;
    foreach(const quint16 serviceCode, serviceBlockList.keys())
    {
        quint8 blockCount = serviceBlockList.value(serviceCode).count();
        result.insert(serviceCode, response.mid(index, 16*blockCount));
        index+=16*blockCount;
    }
    return result;
}
     
#include "moc_qnearfieldtagtype3_symbian_p.cpp"
QTM_END_NAMESPACE
