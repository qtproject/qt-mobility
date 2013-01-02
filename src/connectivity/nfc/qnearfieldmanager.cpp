/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
#include "qnearfieldmanager_maemo6_p.h"
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
    \since 1.2

    NFC Forum devices support two modes of communications.  The first mode, peer-to-peer
    communications, is used to communicate between two NFC Forum devices.  The second mode,
    master/slave communications, is used to communicate between an NFC Forum device and an NFC
    Forum Tag or Contactless Card.  The targetDetected() signal is emitted when a target device
    enters communications range.  Communications can be initiated from the slot connected to this
    signal.

    NFC Forum devices generally operate as the master in master/slave communications. Some devices
    are also capable of operating as the slave, so called Card Emulation mode. In this mode the
    local NFC device emulates a NFC Forum Tag or Contactless Card and can be used to perform
    transactions. The transaction happens entirely within a secure element on the device and only a
    notification of the transaction is provided. The transactionDetected() signal is emitted
    whenever a transaction occurs.

    NFC Forum Tags can contain one or more messages in a standardized format. These messages are
    encapsulated by the QNdefMessage class. Use the registerNdefMessageHandler() functions to
    register message handlers with particular criteria. Handlers can be unregistered with the
    unregisterNdefMessageHandler() function.

    Applications can connect to the targetDetected() and targetLost() signals to get notified when
    an NFC Forum Device or NFC Forum Tag enters or leaves proximity. Before these signals are
    emitted target detection must be started with the startTargetDetection() function, which takes
    a parameter to limit the type of device or tags detected. Target detection can be stopped with
    the stopTargetDetection() function. Before a detected target can be accessed it is necessary to
    request access rights. This must be done before the target device is touched. The
    setTargetAccessModes() function is used to set the types of access the application wants to
    perform on the detected target. When access is no longer required the target access modes
    should be set to NoTargetAccess as other applications may be blocked from accessing targets.
    The current target access modes can be retried with the targetAccessModes() function.


    \section2 Automatically launching NDEF message handlers

    It is possible to pre-register an application to receive NDEF messages matching a given
    criteria. This is useful to get the system to automatically launch your application when a
    matching NDEF message is received. This removes the need to have the user manually launch NDEF
    handling applications, prior to touching a tag, or to have those applications always running
    and using system resources.

    The process of registering the handler is different on each platform. The platform specifics
    are documented in the sections below. Qt Mobility provides a tool, \c {ndefhandlergen}, to
    generate the platform specific registration files. The output of \c {ndefhandlergen -help} is
    reproduced here for convenience:

    \code
        Generate platform specific NFC message handler registration files.
        Usage: ndefhandlergen [options]

            -template TEMPLATE    Template to use.
            -appname APPNAME      Name of the application.
            -apppath APPPATH      Path to installed application binary.
            -datatype DATATYPE    URN of the NDEF message type to match.
            -match MATCHSTRING    Platform specific match string.

        The -datatype and -match options are mutually exclusive.

        Available templates: harmattan, symbian
    \endcode

    A typical invocation of the \c ndefhandlergen tool for Symbian^3 target:

    \code
        ndefhandlergen -template symbian -appname myapplication -datatype urn:nfc:ext:com.example:f
    \endcode

    and for the Meego 1.2 Harmattan target:

    \code
        ndefhandlergen -template harmattan -appname myapplication -apppath /usr/bin/myapplication -datatype urn:nfc:ext:com.example:f
    \endcode


    Once the application has been registered as an NDEF message handler, the application only needs
    to call the registerNdefMessageHandler() function:

    \code
        QNearFieldManager *manager = new QNearFieldManager;
        manager->registerNdefMessageHandler(this,
                                            SLOT(handleNdefMessage(QNdefMessage,QNearFieldTarget)));
    \endcode

    \section3 Symbian^3

    On Symbian^3 an xml file needs to be created and installed into a particular directory on the
    device. The format of the xml is given below.

    \quotefile tools/ndefhandlergen/templates/symbian/symbian.xml

    The \i {%APPNAME%} tags need to be changed to match the name of the application. The
    \i description xml tags should be used to describe the application, however these values are
    not used. The \i {%DATATYPE%} tag must be set with the NDEF record type to match  For example
    the following would be used to match NDEF messages that contain a RTD-URI record:

    \code
        <customproperty key="datatype">urn:nfc:wkt:U</customproperty>
    \endcode

    The following would be used to match NDEF messages that contain a custom type
    urn:nfc:ext:example.com:f:

    \code
        <customproperty key="datatype">urn:nfc:ext:com.example:f</customproperty>
    \endcode

    The value of the \i customproperty xml tag can be set to any valid match string supported by
    the Symbian^3 platform.

    It is recommended to name the xml file after the application package name. For example
    myapplication.xml. To install the above xml file into the correct location the following should
    be added to the application .pro file:

    \code
        symbian {
            ndefhandler.sources = %APPNAME%.xml
            ndefhandler.path = c:/private/2002AC7F/import/
            DEPLOYMENT += ndefhandler
        }
    \endcode

    The \i {%APPNAME%} tag need to be changed to match the name of the application.

    \section3 Meego 1.2 Harmattan

    On Meego 1.2 Harmattan the NDEF message handler notifications are passed over D-Bus.
    Registration of the NDEF message match criteria is done via a D-Bus call. The
    application must also ensure that it has registered a D-Bus service name so that the
    application can be automatically launched when a notification message is sent to the
    registered service.  It is also a requirement that the handler is a singe instance application.

    To register the D-Bus service the two files need to be created and installed into particular
    directories on the device. The first file is the D-Bus bus configuration file:

    \quotefile tools/ndefhandlergen/templates/harmattan/harmattan.conf

    The \i {%APPNAME%} tag must be replaced with the name of your application binary.

    The second file is a D-Bus service file which is used by the D-Bus daemon to launch your
    application.

    \quotefile tools/ndefhandlergen/templates/harmattan/harmattan.service

    The \i {%APPNAME%} tag must be replace with the name of your application binary and the
    \i {%APPPATH%} tag must be replaced with the path to your installed application binary.  The
    use of \c {/usr/bin/single-instance} ensures that only a single instance of your application is
    started by the service file.

    It is recommended to name these files after the application package name. For example
    myapplication.conf and myapplication.service. To install the above files into the correct
    location the following should be added to the application .pro file:

    \code
    contains(MEEGO_EDITION,harmattan) {
        ndefhandler_conf.files = %APPNAME%.conf
        ndefhandler_conf.path = /etc/dbus-1/session.d/

        ndefhandler_service.files = com.nokia.qtmobility.nfc.%APPNAME%.service
        ndefhandler_service.path = /usr/share/dbus-1/services/

        INSTALLS += ndefhandler_conf ndefhandler_service
    }
    \endcode

    The NFC auto-start handler on Meego 1.2 Harmattan requires that the applications .desktop file
    has the same name as the application.  This may not be the case as the .desktop files created
    by Qt Creator is named \i {%APPNAME%_harmattan.desktop} by default.  The application developer
    should ensure that the \i {%APPNAME%.desktop} file is installed to the appropriate place.  A
    .pro file snippet similar to the following may be used:

    \code
    contains(MEEGO_EDITION,harmattan) {
        harmattandesktopfile.files = %APPNAME%.desktop
        harmattandesktopfile.path = /usr/share/applications
        INSTALLS += harmattandesktopfile
    }
    \endcode

    The NDEF message handler is registered with the following D-Bus command. Applications should
    ensure that the following command (or similar) is executed once at installation time. For
    example in the packages post-installation script.

    \quotefile tools/ndefhandlergen/templates/harmattan/harmattan.postinst

    The \i {%APPNAME%} string must be replaced with the name of the application binary. The
    \i {%DATATYPE%} string must be replaced with the NDEF record type to match. For example the
    following would be used to match NDEF messages that contain a RTD-URI record:

    \code
        string:"'urn:nfc:wkt:U'[1:*];"
    \endcode

    The following would be used to match NDEF messages that contain a custom type
    urn:nfc:ext:example.com:f:

    \code
        string:"'urn:nfc:ext:example.com:f'[1:*];"
    \endcode

    Note that \c {[1:*]} indicates one or more records of the specified type must be in the NDEF
    message. The value of the datatype string argument can be set to any valid match string
    supported by the MeeGo 1.2 Harmattan platform.

    The NDEF message handler should be unregistered at uninstallation time. For example in the
    packages pre-removal script.

    \quotefile tools/ndefhandlergen/templates/harmattan/harmattan.prerm

    The \i {%APPNAME%} string must be replace with the name of the application binary.
