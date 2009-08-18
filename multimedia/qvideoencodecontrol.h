/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVIDEOENCODECONTROL_H
#define QVIDEOENCODECONTROL_H

#include "qabstractmediacontrol.h"

#include <QtCore/qpair.h>
#include <QtCore/qsize.h>

class QByteArray;
class QStringList;

class Q_MEDIA_EXPORT QVideoEncodeControl : public QAbstractMediaControl
{
    Q_OBJECT
public:
    virtual ~QVideoEncodeControl();

    virtual QSize resolution() const = 0;
    virtual QSize minimumResolution() const = 0;
    virtual QSize maximumResolution() const = 0;
    virtual QList<QSize> supportedResolutions() const;
    virtual void setResolution(const QSize &) = 0;

    virtual QPair<int,int> frameRate() const = 0;
    virtual QPair<int,int> minumumFrameRate() const = 0;
    virtual QPair<int,int> maximumFrameRate() const = 0;
    virtual QList< QPair<int,int> > supportedFrameRates() const;
    virtual void setFrameRate(QPair<int,int>) = 0;

    virtual QStringList supportedVideoCodecs() const = 0;
    virtual QString videoCodec() const = 0;
    virtual bool setVideoCodec(const QString &codecName) = 0;

    virtual QString videoCodecDescription(const QString &codecName) const = 0;


    virtual int bitrate() const = 0;
    virtual void setBitrate(int) = 0;

    virtual qreal quality() const = 0;
    virtual void setQuality(qreal) = 0;

    virtual QStringList supportedEncodingOptions() const;
    virtual QVariant encodingOption(const QString &name);
    virtual void setEncodingOption(const QString &name, const QVariant &value);

protected:
    QVideoEncodeControl(QObject *parent);
};

#define QVideoEncodeControl_iid "com.nokia.qt.VideoEncodeControl"
Q_MEDIA_DECLARE_CONTROL(QVideoEncodeControl, QVideoEncodeControl_iid)

#endif // QVIDEOCAPTUREPROPERTIESCONTROL_H
