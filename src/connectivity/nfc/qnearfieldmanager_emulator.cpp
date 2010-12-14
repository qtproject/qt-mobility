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

#include "qnearfieldmanager_emulator_p.h"
#include "targetemulator_p.h"
#include "qnearfieldtarget_p.h"
#include "qnearfieldtagtype1.h"
#include "qndefmessage.h"
#include "qtlv_p.h"

#include <QtCore/QDirIterator>
#include <QtCore/QSettings>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>

#include <QtCore/QDebug>

static QMutex tagMutex;
static QMap<TagBase *, bool> tagMap;

class TagActivator : public QObject
{
    Q_OBJECT

public:
    TagActivator();
    ~TagActivator();

    void initialize();
    void reset();

protected:
    void timerEvent(QTimerEvent *e);

signals:
    void tagActivated(TagBase *tag);
    void tagDeactivated(TagBase *tag);

private:
    QMap<TagBase *, bool>::Iterator m_current;
    int timerId;
} tagActivator;

TagActivator::TagActivator()
:   timerId(-1)
{

}

TagActivator::~TagActivator()
{
    QMutexLocker locker(&tagMutex);
    qDeleteAll(tagMap.keys());
    tagMap.clear();
}

void TagActivator::initialize()
{
    QMutexLocker locker(&tagMutex);

    if (!tagMap.isEmpty())
        return;

    QDirIterator nfcTargets(QDir::currentPath(), QStringList(QLatin1String("*.nfc")), QDir::Files);
    while (nfcTargets.hasNext()) {
        const QString targetFilename = nfcTargets.next();

        QSettings target(targetFilename, QSettings::IniFormat);

        target.beginGroup(QLatin1String("Target"));

        const QString tagType = target.value(QLatin1String("Type")).toString();

        target.endGroup();

        if (tagType == QLatin1String("TagType1")) {
            NfcTagType1 *tag = new NfcTagType1;
            tag->load(&target);

            tagMap.insert(tag, false);
        } else {
            qWarning("Unknown tag type %s\n", qPrintable(tagType));
        }
    }

    m_current = tagMap.end();

    timerId = startTimer(1000);
}

void TagActivator::reset()
{
    QMutexLocker locker(&tagMutex);

    killTimer(timerId);
    timerId = -1;

    qDeleteAll(tagMap.keys());
    tagMap.clear();
}

void TagActivator::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);

    tagMutex.lock();

    if (m_current != tagMap.end()) {
        *m_current = false;

        tagMutex.unlock();
        emit tagDeactivated(m_current.key());
        tagMutex.lock();
    }

    ++m_current;
    if (m_current == tagMap.end())
        m_current = tagMap.begin();

    if (m_current != tagMap.end()) {
        *m_current = true;

        tagMutex.unlock();

        emit tagActivated(m_current.key());
        tagMutex.lock();
    }

    tagMutex.unlock();
}

class TagType1 : public QNearFieldTagType1
{
    Q_OBJECT

public:
    TagType1(TagBase *tag, QObject *parent);
    ~TagType1();

    QByteArray uid() const;

    AccessMethods accessMethods() const;

    RequestId sendCommand(const QByteArray &command);
    bool waitForRequestCompleted(const RequestId &id, int msecs = 5000);

private:
    TagBase *m_tag;
};

TagType1::TagType1(TagBase *tag, QObject *parent)
:   QNearFieldTagType1(parent), m_tag(tag)
{
}

TagType1::~TagType1()
{
}

QByteArray TagType1::uid() const
{
    QMutexLocker locker(&tagMutex);

    return m_tag->uid();
}

QNearFieldTarget::AccessMethods TagType1::accessMethods() const
{
    return NdefAccess | TagTypeSpecificAccess;
}

