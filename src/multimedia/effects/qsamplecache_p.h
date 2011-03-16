/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSAMPLECACHE_P_H
#define QSAMPLECACHE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qobject.h>
#include <QtCore/qthread.h>
#include <QtCore/qurl.h>
#include <QtCore/qmutex.h>
#include <QtCore/qmap.h>
#include <qaudioformat.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;
class QSampleCache;
class QWaveDecoder;

class QSample : public QObject
{
    Q_OBJECT
public:
    friend class QSampleCache;
    enum State
    {
        Creating,
        Loading,
        Error,
        Ready,
    };

    State state() const;
    const QByteArray& data() const { Q_ASSERT(state() == Ready); return m_soundData; }
    const QAudioFormat& format() const { Q_ASSERT(state() == Ready); return m_audioFormat; }
    void release();

Q_SIGNALS:
    void error();
    void ready();

public Q_SLOTS:
    void load();
    void decoderError();
    void readSample();
    void decoderReady();

protected:
    QSample(const QUrl& url, QSampleCache *parent);

private:
    void onReady();
    void cleanup();
    void addRef();
    ~QSample();

    mutable QMutex m_mutex;
    QSampleCache *m_parent;
    QByteArray   m_soundData;
    QAudioFormat m_audioFormat;
    QIODevice    *m_stream;
    QWaveDecoder *m_waveDecoder;
    QUrl         m_url;
    qint64       m_sampleReadLength;
    State        m_state;
    int          m_ref;
};

class QSampleCache : QObject
{
    Q_OBJECT
public:
    friend class QSample;
    QSampleCache();

    ~QSampleCache();

    QSample* requestSample(const QUrl& url);

    void setCapacity(qint64 capacity);

private:
    QMap<QUrl, QSample*> m_samples;
    QNetworkAccessManager *m_networkAccessManager;
    QMutex m_mutex;
    qint64 m_capacity;
    qint64 m_usage;
    QThread m_loadingThread;

    QNetworkAccessManager& networkAccessManager();
    void refresh(qint64 usageChange);
    bool tryRemoveUnrefSample(const QUrl& url);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSAMPLECACHE_P_H
