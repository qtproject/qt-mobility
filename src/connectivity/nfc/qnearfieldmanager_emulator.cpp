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
#include "qnearfieldtagtype1.h"
#include "qndefmessage.h"

#include <QtCore/QDirIterator>
#include <QtCore/QSettings>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

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

protected:
    void timerEvent(QTimerEvent *e);

signals:
    void tagActivated(TagBase *tag);

private:
    QMap<TagBase *, bool>::Iterator m_current;
} tagActivator;

TagActivator::TagActivator()
{

}

TagActivator::~TagActivator()
{
    qDebug() << Q_FUNC_INFO;

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

    startTimer(100);
}

void TagActivator::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);

    tagMutex.lock();

    if (m_current != tagMap.end())
        *m_current = false;

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
public:
    TagType1(TagBase *tag, QObject *parent);
    ~TagType1();

    QByteArray uid() const;

    AccessMethods accessMethods() const;

    QByteArray sendCommand(const QByteArray &command);

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

QByteArray TagType1::sendCommand(const QByteArray &command)
{
    QMutexLocker locker(&tagMutex);

    // tag not in proximity
    if (!tagMap.value(m_tag))
        return QByteArray();

    quint16 crc = qNfcChecksum(command.constData(), command.length());

    QByteArray response = m_tag->processCommand(command + char(crc & 0xff) + char(crc >> 8));

    // check crc
    int responseLength = response.length();
    crc = (quint8(response.at(responseLength - 1)) << 8) | quint8(response.at(responseLength - 2));
    response.chop(2);
    if (crc != qNfcChecksum(response.constData(), response.length()))
        return QByteArray();

    return response;
}

QNearFieldManagerPrivateImpl::QNearFieldManagerPrivateImpl()
{
    tagActivator.initialize();

    connect(&tagActivator, SIGNAL(tagActivated(TagBase*)), this, SLOT(tagActivated(TagBase*)));
}

QNearFieldManagerPrivateImpl::~QNearFieldManagerPrivateImpl()
{
}

int QNearFieldManagerPrivateImpl::getFreeId()
{
    if (!m_freeIds.isEmpty())
        return m_freeIds.takeFirst();

    m_registeredHandlers.append(Callback());
    return m_registeredHandlers.count() - 1;
}

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

#include "qnearfieldmanager_emulator.moc"
