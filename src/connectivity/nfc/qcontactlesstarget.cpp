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

#include "qcontactlesstarget.h"
#include "qndefmessage.h"

#include <QtCore/QString>
#include <QtCore/QUrl>

QTM_BEGIN_NAMESPACE

/*!
    \class QContactlessTarget
    \brief The QContactlessTarget class provides an interface for communicating with a target
           device.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    QContactlessTarget provides a generic interface for communicating with an NFC target device.
    All target specific classes subclass this class.
*/

/*!
    \enum QContactlessTarget::TagType

    This enum describes the type of tag the target is detected as.

    \value NfcTagType1  An NFC tag type 1 target
    \value NfcTagType2  An NFC tag type 2 target
    \value NfcTagType3  An NFC tag type 3 target
    \value NfcTagType4  An NFC tag type 4 target
    \value Mifare       A Mifare target
    \value Proprietary  An unidentified proprietary target tag.
*/

/*!
    \enum QContactlessTarget::TagAccessMethod

    This enum describes the access methods a contactless target supports.

    \value NdefAccess               The target supports NDEF records using ndefMessage() and
                                    setNdefMessage().
    \value AdpuAccess               The target supports ADPU access using sendAdpuCommand() and
                                    sendApduCommands().
    \value TagTypeSpecificAccess    The target supports sending tag type specific commands using
                                    sendCommand() and sendCommands().
    \value LlcpAccess               The target supports peer-to-peer LLCP communication.
*/

class QContactlessTargetPrivate
{
public:
    QString uid;
    QUrl url;
    QContactlessTarget::TagType tagType;
    QContactlessTarget::TagAccessMethods accessMethods;
};

/*!
    Constructs a new contactless target.
*/
QContactlessTarget::QContactlessTarget()
{
}

/*!
    Returns the UID of the contactless target.
*/
QString QContactlessTarget::uid() const
{
    return d->uid;
}

/*!
    Returns the URL of the contactless target.
*/
QUrl QContactlessTarget::url() const
{
    if (!(d->accessMethods & QContactlessTarget::NdefAccess))
        return QUrl();

    return d->url;
}

/*!
    Returns the type of tag type of this contactless target.
*/
QContactlessTarget::TagType QContactlessTarget::type() const
{
    return d->tagType;
}

/*!
    Returns the access methods support by this contactless target.
*/
QContactlessTarget::TagAccessMethods QContactlessTarget::accessMethods() const
{
    return d->accessMethods;
}

/*!
    Returns true if an NDEF message is stored on the contactless target; otherwise returns false.
*/
bool QContactlessTarget::hasNdefMessage() const
{
    return false;
}

/*!
    Returns the NDEF message stored on the contactless target.
*/
QNdefMessage QContactlessTarget::ndefMessage() const
{
    return QNdefMessage();
}

/*!
    Sets the NDEF message on the contactless target to \a message.
*/
void QContactlessTarget::setNdefMessage(const QNdefMessage &message)
{
    Q_UNUSED(message);
}

/*!
    Sends the APDU \a command to the contactless target and returns the result.
*/
QByteArray QContactlessTarget::sendApduCommand(const QByteArray &command)
{
    Q_UNUSED(command);

    return QByteArray();
}

/*!
    Sends multiple APDU \a commands to the contactless target and returns the results.
*/
QList<QByteArray> QContactlessTarget::sendApduCommands(const QList<QByteArray> &commands)
{
    Q_UNUSED(commands);

    return QList<QByteArray>();
}

/*!
    Sends \a command to the contactless target and returns the result.
*/
QVariant QContactlessTarget::sendCommand(const QVariant &command)
{
    Q_UNUSED(command);

    return QVariant();
}

/*!
    Sends multiple \a commands to the contactless target and returns the result.
*/
QVariantList QContactlessTarget::sendCommands(const QVariantList &commands)
{
    Q_UNUSED(commands);

    return QVariantList();
}

QTM_END_NAMESPACE
