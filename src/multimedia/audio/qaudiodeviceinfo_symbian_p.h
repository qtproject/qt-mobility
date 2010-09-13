/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QAUDIODEVICEINFO_SYMBIAN_P_H
#define QAUDIODEVICEINFO_SYMBIAN_P_H

#include <qaudiosystem.h>
#include <sounddevice.h>

QT_BEGIN_NAMESPACE

class QAudioDeviceInfoInternal
    :   public QAbstractAudioDeviceInfo
{
    Q_OBJECT

public:
    QAudioDeviceInfoInternal(QByteArray device, QAudio::Mode mode);
    ~QAudioDeviceInfoInternal();

    // QAbstractAudioDeviceInfo
    QAudioFormat preferredFormat() const;
    bool isFormatSupported(const QAudioFormat &format) const;
    QString deviceName() const;
    QStringList supportedCodecs();
    QList<int> supportedSampleRates();
    QList<int> supportedChannelCounts();
    QList<int> supportedSampleSizes();
    QList<QAudioFormat::Endian> supportedByteOrders();
    QList<QAudioFormat::SampleType> supportedSampleTypes();
    static QByteArray defaultInputDevice();
    static QByteArray defaultOutputDevice();
    static QList<QByteArray> availableDevices(QAudio::Mode);

private:
    void getSupportedFormats() const;

private:
    QScopedPointer<CMMFDevSound> m_devsound;

    QString m_deviceName;
    QAudio::Mode m_mode;

    // Mutable to allow lazy initialization when called from const-qualified
    // public functions (isFormatSupported, nearestFormat)
    mutable bool m_updated;
    mutable QStringList m_codecs;
    mutable QList<int> m_frequencies;
    mutable QList<int> m_channels;
    mutable QList<int> m_sampleSizes;
    mutable QList<QAudioFormat::Endian> m_byteOrders;
    mutable QList<QAudioFormat::SampleType> m_sampleTypes;
};

QT_END_NAMESPACE

#endif
