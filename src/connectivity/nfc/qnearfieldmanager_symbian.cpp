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

#include <qservicefilter.h>
#include <qservicemanager.h>
#include <QCoreApplication>
#include <ndefmessage.h>

#include "qnearfieldmanager_symbian_p.h"
#include "qndefmessage.h"
#include "symbian/nearfieldmanager_symbian.h"
#include "symbian/debug.h"
#include "symbian/nearfieldutility_symbian.h"

QTM_BEGIN_NAMESPACE

Proxy contentHandlerProxy;

Proxy::Proxy(QObject* parent) : QObject (parent)
{
}

ContentHandlerInterface::ContentHandlerInterface(QObject* parent)
       : QObject(parent)
{
}
void ContentHandlerInterface::handleMessage(const QByteArray& btArray)
{
     // incoming message from ECOM content handler loader eventually cause object & method registered via below
     // registerTargetDetectHandler to be invoked with message as parameter.  i.e. MyContentHandler::handleMessage(message)
     // from above.


     QNdefMessage msg = QNdefMessage::fromByteArray(btArray);
     QMetaObject::invokeMethod(&contentHandlerProxy, "handleMessage", Q_ARG(QNdefMessage, msg));


}
/*!
    \class QNearFieldManagerPrivateImpl
    \brief The QNearFieldManagerPrivateImpl class provides symbian backend access to NFC service.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

  A Qt-Symbian wrapper implementation class to support symbian NFC backend.
*/

/*!
    Constructs a new near field manager private implementation.
*/
QNearFieldManagerPrivateImpl::QNearFieldManagerPrivateImpl(): m_symbianbackend(NULL),m_target(NULL),m_serviceRegister(NULL)
{
    BEGIN
    QT_TRAP_THROWING(m_symbianbackend = CNearFieldManager::NewL(*this));
    END
}

/*!
    Destroys the near field manager private implementation.
*/
QNearFieldManagerPrivateImpl::~QNearFieldManagerPrivateImpl()
{
    BEGIN
    delete m_target;
    delete m_serviceRegister;
    delete m_symbianbackend;
    END
}

/*!
    Helper function to get the free handler id.
*/
int QNearFieldManagerPrivateImpl::getFreeId()
{
    BEGIN
    if (!m_freeIds.isEmpty())
        return m_freeIds.takeFirst();

    m_registeredHandlers.append(Callback());
    END
    return m_registeredHandlers.count() - 1;
}

/*!
    Registers \a object to receive notifications on \a method when a tag with a tag type of
    \a targetType has been detected and has an NDEF message is detected. The
    \a method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.
*/
int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(QObject *object,
                                                                const QMetaMethod &method)
{
    QServiceFilter filter("com.nokia.symbian.NdefMessageHandler");
    QCoreApplication* app = QCoreApplication::instance();
    //The appfile path will return something like \sys\bin\nfcspserviceprovider.exe
    //but we only need nfcspserviceprovider as service name
    QString appfilepath = app->applicationFilePath();
    TInt lastseprator = appfilepath.lastIndexOf("\\");
    TInt lastdot = appfilepath.lastIndexOf(".");
    QString servicename = appfilepath.mid(lastseprator+1, lastdot-lastseprator-1);
    qDebug() << "application name: " << servicename << endl;

    filter.setServiceName(servicename);
    QServiceManager sfManager;

    if (!sfManager.findInterfaces(filter).isEmpty())
        {
        // This application has been registered as a content handler (via the xml at install time), start the service
        m_chobject = object;
        m_chmethod = method;

        connect(&contentHandlerProxy, SIGNAL(handleMessage(QNdefMessage)),
                    this, SLOT(_q_privateHandleMessageSlot(QNdefMessage)));
        m_serviceRegister = new QRemoteServiceRegister();
        QRemoteServiceRegister::Entry entry =
                m_serviceRegister->createEntry<ContentHandlerInterface>(servicename,
                                                                     "com.nokia.symbian.NdefMessageHandler",
                                                                     "1.0");
        entry.setInstantiationType(QRemoteServiceRegister::PrivateInstance);
        m_serviceRegister->publishEntries(servicename);
        //m_serviceRegister->setQuitOnLastInstanceClosed(true);
        //delete serviceRegister;

        return 0xffff;

    } else {
        // not supported if not registered as a content handler using this API
        return -1;
    }
}

/*!
    Registers \a object to receive notifications on \a method when a tag with a tag type of
    \a targetType has been detected and has an NDEF message that matches \a filter is detected. The
    \a method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.
*/
int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(const QNdefFilter &filter,
                                                                QObject *object,
                                                                const QMetaMethod &method)
{
    BEGIN
    int id = getFreeId();

    Callback &callback = m_registeredHandlers[id];

    callback.filter = filter;
    callback.object = object;
    callback.method = method;

    for( int i = 0; i < filter.recordCount(); ++i)
      {
      QNdefRecord::TypeNameFormat tnf = filter.recordAt(i).typeNameFormat;
      QByteArray type = filter.recordAt(i).type;
      if (m_symbianbackend->AddNdefSubscription( tnf, type ) < 0)
          {
          for (int j = 0; j < i; ++j)//rollback the previous subscriptions
              {
              QNdefRecord::TypeNameFormat n = filter.recordAt(j).typeNameFormat;
              QByteArray t = filter.recordAt(j).type;
              m_symbianbackend->RemoveNdefSubscription( n, t );
              }
          m_freeIds.append(id);//push back the id to freeid list
          END
          return -1;
          }
      }
    END
    return id;
}

