/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QAUDIOENCODECONTROL_H
#define QAUDIOENCODECONTROL_H

#include "qabstractmediacontrol.h"

class QAudioFormat;
class QStringList;

#ifndef AUDIOSERVICES
class Q_MEDIA_EXPORT QAudioFormat
{
public:
    QAudioFormat();

    void QAudioFormat::setFrequency(int frequency);
    void QAudioFormat::setChannels(int channels);
    void QAudioFormat::setSampleSize(int sampleSize);

private:
    bool m_null;
    int m_frequency;
    int m_channels;
    int m_sampleSize;
};

#endif

class Q_MEDIA_EXPORT QAudioEncodeControl : public QAbstractMediaControl
{
    Q_OBJECT
public:
    virtual ~QAudioEncodeControl();

    virtual QAudioFormat format() const = 0;    
    virtual bool isFormatSupported(const QAudioFormat &format) const = 0;
    virtual bool setFormat(const QAudioFormat &format) = 0;

    virtual QStringList supportedAudioCodecs() const = 0;
    virtual QString audioCodec() const = 0;
    virtual bool setAudioCodec(const QString &codecName) = 0;

    virtual QString codecDescription(const QString &codecName) = 0;

    virtual int bitrate() const = 0;
    virtual void setBitrate(int) = 0;

    virtual qreal quality() const = 0;
    virtual void setQuality(qreal) = 0;

    virtual QStringList supportedEncodingOptions() const;
    virtual QVariant encodingOption(const QString &name);
    virtual void setEncodingOption(const QString &name, const QVariant &value);

protected:
    QAudioEncodeControl(QObject *parent);
};

#endif // QAUDIOCAPTUREPROPERTIESCONTROL_H
