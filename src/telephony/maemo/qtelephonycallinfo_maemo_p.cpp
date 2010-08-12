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

#include <qmobilityglobal.h>

#include "qtelephony.h"
#include "maemo/cli-constants.h"
#include "maemo/qtelephonycallinfo_maemo_p.h"

using namespace Tp;

QTM_BEGIN_NAMESPACE

bool QTelephonyCallInfoPrivate::isCall( const QVariantMap &properties)
{
    if(properties.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"))){
        QString type = properties.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString();
        /**************************
        check for Mediastream
        **************************/
        if(type == TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA){
            return true;
        }
        /************************************************************
        check for Text Telephony API doesn't care about SMS (no call)
        *************************************************************/
        else if(type == TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT){
            return false;
        }
    }
    return false;
}

QTelephonyCallInfoPrivate::QTelephonyCallInfoPrivate()
            : _subType("")
            , telepathychannel(0)
{
}

QTelephonyCallInfoPrivate::QTelephonyCallInfoPrivate(const QTelephonyCallInfoPrivate &other)
            : QSharedData(other)
            , _subType(other._subType)
            , telepathychannel(other.telepathychannel)
{
}

QTelephonyCallInfoPrivate::QTelephonyCallInfoPrivate(Tp::ChannelPtr channel)
    : _subType("")
    , telepathychannel(channel)
{

}

QString QTelephonyCallInfoPrivate::remotePartyIdentifier() const
{
    QString ret = "";
    if(telepathychannel){
        if(telepathychannel->properties().contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetID")))
            ret = telepathychannel->properties().value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetID")).toString();
    }
    return ret;
}

QTelephonyEvents::CallType QTelephonyCallInfoPrivate::type() const
{
    QTelephonyEvents::CallType ret = QTelephonyEvents::Any;
    if(telepathychannel){
        if(telepathychannel->properties().contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"))){
            QString type = telepathychannel->properties().value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString();

            /**************************
            check for Mediastream
            **************************/
            if(type == TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA){
                //In this case we need to check video or audio
                bool initialaudio = false;
                bool initialvideo = false;
                if(telepathychannel->properties().contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA ".InitialAudio")))
                    initialaudio = telepathychannel->properties().value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA ".InitialAudio")).toBool();
                if(telepathychannel->properties().contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA ".InitialVideo")))
                    initialvideo = telepathychannel->properties().value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA ".InitialVideo")).toBool();
                if(initialvideo) {
                    ret = QTelephonyEvents::Video;
                }
                else if(initialaudio)
                    ret = QTelephonyEvents::Voice;
            }
        }
    }


    return ret;
}

QString QTelephonyCallInfoPrivate::subType() const
{
    return _subType;
}

QTelephonyEvents::CallStatus QTelephonyCallInfoPrivate::status() const
{
    QTelephonyEvents::CallStatus ret = QTelephonyEvents::Idle;
    if(telepathychannel)
        ret = telepathychannel->getStatus();
    return ret;
}

QTM_END_NAMESPACE
