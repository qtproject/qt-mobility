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

#ifndef QAUDIORENDERERENDPOINT_H
#define QAUDIORENDERERENDPOINT_H

#include "qmediaendpointinterface.h"

class Q_MEDIA_EXPORT QAudioRendererEndpointInterface : public QMediaEndpointInterface
{
public:
    virtual ~QAudioRendererEndpointInterface();

    Direction direction() const;
};

#define QAudioRendererEndpointInterface_iid "com.nokia.Qt.QAudioRendererEndpointInterface/1.0"

Q_DECLARE_INTERFACE(QAudioRendererEndpointInterface, QAudioRendererEndpointInterface_iid)

class QIODevice;

class QAudioRendererEndpointPrivate;

class Q_MEDIA_EXPORT QAudioRendererEndpoint
        : public QObject
        , public QAudioRendererEndpointInterface
{
    Q_OBJECT
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency)
    Q_PROPERTY(int channels READ channels WRITE setChannels)
    Q_PROPERTY(int sampleSize READ sampleSize WRITE setSampleSize)
    Q_INTERFACES(QAudioRendererEndpointInterface)
    Q_DECLARE_PRIVATE(QAudioRendererEndpoint)
public:
    QAudioRendererEndpoint(QObject *parent = 0);
    ~QAudioRendererEndpoint();

    int frequency() const;
    virtual void setFrequency(int frequency);
    virtual QList<int> supportedFrequencies() const = 0;

    int channels() const;
    virtual void setChannels(int channels);
    virtual QList<int> supportedChannels() const = 0;

    int sampleSize() const;
    virtual void setSampleSize(int size);
    virtual QList<int> supportedSampleSizes() const = 0;

    QIODevice *device() const;
    virtual void setDevice(QIODevice *device);
};

#endif
