/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TST_QMEDIARECORDER_XA_H
#define TST_QMEDIARECORDER_XA_H

#include <QtTest/QtTest>
#include <QDebug>
#include <qmediaobject.h>
#include <qmediacontrol.h>
#include <qmediaservice.h>
#include <qmediarecordercontrol.h>
#include <qmediarecorder.h>
#include <qaudioendpointselector.h>
#include <qaudioencodercontrol.h>
#include <qmediacontainercontrol.h>
#include <QtMultimedia/qaudioformat.h>
#include <qaudiocapturesource.h>

QT_USE_NAMESPACE
class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:

    // Positive test cases
    void testMediaRecorderObject();
    void testAudioSink();
    void testAudioEndPointSelector();
    void testDefaultAudioEncodingSettings();
    void testAudioWav();
    void testAudioWavSr8kHz();
    void testAudioWavSr16kHz();
    void testAudioWavSr32kHz();
    void testAudioWavSr96kHz();
    void testAudioWavSr8kHzStereo();
    void testAudioWavSr16kHzStereo();
    void testAudioWavSr32kHzStereo();
    void testAudioWavSr96kHzStereo();
    void testAudioWavQualityVeryLow();
    void testAudioWavQualityLow();
    void testAudioWavQualityNormal();
    void testAudioWavQualityHigh();
    void testAudioWavQualityVeryHigh();
    // Negetive test cases
    void testAudioWavSr4kHz();
    void testAudioWavSr8kHz5Channel();

    // Positive test cases
    void testAudioAmr();
    void testAudioAmrBr4750bps();
    void testAudioAmrBr5150bps();
    void testAudioAmrBr5900bps();
    void testAudioAmrBr6700bps();
    void testAudioAmrBr7400bps();
    void testAudioAmrBr7950bps();
    void testAudioAmrBr10200bps();
    void testAudioAmrBr12200bps();
    void testAudioAmrQualityVeryLow();
    void testAudioAmrQualityLow();
    void testAudioAmrQualityNormal();
    void testAudioAmrQualityHigh();
    void testAudioAmrQualityVeryHigh();
    // Negetive test cases
    void testAudioAmrSr8kHzMono();
    void testAudioAmrStereo();
    void testAudioAmrSr16kHz();

    // Positive test cases
    void testAudioMpegBr32kSr8kHz();
    void testAudioMpegBr32kbpsSr8kHzStereo();
    void testAudioMpegBr128kbpsSr16kHz();
    void testAudioMpegBr128kbpsSr16kHzStereo();
    void testAudioMpegBr192kbpsSr32kHz();
    void testAudioMpegBr192kbpsSr32kHzStereo();
    void testAudioMpegBr256kbpsSr48kHz();
    void testAudioMpegBr256kbpsSr48kHzStereo();
    void testAudioMpegQualityVeryLow();
    void testAudioMpegQualityVeryHigh();
    // Negetive test cases
    void testAudioMpegBr16kSr8kHz();

private:

    void testAudioMpeg();
    void testAudioMpegQualityLow();
    void testAudioMpegQualityNormal();
    void testAudioMpegQualityHigh();

    QUrl nextFileName(QDir outputDir, QString appendName, QString ext);

    QAudioEncoderControl *audioEncoder;
    QAudioEndpointSelector *audioEndpoint;
    QMediaRecorder  *audiocapture;
    QAudioCaptureSource *captureSource;
};

#endif /* TST_QMEDIARECORDER_XA_H */