/*!
    Unregisters the target detect handler identified by \a id.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldManagerPrivateImpl::unregisterTargetDetectedHandler(int id)
{
    BEGIN
     if ( 0xffff == id )
        {
        disconnect(&contentHandlerProxy, SIGNAL(handleMessage(QNdefMessage)),
                          this, SLOT(_q_privateHandleMessageSlot(QNdefMessage)));
        return true;
        }
    if (id < 0 || id >= m_registeredHandlers.count() || m_freeIds.contains(id))
        return false;

    m_freeIds.append(id);

    for ( int i = 0; i < m_registeredHandlers[id].filter.recordCount(); ++i)
      {
        QNdefRecord::TypeNameFormat tnf = m_registeredHandlers[id].filter.recordAt(i).typeNameFormat;
        QByteArray type = m_registeredHandlers[id].filter.recordAt(i).type;
        m_symbianbackend->RemoveNdefSubscription( tnf, type );
      }

    while (m_freeIds.contains(m_registeredHandlers.count() - 1)) {
        m_freeIds.removeAll(m_registeredHandlers.count() - 1);
        m_registeredHandlers.removeLast();
    }
    END
    return true;
}

void QNearFieldManagerPrivateImpl::startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes)
    {
    BEGIN
    QT_TRAP_THROWING(m_symbianbackend->StartTargetDetectionL(targetTypes));
    END
    }

void QNearFieldManagerPrivateImpl::stopTargetDetection()
    {
    BEGIN
    m_symbianbackend->stopTargetDetection();
    END
    }

struct VerifyRecord
{
    QNdefFilter::Record filterRecord;
    unsigned int count;
};

/*!
    Callback function when symbian NFC backend found the NFC \a target.
*/
void QNearFieldManagerPrivateImpl::targetFound(QNearFieldTarget *target)
{
    BEGIN
    if (!target){
        return;
    }
    if (m_target){
        delete m_target;
        m_target = NULL;
    }
    m_target = target;
    emit targetDetected(target);
    END
}

/*
 * Private slot to be invoked by QT content handler loader
 */
void QNearFieldManagerPrivateImpl::_q_privateHandleMessageSlot(QNdefMessage aMsg)
    {

    m_chmethod.invoke(m_chobject, Q_ARG(QNdefMessage, aMsg), Q_ARG(QNearFieldTarget* , NULL));
    }

/*!
    Helper function to invoke the filtered TargetDetectedHandler for a found \a target.
*/

void QNearFieldManagerPrivateImpl::invokeTargetDetectedHandler(const QNdefMessage msg)
    {
    BEGIN
    for (int i = 0; i < m_registeredHandlers.count(); ++i) {
        if (m_freeIds.contains(i))
            continue;
        Callback &callback = m_registeredHandlers[i];
        bool matched = true;
        QList<VerifyRecord> filterRecords;
        for (int j = 0; j < callback.filter.recordCount(); ++j) {
            VerifyRecord vr;
            vr.count = 0;
            vr.filterRecord = callback.filter.recordAt(j);
            filterRecords.append(vr);
        }

        foreach (const QNdefRecord &record, msg) {
            for (int j = 0; matched && (j < filterRecords.count()); ++j) {
                VerifyRecord &vr = filterRecords[j];

                if (vr.filterRecord.typeNameFormat == record.typeNameFormat() &&
                        vr.filterRecord.type == record.type()) {
                    ++vr.count;
                    break;
                } else {
                    if (callback.filter.orderMatch()) {
                        if (vr.filterRecord.minimum <= vr.count &&
                                vr.count <= vr.filterRecord.maximum) {
                            continue;
                        } else {
                            matched = false;
                        }
                    }
                }
            }
        }//end of foreach (const QNdefRecord &record, msg) {

        for (int j = 0; matched && (j < filterRecords.count()); ++j) {
            const VerifyRecord &vr = filterRecords.at(j);

            if (vr.filterRecord.minimum <= vr.count && vr.count <= vr.filterRecord.maximum)
                continue;
            else
                matched = false;
        }

        if (matched) {
            callback.method.invoke(callback.object, Q_ARG(QNdefMessage, msg),
                    Q_ARG(QNearFieldTarget *, NULL));
        }

    }
    END
    }

/*!
    Callback function when symbian NFC backend lost the NFC \a target.
*/
void QNearFieldManagerPrivateImpl::targetDisconnected()
    {
    BEGIN
    if (m_target)
        {
        QMetaObject::invokeMethod(m_target, "disconnected");
        emit targetLost(m_target);
        }
    END
    }

#include "moc_qnearfieldmanager_symbian_p.cpp"

QTM_END_NAMESPACE