*/

/*!
    \enum QNearFieldManager::TargetAccessMode

    This enum describes the different access modes an application can have.

    \value NoTargetAccess           The application cannot access NFC capabilities.
    \value NdefReadTargetAccess     The application can read NDEF messages from targets by calling
                                    QNearFieldTarget::readNdefMessages().
    \value NdefWriteTargetAccess    The application can write NDEF messages to targets by calling
                                    QNearFieldTarget::writeNdefMessages().
    \value TagTypeSpecificTargetAccess  The application can access targets using raw commands by
                                        calling QNearFieldTarget::sendCommand().
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
    Returns true if NFC functionality is available; otherwise returns false.
*/
bool QNearFieldManager::isAvailable() const
{
    Q_D(const QNearFieldManager);

    return d->isAvailable();
}

/*!
    Starts detecting targets of type \a targetTypes. Returns \c true if target detection is
    successfully started; otherwise returns \c false. The application can start to get 
    notifications when the target detection is successfully started and NFC is switched on.
    
    The targetDetected() signal is emitted when a target with the type in \a targetTypes is
    within proximity. If \a targetTypes is empty targets of all types will be detected.
    
    \note In a MeeGo 1.2 Harmattan device the method returns \c true regardless of the device's NFC state
    (on or off). In Symbian devices the method returns \c false if NFC is switched off; otherwise 
    returns \c true.

    \sa stopTargetDetection()
*/
bool QNearFieldManager::startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes)
{
    Q_D(QNearFieldManager);

    if (targetTypes.isEmpty())
        return d->startTargetDetection(QList<QNearFieldTarget::Type>() << QNearFieldTarget::AnyTarget);
    else
        return d->startTargetDetection(targetTypes);
}

