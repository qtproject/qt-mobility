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
#ifndef QNEARFIELDTAGTYPE3SYMBIAN_H
#define QNEARFIELDTAGTYPE3SYMBIAN_H

#include <qnearfieldtagtype3.h>
#include "nearfieldtagimpl_symbian.h"
#include "nearfieldndeftarget_symbian.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QNearFieldTagType3Symbian : public QNearFieldTagType3, private QNearFieldTagImpl<QNearFieldTagType3Symbian>
{
    Q_OBJECT
public:

    explicit QNearFieldTagType3Symbian(MNearFieldTarget *tag, QObject *parent = 0);

    ~QNearFieldTagType3Symbian();

    virtual QByteArray uid() const;

    void setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
    {
        _setAccessMethods(accessMethods);
    }

    QNearFieldTarget::AccessMethods accessMethods() const
    {
        return _accessMethods();
    }

    bool hasNdefMessage();
    void ndefMessages();
    void setNdefMessages(const QList<QNdefMessage> &messages);

    quint16 systemCode();
    QList<quint16> services();
    int serviceMemorySize(quint16 serviceCode);

    QByteArray serviceData(quint16 serviceCode);
    void writeServiceData(quint16 serviceCode, const QByteArray &data);

    virtual QMap<quint16, QByteArray> check(const QMap<quint16, QList<unsigned int> > &serviceBlockList);
    virtual void update(const QMap<quint16, QList<unsigned int> > &serviceBlockList,
                        const QByteArray &data);  

    RequestId sendCommand(const QByteArray &command);
    RequestId sendCommands(const QList<QByteArray> &commands);
private:
    const QByteArray& getIDm();
    QByteArray serviceBlockList2CmdParam(const QMap<quint16, QList<unsigned int> > &serviceBlockList, quint8& numberOfBlocks);
    QMap<quint16, QByteArray> checkResponse2ServiceBlockList(const QMap<quint16, QList<unsigned int> > &serviceBlockList, const QByteArray& response);
    bool handleTagOperationResponse(const RequestId &id, const QByteArray &command, const QByteArray &response);
private:
    QByteArray mIDm;
    
    friend class QNearFieldTagImpl<QNearFieldTagType3Symbian>;
};
    
QTM_END_NAMESPACE
QT_END_HEADER
#endif // QNEARFIELDTAGTYPE3SYMBIAN_H

