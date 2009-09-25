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

#ifndef QAUDIOSOURCE_H
#define QAUDIOSOURCE_H

#include <QtCore/qstringlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qsize.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/qaudioformat.h>
#else
#include <multimedia/qaudioformat.h>
#endif

#include <multimedia/qmediarecorder.h>
#include <multimedia/qabstractmediacontrol.h>
#include <multimedia/qabstractmediaobject.h>
#include <multimedia/qabstractmediaservice.h>

#include <multimedia/qmediaserviceprovider.h>

class QAudioSourceService;
class QAudioSourcePrivate;

class Q_MEDIA_EXPORT QAudioSource : public QAbstractMediaObject
{
    Q_OBJECT

    Q_ENUMS(State)
public:
    QAudioSource(QObject *parent = 0, QAbstractMediaService *service = 0);
    ~QAudioSource();

    bool isValid() const;
    QAbstractMediaService* service() const;

    QAudioFormat format() const;
    void setFormat(const QAudioFormat &format);
    bool isFormatSupported(const QAudioFormat &format) const;

    QStringList supportedCodecs() const;
    QList<int> supportedFrequencies() const;
    QList<int> supportedChannels() const;
    QList<int> supportedSampleSizes() const;

private:
    Q_DECLARE_PRIVATE(QAudioSource)
};

#endif  // QAUDIOSOURCE_H
