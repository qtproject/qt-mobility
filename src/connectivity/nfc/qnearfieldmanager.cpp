/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qnearfieldmanager.h"
#include "qnearfieldmanager_p.h"

#if defined(QT_SIMULATOR)
#include "qnearfieldmanager_simulator_p.h"
#elif defined(Q_OS_SYMBIAN)
#include "qnearfieldmanager_symbian_p.h"
#elif defined(Q_WS_MAEMO_6) || defined (Q_WS_MEEGO)
#include "qnearfieldmanager_meego_p.h"
#else
#include "qnearfieldmanagerimpl_p.h"
#endif

#include <QtCore/QMetaType>
#include <QtCore/QMetaMethod>

QTM_BEGIN_NAMESPACE

/*!
    \class QNearFieldManager
    \brief The QNearFieldManager class provides access to notifications for NFC events.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    NFC Forum devices support two modes of communications.  The first mode, peer-to-peer
    communications, is used to communicate between two NFC Forum devices.  The second mode,
    master/slave communications, is used to communicate between an NFC Forum device and an NFC
    Forum Tag or Contactess Card.  The targetDetected() signal is emitted when a target device
    enters communications range.  Communications can be initiated from the slot connected to this
    signal.

    NFC Forum devices generally operate as the master in master/slave communications.  Some devices
    are capable of operate as the slave, so called Card Emulation mode.  In this mode the local NFC
    device emulates a NFC Forum Tag or Contactless Card and can be used to perform transactions.
    The transaction happens entirely within a secure element on the device and only a notification
    of the transaction is provided.  The transactionDetected() signal is emitted whenever a
    transaction occurs.

    NFC Forum Tags can contain one or more messages in a standardized format.  These messages are
    encapsulated in the QNdefMessage class.  Use the registerTargetDetectedHandler() functions to
    register message handlers with particular criteria.  Handlers can be unregistered with the
    unregisterTargetDetectedHandler() function.
*/

/*!
    \fn void QNearFieldManager::targetDetected(QNearFieldTarget *target)

    This signal is emitted whenever a target is detected. The \a target parameter represents the
    detected target.

    This signal will be emitted for all detected targets.

    QNearFieldManager maintains ownership of \a target, however, it will not be destroyed until
    the QNearFieldManager destructor is called. Ownership may be transferred by calling
    setParent().

    Do not delete \a target from the slot connected to this signal, instead call deleteLater().

    \note that if \a target is deleted before it moves out of proximity the targetLost() signal
    will not be emitted.

    \sa targetLost()
*/

/*!
    \fn void QNearFieldManager::targetLost(QNearFieldTarget *target)

    This signal is emitted whenever a target moves out of proximity. The \a target parameter
    represents the lost target.

    Do not delete \a target from the slot connected to this signal, instead use deleteLater().

    \sa QNearFieldTarget::disconnected()
*/

/*!
    \fn void QNearFieldManager::transactionDetected(const QByteArray &applicationIdentifier)

    This signal is emitted when ever a transaction is performed with the application identified by
    \a applicationIdentifier.

    The \a applicationIdentifier is a byte array of up to 16 bytes as defined by ISO 7816-4 and
    uniquely identifies the application and application vendor that was involved in the
    transaction.
*/

/*!
    \fn int QNearFieldManager::registerTargetDetectedHandler(QObject *object, const char *method)

    Registers \a object to receive notifications on \a method when a tag has been detected and has
    an NDEF record that matches template argument.  The \a method on \a object should have the
    prototype 'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.

    \note The \i target parameter of \a method may not be available on all platforms, in which case
    \i target will be 0.
*/

/*!
    Constructs a new near field manager with \a parent.
*/
QNearFieldManager::QNearFieldManager(QObject *parent)
:   QObject(parent), d_ptr(new QNearFieldManagerPrivateImpl)
{
    connect(d_ptr, SIGNAL(targetDetected(QNearFieldTarget*)),
            this, SIGNAL(targetDetected(QNearFieldTarget*)));
    connect(d_ptr, SIGNAL(targetLost(QNearFieldTarget*)),
            this, SIGNAL(targetLost(QNearFieldTarget*)));
}

/*!
    \internal

    Constructs a new near field manager with the specified \a backend and with \a parent.

    \note: This constructor is only enable for internal builds and is used for testing the
    simulator backend.
*/
QNearFieldManager::QNearFieldManager(QNearFieldManagerPrivate *backend, QObject *parent)
:   QObject(parent), d_ptr(backend)
{
    connect(d_ptr, SIGNAL(targetDetected(QNearFieldTarget*)),
            this, SIGNAL(targetDetected(QNearFieldTarget*)));
    connect(d_ptr, SIGNAL(targetLost(QNearFieldTarget*)),
            this, SIGNAL(targetLost(QNearFieldTarget*)));
}

/*!
    Destroys the near field manager.
*/
QNearFieldManager::~QNearFieldManager()
{
    delete d_ptr;
}

