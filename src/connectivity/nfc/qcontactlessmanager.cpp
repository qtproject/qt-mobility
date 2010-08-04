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

#include "qcontactlessmanager.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QContactlessManager
    \brief The QContactlessManager class provides access to notifications for NFC events.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

/*!
    \fn void QContactlessManager::targetDetected(const QContactlessTarget &target)

    This signal is emitted whenever a target is detected. The \a target parameter represents the
    detected target.

    This signal will be emitted for all detected targets. To receive signals only for desired
    targets use the connect() member function.
*/

/*!
    \fn void QContactlessManager::transactionDetected(const QByteArray &applicationIdentifier)

    This signal is emitted when ever a transaction is performed with the application identified by
    \a applicationIdentifier.

    The \a applicationIdentifier is a byte array of up to 16 bytes as defined by ISO 7816-4 and
    uniquely identifies the application and application vendor that was involved in the
    transaction.
*/

/*!
    Constructs a new contactless manager with \a parent.
*/
QContactlessManager::QContactlessManager(QObject *parent)
:   QObject(parent)
{
}

/*!
    Destroys the contactless manager.
*/
QContactlessManager::~QContactlessManager()
{
}

/*!
    Registers for notifications when a target of \a type is detected. The \a slot method on
    \a object will be called and should have the prototype
    'void targetDetected(const QContactlessTarget &target)'.

    Returns true of success; other returns false.
*/
bool QContactlessManager::connect(QContactlessTarget::TagType type,
                                  const QObject *object,
                                  const char *slot)
{
    Q_UNUSED(type);
    Q_UNUSED(object);
    Q_UNUSED(slot);

    return false;
}

/*!
    Unregisters notifications for targets of \a type to \a slot on \a object. Returns true on
    success; otherwise returns false.
*/
bool QContactlessManager::disconnect(QContactlessTarget::TagType type,
                                     const QObject *object,
                                     const char *slot)
{
    Q_UNUSED(type);
    Q_UNUSED(object);
    Q_UNUSED(slot);

    return false;
}

/*!
    Registers for notifications when an NDEF record that matches \a typeNameFormat is detected. The
    \a slot method on \a object will be called and should have the prototype
    'void ndefRecordDetected(const QNdefRecord &record)'.

    Returns true on success; otherwise return false.
*/
bool QContactlessManager::connect(QNdefRecord::TypeNameFormat typeNameFormat,
                                  const QObject *object, const char *slot)
{
    Q_UNUSED(typeNameFormat);
    Q_UNUSED(object);
    Q_UNUSED(slot);

    return false;
}

/*!
    Unregisters notifications for when an NDEF record that match \a typeNameFormat to \a slot on
    \a object. Returns true o success; otherwise returns false.
*/
bool QContactlessManager::disconnect(QNdefRecord::TypeNameFormat typeNameFormat,
                                     const QObject *object, const char *slot)
{
    Q_UNUSED(typeNameFormat);
    Q_UNUSED(object);
    Q_UNUSED(slot);

    return false;
}

#include "moc_qcontactlessmanager.cpp"

QTM_END_NAMESPACE
