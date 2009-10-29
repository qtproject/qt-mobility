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

class S60CameraSession;

class S60VideoEncoder : public QVideoEncoderControl
{
    Q_OBJECT
public:
    S60VideoEncoder(QObject *parent = 0);
    S60VideoEncoder(QObject *session, QObject *parent = 0);
    virtual ~S60VideoEncoder();

    // TODO move inside S60QVideoEncoderSettings
    //QSize resolution() const;
    //void setResolution(const QSize &);
    QList<QSize> supportedResolutions() const;

    QSize minimumResolution() const;
    QSize maximumResolution() const;

    // TODO move inside S60QVideoEncoderSettings
    //qreal frameRate() const;
    //void setFrameRate(qreal &rate);

    qreal minimumFrameRate() const;
    qreal maximumFrameRate() const;
    QList< qreal >  supportedFrameRates() const;

    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;

    QVideoEncoderSettings videoSettings() const;
    void setVideoSettings(const QVideoEncoderSettings &settings);

    // TODO move inside S60QVideoEncoderSettings
    //QString videoCodec() const;
    //bool setVideoCodec(const QString &codecName);
    //QString audioCodec() const;
    // TODO move inside S60QVideoEncoderSettings
    //int bitrate() const;
    //void setBitrate(int);
    // TODO move inside S60QVideoEncoderSettings
    //int quality() const;
    //void setQuality(int);

    QStringList supportedEncodingOptions(const QString &codec) const;
    QVariant encodingOption(const QString &codec, const QString &name) const;
    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value);

private:
    S60CameraSession* m_session;

    // TODO move these when creating S60QVideoEncoderSettings
    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QStringList> m_codecOptions;

    QString m_codec;
    QMap<QString, QMap<QString, QVariant> > m_options;
    QSize m_resolution;
    qreal m_frameRate;

    QVideoEncoderSettings m_videoSettings;


};

#endif