/*!
    Starts detecting targets of type \a targetTypes. Causes the targetDetected() signal to be
    emitted when a target with a type in \a targetTypes is within proximity. If \a targetTypes is
    empty targets of all types will be detected.

    \sa stopTargetDetection()
*/
void QNearFieldManager::startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes)
{
    Q_D(QNearFieldManager);

    if (targetTypes.isEmpty())
        d->startTargetDetection(QList<QNearFieldTarget::Type>() << QNearFieldTarget::AnyTarget);
    else
        d->startTargetDetection(targetTypes);
}

/*!
    \overload

    Starts detecting targets of type \a targetType. Causes the targetDetected() signal to be
    emitted when a target with the type \a targetType is within proximity.
*/
void QNearFieldManager::startTargetDetection(QNearFieldTarget::Type targetType)
{
    startTargetDetection(QList<QNearFieldTarget::Type>() << targetType);
}

/*!
    Stops detecting targets.  The targetDetected() signal will no longer be emitted until another
    call to startTargetDetection() is made.
*/
void QNearFieldManager::stopTargetDetection()
{
    Q_D(QNearFieldManager);

    d->stopTargetDetection();
}

static QMetaMethod methodForSignature(QObject *object, const char *method)
{
    QByteArray normalizedMethod = QMetaObject::normalizedSignature(method);

    if (!QMetaObject::checkConnectArgs(SIGNAL(targetDetected(QNdefMessage,QNearFieldTarget*)),
                                       normalizedMethod)) {
        qWarning("Signatures do not match: %s:%d\n", __FILE__, __LINE__);
        return QMetaMethod();
    }

    quint8 memcode = (normalizedMethod.at(0) - '0') & 0x03;
    normalizedMethod = normalizedMethod.mid(1);

    int index;
    switch (memcode) {
    case QSLOT_CODE:
        index = object->metaObject()->indexOfSlot(normalizedMethod.constData());
        break;
    case QSIGNAL_CODE:
        index = object->metaObject()->indexOfSignal(normalizedMethod.constData());
        break;
    case QMETHOD_CODE:
        index = object->metaObject()->indexOfMethod(normalizedMethod.constData());
        break;
    default:
        index = -1;
    }

    if (index == -1)
        return QMetaMethod();

    return object->metaObject()->method(index);
}

/*!
    Registers \a object to receive notifications on \a method when a tag has been detected and has
    an NDEF record that matchings \a typeNameFormat and \a type.  The \a method on \a object should
    have the prototype
    'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.

    \note The \i target parameter of \a method may not be available on all platforms, in which case
    \i target will be 0.
*/

int QNearFieldManager::registerTargetDetectedHandler(QNdefRecord::TypeNameFormat typeNameFormat,
                                                     const QByteArray &type,
                                                     QObject *object, const char *method)
{
    QMetaMethod metaMethod = methodForSignature(object, method);
    if (!metaMethod.enclosingMetaObject())
        return -1;

    QNdefFilter filter;
    filter.appendRecord(typeNameFormat, type);

    Q_D(QNearFieldManager);

    return d->registerTargetDetectedHandler(filter, object, metaMethod);
}

/*!
    Registers \a object to receive notifications on \a method when a tag has been detected and has
    an NDEF message.  The \a method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.

    \note The \i target parameter of \a method may not be available on all platforms, in which case
    \i target will be 0.
*/
int QNearFieldManager::registerTargetDetectedHandler(QObject *object, const char *method)
{
    QMetaMethod metaMethod = methodForSignature(object, method);
    if (!metaMethod.enclosingMetaObject())
        return -1;

    Q_D(QNearFieldManager);

    return d->registerTargetDetectedHandler(object, metaMethod);
}

/*!
    Registers \a object to receive notifications on \a method when a tag has been detected and has
    an NDEF message that matches \a filter is detected. The \a method on \a object should have the
    prototype 'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.

    \note The \i target parameter of \a method may not be available on all platforms, in which case
    \i target will be 0.
*/
int QNearFieldManager::registerTargetDetectedHandler(const QNdefFilter &filter,
                                                     QObject *object, const char *method)
{
    QMetaMethod metaMethod = methodForSignature(object, method);
    if (!metaMethod.enclosingMetaObject())
        return -1;

    Q_D(QNearFieldManager);

    return d->registerTargetDetectedHandler(filter, object, metaMethod);
}

/*!
    Unregisters the target detect handler identified by \a handlerId.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldManager::unregisterTargetDetectedHandler(int handlerId)
{
    Q_D(QNearFieldManager);

    return d->unregisterTargetDetectedHandler(handlerId);
}

void QNearFieldManager::setTargetAccessModes(TargetAccessModes accessModes)
{
    Q_D(QNearFieldManager);

    TargetAccessModes removedModes = ~accessModes & d->m_requestedModes;
    if (removedModes)
        d->releaseAccess(removedModes);

    TargetAccessModes newModes = accessModes & ~d->m_requestedModes;
    if (newModes)
        d->requestAccess(newModes);
}

QNearFieldManager::TargetAccessModes QNearFieldManager::targetAccessModes() const
{
    Q_D(const QNearFieldManager);

    return d->m_requestedModes;
}

#include "moc_qnearfieldmanager.cpp"
#include "moc_qnearfieldmanager_p.cpp"

QTM_END_NAMESPACE
