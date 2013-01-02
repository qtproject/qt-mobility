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

#ifndef S60AUDIOENCODERCONTROL_H
#define S60AUDIOENCODERCONTROL_H

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <qaudioencodercontrol.h>

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60VideoCaptureSession)

/*
 * Control for audio settings when recording video using QMediaRecorder.
 */
class S60AudioEncoderControl : public QAudioEncoderControl
{
    Q_OBJECT

public: // Constructor & Destructor

    S60AudioEncoderControl(QObject *parent = 0);
    S60AudioEncoderControl(S60VideoCaptureSession *session, QObject *parent = 0);
    virtual ~S60AudioEncoderControl();

public: // QAudioEncoderControl

    // Audio Codec
    QStringList supportedAudioCodecs() const;
    QString codecDescription(const QString &codecName) const;

    // Sample Rate
    QList<int> supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous = 0) const;

    // Audio Settings
    QAudioEncoderSettings audioSettings() const;
    void setAudioSettings(const QAudioEncoderSettings &settings);

    // Encoding Option
    QStringList supportedEncodingOptions(const QString &codec) const;
    QVariant encodingOption(const QString &codec, const QString &name) const;
    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value);

private: // Data

    S60VideoCaptureSession *m_session;
};

#endif // S60AUDIOENCODERCONTROL_H
