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

#ifndef QAUDIO_SYMBIAN_P_H
#define QAUDIO_SYMBIAN_P_H

#include <QtCore/qnamespace.h>
#include <qaudioformat.h>
#include <qaudio.h>
#include <sounddevice.h>

QT_BEGIN_NAMESPACE

namespace SymbianAudio {

/**
 * Default values used by audio input and output classes, when underlying
 * DevSound instance has not yet been created.
 */

const int DefaultBufferSize = 4096; // bytes
const int DefaultNotifyInterval = 1000; // ms

/**
 * Enumeration used to track state of internal DevSound instances.
 * Values are translated to the corresponding QAudio::State values by
 * SymbianAudio::Utils::stateNativeToQt.
 */
enum State {
        ClosedState
    ,   InitializingState
    ,   ActiveState
    ,   IdleState
    ,   SuspendedState
};

/*
 * Helper class for querying DevSound codec / format support
 */
class DevSoundCapabilities {
public:
    DevSoundCapabilities(CMMFDevSound &devsound, QAudio::Mode mode);
    ~DevSoundCapabilities();

    const RArray<TFourCC>& fourCC() const   { return m_fourCC; }
    const TMMFCapabilities& caps() const    { return m_caps; }

private:
    void constructL(CMMFDevSound &devsound, QAudio::Mode mode);

private:
    RArray<TFourCC> m_fourCC;
    TMMFCapabilities m_caps;
};

namespace Utils {

/**
 * Convert native audio capabilities to QAudio lists.
 */
void capabilitiesNativeToQt(const DevSoundCapabilities &caps,
                            QList<int> &frequencies,
                            QList<int> &channels,
                            QList<int> &sampleSizes,
                            QList<QAudioFormat::Endian> &byteOrders,
                            QList<QAudioFormat::SampleType> &sampleTypes);

/**
 * Check whether format is supported.
 */
bool isFormatSupported(const QAudioFormat &format,
                       const DevSoundCapabilities &caps);

/**
 * Convert QAudioFormat to native format types.
 *
 * Note that, despite the name, DevSound uses TMMFCapabilities to specify
 * single formats as well as capabilities.
 *
 * Note that this function does not modify outputFormat.iBufferSize.
 */
bool formatQtToNative(const QAudioFormat &inputFormat,
                      TUint32 &outputFourCC,
                      TMMFCapabilities &outputFormat);

/**
 * Convert internal states to QAudio states.
 */
QAudio::State stateNativeToQt(State nativeState,
                              QAudio::State initializingState);

/**
 * Convert data length to number of samples.
 */
qint64 bytesToSamples(const QAudioFormat &format, qint64 length);

/**
 * Convert number of samples to data length.
 */
qint64 samplesToBytes(const QAudioFormat &format, qint64 samples);

} // namespace Utils
} // namespace SymbianAudio

QT_END_NAMESPACE

#endif
