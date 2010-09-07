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

#include <qmobilityglobal.h>

#include "qtelephony.h"
#include "maemo/constants.h"
#include "maemo/qtelephonycallinfo_maemo_p.h"

using namespace DBus;

QTM_BEGIN_NAMESPACE

QTelephonyCallInfoPrivate::QTelephonyCallInfoPrivate()
            : telepathychannel(0)
{
}

QTelephonyCallInfoPrivate::QTelephonyCallInfoPrivate(const QTelephonyCallInfoPrivate &other)
            : QSharedData(other)
            , telepathychannel(other.telepathychannel)
{
}

QTelephonyCallInfoPrivate::QTelephonyCallInfoPrivate(DBus::ChannelPtr channel)
    : telepathychannel(channel)
{
}

QString QTelephonyCallInfoPrivate::remotePartyIdentifier() const
{
    if(telepathychannel)
        return telepathychannel->getRemotePartyIdentifier();

    return QString("");
}

unsigned int QTelephonyCallInfoPrivate::type() const
{
    unsigned int ret = 0;
    if(telepathychannel)
        ret = telepathychannel->getCallType();
    return ret;
}

QString QTelephonyCallInfoPrivate::subType() const
{
    QString ret = "";
    if(telepathychannel)
        ret = telepathychannel->getSubType();
    return ret;
}

QTelephony::CallStatus QTelephonyCallInfoPrivate::status() const
{
    QTelephony::CallStatus ret = QTelephony::Idle;
    if(telepathychannel)
        ret = telepathychannel->getStatus();
    return ret;
}

QTelephony::Direction QTelephonyCallInfoPrivate::direction() const
{
    QTelephony::Direction ret = QTelephony::Received;
    if(telepathychannel){
        /*
        1 = incoming
        2 = outgoing*/
        if(telepathychannel->getDirection() == 2)
            ret = QTelephony::Dialed;
    }
    return ret;
}

QTM_END_NAMESPACE