/*!
    \overload

    Starts detecting targets of type \a targetType. Returns true if target detection is
    successfully started; otherwise returns false. Causes the targetDetected() signal to be emitted
    when a target with the type \a targetType is within proximity.
*/
bool QNearFieldManager::startTargetDetection(QNearFieldTarget::Type targetType)
{
    return startTargetDetection(QList<QNearFieldTarget::Type>() << targetType);
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
    an NDEF record that matches \a typeNameFormat and \a type.  The \a method on \a object should
    have the prototype
    'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.

    \note The \i target parameter of \a method may not be available on all platforms, in which case
    \i target will be 0.
*/

int QNearFieldManager::registerNdefMessageHandler(QNdefRecord::TypeNameFormat typeNameFormat,
                                                  const QByteArray &type,
                                                  QObject *object, const char *method)
{
    QMetaMethod metaMethod = methodForSignature(object, method);
    if (!metaMethod.enclosingMetaObject())
        return -1;

    QNdefFilter filter;
    filter.appendRecord(typeNameFormat, type);

    Q_D(QNearFieldManager);

    return d->registerNdefMessageHandler(filter, object, metaMethod);
}

/*!
    \fn int QNearFieldManager::registerNdefMessageHandler(QObject *object, const char *method)

    Registers \a object to receive notifications on \a method when a tag has been detected and has
    an NDEF message that matches a pre-registered message format. The \a method on \a object should
    have the prototype
    'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.

    This function is used to register a QNearFieldManager instance to receive notifications when a
    NDEF message matching a pre-registered message format is received. See the section on
    \l {Automatically launching NDEF message handlers}.

    \note The \i target parameter of \a method may not be available on all platforms, in which case
    \i target will be 0.
*/
int QNearFieldManager::registerNdefMessageHandler(QObject *object, const char *method)
{
    QMetaMethod metaMethod = methodForSignature(object, method);
    if (!metaMethod.enclosingMetaObject())
        return -1;

    Q_D(QNearFieldManager);

    return d->registerNdefMessageHandler(object, metaMethod);
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
int QNearFieldManager::registerNdefMessageHandler(const QNdefFilter &filter,
                                                  QObject *object, const char *method)
{
    QMetaMethod metaMethod = methodForSignature(object, method);
    if (!metaMethod.enclosingMetaObject())
        return -1;

    Q_D(QNearFieldManager);

    return d->registerNdefMessageHandler(filter, object, metaMethod);
}

/*!
    Unregisters the target detect handler identified by \a handlerId.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldManager::unregisterNdefMessageHandler(int handlerId)
{
    Q_D(QNearFieldManager);

    return d->unregisterNdefMessageHandler(handlerId);
}

/*!
    Sets the requested target access modes to \a accessModes.
*/
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

/*!
    Returns current requested target access modes.
*/
QNearFieldManager::TargetAccessModes QNearFieldManager::targetAccessModes() const
{
    Q_D(const QNearFieldManager);

    return d->m_requestedModes;
}

#include "moc_qnearfieldmanager.cpp"
#include "moc_qnearfieldmanager_p.cpp"

QTM_END_NAMESPACE
