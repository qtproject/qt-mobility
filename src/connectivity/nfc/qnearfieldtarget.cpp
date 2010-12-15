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

#include "qnearfieldtarget.h"
#include "qnearfieldtarget_p.h"
#include "qndefmessage.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

#include <QtCore/QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QNearFieldTarget
    \brief The QNearFieldTarget class provides an interface for communicating with a target
           device.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    QNearFieldTarget provides a generic interface for communicating with an NFC target device.
    Both NFC Forum devices and NFC Forum Tag targets are supported by this class.  All target
    specific classes subclass this class.

    The type() function can be used to get the type of the target device.  The uid() function
    returns the unique identifier of the target.  The AccessMethods flags returns from the
    accessMethods() function can be tested to determine which access methods are supported by the
    target.

    If the target supports NdefAccess, hasNdefMessage() can be called to test if the target has a
    stored NDEF message, ndefMessages() and setNdefMessages() functions can be used to get and set
    the NDEF message.

    If the target supports ApduAccess, sendApduCommand() can be used to send a single APDU command
    to the target and retrieve the response.  sendApduCommands() can be used to send
    multiple APDU commands to the target and retrieve all of the responses.

    If the target supports TagTypeSpecificAccess, sendCommand() can be used to send a single
    proprietary command to the target and retrieve the response.  sendCommands() can be used to
    send multiple proprietary commands to the target and retrieve all of the responses.

    If the target supports LlcpAccess, the QLlcpSocket class can be used to connected to a
    service provided by the target.
*/

/*!
    \enum QNearFieldTarget::Type

    This enum describes the type of tag the target is detected as.

    \value AnyTarget        This value is only used when registering handlers to indicate that any
                            compatible target can be used.
    \value ProprietaryTag   An unidentified proprietary target tag.
    \value NfcTagType1      An NFC tag type 1 target.
    \value NfcTagType2      An NFC tag type 2 target.
    \value NfcTagType3      An NFC tag type 3 target.
    \value NfcTagType4      An NFC tag type 4 target.
    \value MifareTag        A Mifare target.
*/

/*!
    \enum QNearFieldTarget::AccessMethod

    This enum describes the access methods a near field target supports.

    \value NdefAccess               The target supports NDEF records using ndefMessages() and
                                    setNdefMessages().
    \value ApduAccess               The target supports APDU access using sendApduCommand() and
                                    sendApduCommands().
    \value TagTypeSpecificAccess    The target supports sending tag type specific commands using
                                    sendCommand() and sendCommands().
    \value LlcpAccess               The target supports peer-to-peer LLCP communication.
*/

/*!
    \enum QNearFieldTarget::Error

    This enum describes the error codes that that a near field target reports.

    \value NoError                  No error has occurred.
    \value UnsupportedError         The requested operation is unsupported by this near field
                                    target.
    \value TargetOutOfRangeError    The target is no longer within range.
    \value NoResponseError          The target did not respond.
    \value ChecksumMismatchError    The checksum has detected a corrupted response.
    \value InvalidParametersError   Invalid parameters were passed to a tag type specific function.
    \value NdefReadError            Failed to read NDEF messages from the target.
    \value NdefWriteError           Failed to write NDEF messages to the target.
*/

/*!
    \fn qNfcChecksum(const char *data, uint len)

    \relates QNearFieldTarget

    Returns the NFC checksum of the first \a len bytes of \a data.
*/
#include "checksum_p.h"

/*!
    \fn void QNearFieldTarget::disconnected()

    This signal is emitted when the near field target moves out of proximity.
*/

/*!
    \fn void QNearFieldTarget::ndefMessageRead(const QNdefMessage &message)

    This signal is emitted when a complete NDEF \a message has been read from the target.

    \sa readNdefMessages()
*/

/*!
    \fn void QNearFieldTarget::ndefMessagesWritten()

    This signal is emitted when NDEF messages have been sucessfully written to the target.

    \sa writeNdefMessages()
*/

/*!
    \fn void QNearFieldTarget::requestCompleted(const QNearFieldTarget::RequestId &id)

    This signal is emitted when a request \a id completes.

    \sa sendCommand()
*/

/*!
    \fn void QNearFieldTarget::error(QNearFieldTarget::Error error)

    This signal is emitted when an error occurs. The \a error parameter describes the error.
*/

/*!
    Constructs a new invalid request id handle.
*/
QNearFieldTarget::RequestId::RequestId()
{
}

/*!
    Constructs a new request id handle that is a copy of \a other.
*/
QNearFieldTarget::RequestId::RequestId(const RequestId &other)
:   d(other.d)
{
}

/*!
    \internal
*/
QNearFieldTarget::RequestId::RequestId(RequestIdPrivate *p)
:   d(p)
{
}

/*!
    Destroys the request id handle.
*/
QNearFieldTarget::RequestId::~RequestId()
{
}

/*!
    Returns true if this is a valid request id; otherwise returns false.
*/
bool QNearFieldTarget::RequestId::isValid() const
{
    return d;
}

/*!
    Returns the current reference count of the request id.
*/
int QNearFieldTarget::RequestId::refCount() const
{
    if (d)
        return d->ref;

    return 0;
}

/*!
    \internal
*/
bool QNearFieldTarget::RequestId::operator<(const RequestId &other) const
{
    return d < other.d;
}

