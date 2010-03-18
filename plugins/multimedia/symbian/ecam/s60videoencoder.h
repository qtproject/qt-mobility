/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef S60VIDEOENCODE_H
#define S60VIDEOENCODE_H

#include "qvideoencodercontrol.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

QTM_USE_NAMESPACE

class S60VideoCaptureSession;

class S60VideoEncoder : public QVideoEncoderControl
{
    Q_OBJECT
public:
    S60VideoEncoder(QObject *parent = 0);
    S60VideoEncoder(QObject *session, QObject *parent = 0);
    virtual ~S60VideoEncoder();
          
    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;
    QString videoCodec() const;
    void setVideoCodec(const QString &codecName);    
    
    int bitRate() const;
    void setBitRate(int);

    QtMedia::EncodingQuality quality() const;
    void setQuality(QtMedia::EncodingQuality);

    QList<qreal>  supportedFrameRates(const QVideoEncoderSettings &settings, bool *continuous = 0) const;
    qreal minimumFrameRate() const;
    qreal maximumFrameRate() const;
    qreal frameRate() const;
    void setFrameRate(qreal frameRate);    

    QList<QSize> supportedResolutions(const QVideoEncoderSettings &settings, bool *continuous = 0) const;
    QSize minimumResolution() const;
    QSize maximumResolution() const;
    QSize videoResolution() const;
    void setResolution(const QSize resolution);    

    QStringList supportedEncodingOptions(const QString &codec) const;
    QVariant encodingOption(const QString &codec, const QString &name) const;
    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value);
    
    QVideoEncoderSettings videoSettings() const;
    void setVideoSettings(const QVideoEncoderSettings &settings);

private:
    S60VideoCaptureSession* m_session;

/*    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QStringList> m_codecOptions;

    QString m_codec;
    QMap<QString, QMap<QString, QVariant> > m_options;
    QSize m_resolution;
    qreal m_frameRate;

    QVideoEncoderSettings m_videoSettings;*/


};

#endif
