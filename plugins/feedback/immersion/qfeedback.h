/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#ifndef QFEEDBACK_IMMERSION_H
#define QFEEDBACK_IMMERSION_H

#include <qmobilityglobal.h>
#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QMutex>

#include <qfeedbackplugin.h>

#include <ImmVibe.h>

#define DEVICE_COUNT 3

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QFeedbackImmersion : public QObject, public QFeedbackInterface, public QFileFeedbackInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QFeedbackInterface)
    Q_INTERFACES(QtMobility::QFileFeedbackInterface)
public:
    QFeedbackImmersion();
    virtual ~QFeedbackImmersion();

    virtual QFeedbackDevice defaultDevice(QFeedbackDevice::Type);
    virtual QList<QFeedbackDevice> devices();

    //for device handling
    virtual QString deviceName(const QFeedbackDevice &);
    virtual QFeedbackDevice::State deviceState(const QFeedbackDevice &);
    virtual QFeedbackDevice::Capabilities supportedCapabilities(const QFeedbackDevice &);
    virtual QFeedbackDevice::Type type(const QFeedbackDevice &);
    virtual bool isEnabled(const QFeedbackDevice &);
    virtual void setEnabled(const QFeedbackDevice &, bool);

    virtual QFeedbackEffect::ErrorType updateEffectProperty(const QFeedbackEffect *, EffectProperty);
    virtual QFeedbackEffect::ErrorType updateEffectState(const QFeedbackEffect *);
    virtual QAbstractAnimation::State actualEffectState(const QFeedbackEffect *);

    //for loading files
    virtual bool setLoaded(const QFileFeedbackEffect*, bool);
    virtual QFileFeedbackEffect::ErrorType updateEffectState(const QFileFeedbackEffect *);
    virtual QAbstractAnimation::State actualEffectState(const QFileFeedbackEffect *);
    virtual int effectDuration(const QFileFeedbackEffect *);
    virtual QStringList mimeTypes();

private:
    VibeInt32 handleForDevice(const QFeedbackDevice &device);
    VibeInt32 handleForDevice(int devId);
    static QFeedbackDevice::Type convert(VibeInt32 t);
    static VibeInt32 convertedDuration(int duration);

    QMutex mutex;
    int defaultDevices[DEVICE_COUNT];
    QVector<VibeInt32> deviceHandles;
    QHash<const QAbstractAnimation*, VibeInt32> effectHandles;

    struct FileContent {
        FileContent() : refCount(0) { }
        FileContent(const QByteArray &arr) : ba(arr), refCount(1) { }
        const VibeUInt8 *constData() const {return reinterpret_cast<const VibeUInt8 *>(ba.constData()); }

        QByteArray ba;
        int refCount;
    };
    QHash<QString, FileContent> fileData;
};


QT_END_HEADER

#endif