/*!
    \internal
*/
bool QNearFieldTarget::RequestId::operator==(const RequestId &other) const
{
    return d == other.d;
}

/*!
    Assigns a copy of \a other to this request id and returns a reference to this request id.
*/
QNearFieldTarget::RequestId &QNearFieldTarget::RequestId::operator=(const RequestId &other)
{
    d = other.d;
    return *this;
}

/*!
    Constructs a new near field target with \a parent.
*/
QNearFieldTarget::QNearFieldTarget(QObject *parent)
:   QObject(parent), d_ptr(new QNearFieldTargetPrivate)
{
    qRegisterMetaType<RequestId>("QNearFieldTarget::RequestId");
}

/*!
    \fn QByteArray QNearFieldTarget::uid() const = 0

    Returns the UID of the near field target.
*/

/*!
    Returns the URL of the near field target.
*/
QUrl QNearFieldTarget::url() const
{
    return QUrl();
}

/*!
    \fn QNearFieldTarget::Type QNearFieldTarget::type() const = 0

    Returns the type of tag type of this near field target.
*/

/*!
    \fn QNearFieldTarget::AccessMethods QNearFieldTarget::accessMethods() const = 0

    Returns the access methods support by this near field target.
*/

/*!
    Returns true if at least one NDEF message is stored on the near field target; otherwise returns
    false.
*/
bool QNearFieldTarget::hasNdefMessage()
{
    return false;
}

/*!
    Starts reading NDEF messages stored on the near field target. An ndefMessageRead() signal will
    be emitted for each NDEF message.  If an error occurs the error() signal will be emitted.
*/
void QNearFieldTarget::readNdefMessages()
{
    emit error(UnsupportedError);
}

/*!
    Writes the NDEF messages in \a messages to the target. The ndefMessagesWritten() signal will be
    emitted when the write operation completes sucessfully; otherwise the error() signal is
    emitted.
*/
void QNearFieldTarget::writeNdefMessages(const QList<QNdefMessage> &messages)
{
    Q_UNUSED(messages);

    emit error(UnsupportedError);
}

/*!
    Sends the APDU \a command to the near field target. The apduCommandCompleted() signal will be
    emitted if the command completes sucessfully; otherwise the error() signal will be emitted.
*/
void QNearFieldTarget::sendApduCommand(const QByteArray &command)
{
    Q_UNUSED(command);

    emit error(UnsupportedError);
}

/*!
    Sends multiple APDU \a commands to the near field target. The apduCommandsCompleted() signal
    will be emitted if the command completes sucessfully; otherwise the error() signal will be
    emitted.
*/
void QNearFieldTarget::sendApduCommands(const QList<QByteArray> &commands)
{
    Q_UNUSED(commands);

    emit error(UnsupportedError);
}

/*!
    Sends \a command to the near field target. The commandCompleted() signal will be emitted if the
    command completes sucessfully; otherwise the error() signal will be emitted.
*/
QNearFieldTarget::RequestId QNearFieldTarget::sendCommand(const QByteArray &command)
{
    Q_UNUSED(command);

    emit error(UnsupportedError);

    return RequestId();
}

/*!
    Sends multiple \a commands to the near field target. The commandsCompleted() signal will be
    emitted if the commands complete sucessfully; otherwise the error() signal will be emitted.
*/
QNearFieldTarget::RequestId QNearFieldTarget::sendCommands(const QList<QByteArray> &commands)
{
    Q_UNUSED(commands);

    emit error(UnsupportedError);

    return RequestId();
}

/*!
    Waits up to \a msecs milliseconds for the request \a id to complete. Returns true if the
    request completes sucessfully and the requestCompeted() signal is emitted; otherwise returns
    false.
*/
bool QNearFieldTarget::waitForRequestCompleted(const RequestId &id, int msecs)
{
    Q_UNUSED(msecs);

    Q_D(QNearFieldTarget);

    return d->m_decodedResponses.contains(id);
}

/*!
    Returns the decoded response for request \a id. If the request is unknown or has not yet been
    completed an invalid QVariant is returned.
*/
QVariant QNearFieldTarget::requestResponse(const RequestId &id)
{
    Q_D(QNearFieldTarget);

    return d->m_decodedResponses.value(id);
}

/*!
    Sets the decoded response for request \a id to \a response.

    \sa requestResponse()
*/
void QNearFieldTarget::setResponseForRequest(const QNearFieldTarget::RequestId &id,
                                             const QVariant &response)
{
    Q_D(QNearFieldTarget);

    QMutableMapIterator<RequestId, QVariant> i(d->m_decodedResponses);
    while (i.hasNext()) {
        i.next();

        // no more external references
        if (i.key().refCount() == 1)
            i.remove();
    }

    d->m_decodedResponses.insert(id, response);

    emit requestCompleted(id);
}

/*!
    Handles the \a response received for the request \a id. Returns true if the response is
    handled; otherwise returns false.

    Class reimplementing this virtual function should call the base class implementation to ensure
    that requests initiated by those classes are handled correctly.

    The default implementation stores the response such that it can be retrieved by
    requestResponse().
*/
bool QNearFieldTarget::handleResponse(const RequestId &id, const QByteArray &response)
{
    setResponseForRequest(id, response);

    return true;
}

#include "moc_qnearfieldtarget.cpp"

QTM_END_NAMESPACE