QNearFieldTarget::RequestId TagType1::sendCommand(const QByteArray &command)
{
    QMutexLocker locker(&tagMutex);

    // tag not in proximity
    if (!tagMap.value(m_tag)) {
        emit error(TargetOutOfRangeError);
        return RequestId();
    }

    quint16 crc = qNfcChecksum(command.constData(), command.length());

    RequestIdPrivate *idPrivate = new RequestIdPrivate;
    RequestId id(idPrivate);

    QByteArray response = m_tag->processCommand(command + char(crc & 0xff) + char(crc >> 8));

    if (response.isEmpty()) {
        emit error(NoResponseError);
        return id;
    }

    // check crc
    if (qNfcChecksum(response.constData(), response.length()) != 0) {
        emit error(ChecksumMismatchError);
        return id;
    }

    response.chop(2);

    QMetaObject::invokeMethod(this, "handleResponse", Qt::QueuedConnection,
                              Q_ARG(QNearFieldTarget::RequestId, id), Q_ARG(QByteArray, response));

    return id;
}

bool TagType1::waitForRequestCompleted(const RequestId &id, int msecs)
{
    QCoreApplication::sendPostedEvents(this, QEvent::MetaCall);

    return QNearFieldTagType1::waitForRequestCompleted(id, msecs);
}

QNearFieldManagerPrivateImpl::QNearFieldManagerPrivateImpl()
{
    tagActivator.initialize();

    connect(&tagActivator, SIGNAL(tagActivated(TagBase*)), this, SLOT(tagActivated(TagBase*)));
    connect(&tagActivator, SIGNAL(tagDeactivated(TagBase*)), this, SLOT(tagDeactivated(TagBase*)));
}

QNearFieldManagerPrivateImpl::~QNearFieldManagerPrivateImpl()
{
}

void QNearFieldManagerPrivateImpl::reset()
{
    tagActivator.reset();
}

void QNearFieldManagerPrivateImpl::startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes)
{
    m_detectTargetTypes = targetTypes;
}

void QNearFieldManagerPrivateImpl::stopTargetDetection()
{
    m_detectTargetTypes.clear();
}

int QNearFieldManagerPrivateImpl::getFreeId()
{
    if (!m_freeIds.isEmpty())
        return m_freeIds.takeFirst();

    m_registeredHandlers.append(Callback());
    return m_registeredHandlers.count() - 1;
}

int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(QObject *object,
                                                                const QMetaMethod &method)
{
    int id = getFreeId();

    Callback &callback = m_registeredHandlers[id];

    callback.filter = QNdefFilter();
    callback.object = object;
    callback.method = method;

    return id;
}

int QNearFieldManagerPrivateImpl::registerTargetDetectedHandler(const QNdefFilter &filter,
                                                                QObject *object,
                                                                const QMetaMethod &method)
{
    int id = getFreeId();

    Callback &callback = m_registeredHandlers[id];

    callback.filter = filter;
    callback.object = object;
    callback.method = method;

    return id;
}

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

void QNearFieldManagerPrivateImpl::tagActivated(TagBase *tag)
{
    QNearFieldTarget *target = m_targets.value(tag);
    if (!target) {
        target = new TagType1(tag, this);
        m_targets.insert(tag, target);
    }

    if (m_detectTargetTypes.contains(QNearFieldTarget::NfcTagType1) ||
        m_detectTargetTypes.contains(QNearFieldTarget::AnyTarget)) {
        emit targetDetected(target);
    }

    if (target->hasNdefMessage()) {
        QTlvReader reader(target);
        while (!reader.atEnd()) {
            if (!reader.readNext()) {
                if (!target->waitForRequestCompleted(reader.requestId()))
                    break;
                else
                    continue;
            }

            // NDEF Message TLV
            if (reader.tag() == 0x03)
                ndefReceived(QNdefMessage::fromByteArray(reader.data()), target);
        }
    }
}

void QNearFieldManagerPrivateImpl::tagDeactivated(TagBase *tag)
{
    QNearFieldTarget *target = m_targets.value(tag);
    if (!target)
        return;

    emit targetLost(target);
    QMetaObject::invokeMethod(target, "disconnected");
}

void QNearFieldManagerPrivateImpl::ndefReceived(const QNdefMessage &message,
                                                QNearFieldTarget *target)
{
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

#include "qnearfieldmanager_emulator.moc"
