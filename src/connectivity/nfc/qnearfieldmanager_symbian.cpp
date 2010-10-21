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

#include "qnearfieldmanager_symbian_p.h"
#include "qndefmessage.h"

#include <QtCore/QDirIterator>
#include <QtCore/QSettings>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

#include <QtCore/QDebug>

#include "symbian/nearfieldmanager_symbian.h"

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
QNearFieldManagerPrivateImpl::QNearFieldManagerPrivateImpl()
{
	QT_TRAP_THROWING(m_symbianbackend = CNearFieldManager::NewL(*this));
}

/*!
    Destroys the near field manager private implementation.
*/
QNearFieldManagerPrivateImpl::~QNearFieldManagerPrivateImpl()
{
	delete m_symbianbackend;
	
}

/*!
    Helper function to get the free handler id.
*/
int QNearFieldManagerPrivateImpl::getFreeId()
{
    if (!m_freeIds.isEmpty())
        return m_freeIds.takeFirst();

    m_registeredHandlers.append(Callback());
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
int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                                                QObject *object,
                                                                const QMetaMethod &method)
{
    int id = getFreeId();

    Callback &callback = m_registeredHandlers[id];

    callback.targetType = targetType;
    callback.filter = QNdefFilter();
    callback.object = object;
    callback.method = method;

    return id;
}

/*!
    Registers \a object to receive notifications on \a method when a tag with a tag type of
    \a targetType has been detected and has an NDEF message that matches \a filter is detected. The
    \a method on \a object should have the prototype
    'void targetDetected(const QNdefMessage &message, QNearFieldTarget *target)'.

    Returns an identifier, which can be used to unregister the handler, on success; otherwise
    returns -1.
*/
int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(QNearFieldTarget::Type targetType,
                                                                const QNdefFilter &filter,
                                                                QObject *object,
                                                                const QMetaMethod &method)
{
    int id = getFreeId();

    Callback &callback = m_registeredHandlers[id];

    callback.targetType = targetType;
    callback.filter = filter;
    callback.object = object;
    callback.method = method;

    return id;
}

/*!
    Unregisters the target detect handler identified by \a id.

    Returns true on success; otherwise returns false.
*/
bool QNearFieldManagerPrivateImpl::unregisterTargetDetectedHandler(int id)
{
    if (id < 0 || id >= m_registeredHandlers.count())
        return false;

    m_freeIds.append(id);

    while (m_freeIds.contains(m_registeredHandlers.count() - 1)) {
        m_freeIds.removeAll(m_registeredHandlers.count() - 1);
        m_registeredHandlers.removeLast();
    }

    return true;
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
	m_target = target;
    emit targetDetected(target);

    if (target->hasNdefMessage()) {
        for (int i = 0; i < m_registeredHandlers.count(); ++i) {
            if (m_freeIds.contains(i))
                continue;

            Callback &callback = m_registeredHandlers[i];

            if (callback.targetType != QNearFieldTarget::AnyTarget &&
                target->type() != callback.targetType) {
                continue;
            }

            QList<QNdefMessage> messages = target->ndefMessages();
            foreach (const QNdefMessage &message, messages) {
                bool matched = true;

                QList<VerifyRecord> filterRecords;
                for (int j = 0; j < callback.filter.recordCount(); ++j) {
                    VerifyRecord vr;
                    vr.count = 0;
                    vr.filterRecord = callback.filter.recordAt(j);

                    filterRecords.append(vr);
                }

                foreach (const QNdefRecord &record, message) {
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
                }

                for (int j = 0; matched && (j < filterRecords.count()); ++j) {
                    const VerifyRecord &vr = filterRecords.at(j);

                    if (vr.filterRecord.minimum <= vr.count && vr.count <= vr.filterRecord.maximum)
                        continue;
                    else
                        matched = false;
                }

                if (matched) {
                    callback.method.invoke(callback.object, Q_ARG(QNdefMessage, message),
                                                            Q_ARG(QNearFieldTarget *, target));
                }
            }
        }
    }
}

/*!
    Callback function when symbian NFC backend lost the NFC \a target.
*/
void QNearFieldManagerPrivateImpl::targetDisconnected()
	{
	if (m_target)
		{
		emit targetLost(m_target);
		QMetaObject::invokeMethod(m_target, "disconnected");
		}
	}

//#include "qnearfieldmanager_symbian.moc"
