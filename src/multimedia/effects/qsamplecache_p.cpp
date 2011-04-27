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

#include "qsamplecache_p.h"
#include "qwavedecoder_p.h"
#include <QtNetwork>

//#define QT_SAMPLECACHE_DEBUG

QT_BEGIN_NAMESPACE


/*!
    \class QSampleCache

    When you want to get a sound sample data, you need to request the QSample reference from QSampleCache.

    \since 1.1

    \code
        QSample *m_sample;     // class member.

      private Q_SLOTS:
        void decoderError();
        void sampleReady();
    \endcode

    \code
      Q_GLOBAL_STATIC(QSampleCache, sampleCache) //declare a singleton manager
    \endcode

    \code
        m_sample = sampleCache()->requestSample(url);
        switch(m_sample->state()) {
        case QSample::Ready:
            sampleReady();
            break;
        case QSample::Error:
            decoderError();
            break;
        default:
            connect(m_sample, SIGNAL(error()), this, SLOT(decoderError()));
            connect(m_sample, SIGNAL(ready()), this, SLOT(sampleReady()));
            break;
        }
    \endcode

    When you no longer need the sound sample data, you need to release it:

    \code
       if (m_sample) {
           m_sample->release();
           m_sample = 0;
       }
    \endcode
*/


QSampleCache::QSampleCache()
    : m_networkAccessManager(0)
    , m_mutex(QMutex::Recursive)
    , m_capacity(0)
    , m_usage(0)
{
}

QNetworkAccessManager& QSampleCache::networkAccessManager()
{
    if (!m_networkAccessManager)
        m_networkAccessManager = new QNetworkAccessManager();
    return *m_networkAccessManager;
}

QSampleCache::~QSampleCache()
{
    for (QMap<QUrl, QSample*>::iterator it = m_samples.begin();
        it != m_samples.end(); ++it)
    {
        (*it)->deleteLater();
    }
    if (m_networkAccessManager)
        m_networkAccessManager->deleteLater();
    m_loadingThread.quit();
    m_loadingThread.wait();
}

QSample* QSampleCache::requestSample(const QUrl& url)
{
    if (!m_loadingThread.isRunning())
        m_loadingThread.start();
#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSampleCache: request sample [" << url << "]";
#endif
    QMutexLocker locker(&m_mutex);
    QMap<QUrl, QSample*>::iterator it = m_samples.find(url);
    QSample* sample;
    if (it == m_samples.end()) {
        sample = new QSample(url, this);
        it = m_samples.insert(url, sample);
        sample->moveToThread(&m_loadingThread);
    } else {
        sample = *it;
    }

    QSample::State state = sample->state();
    if (state == QSample::Error || state == QSample::Creating) {
        sample->m_state = QSample::Loading;
        QMetaObject::invokeMethod(sample, "load", Qt::QueuedConnection);
    }

    sample->addRef();
    return sample;
}

void QSampleCache::setCapacity(qint64 capacity)
{
    QMutexLocker locker(&m_mutex);
    if (m_capacity == capacity)
        return;
#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSampleCache: capacity changes from " << m_capacity << "to " << capacity;
#endif
    if (m_capacity > 0 && capacity <= 0) { //memory management strategy changed
        for (QMap<QUrl, QSample*>::iterator it = m_samples.begin();
            it != m_samples.end();)
        {
            QSample* sample = *it;
            if (sample->m_ref == 0) {
                m_usage -= sample->m_soundData.size();
                sample->deleteLater();
                it = m_samples.erase(it);
            }
        }
    }

    m_capacity = capacity;
    refresh(0);
}

void QSampleCache::refresh(qint64 usageChange)
{
    QMutexLocker locker(&m_mutex);
    m_usage += usageChange;
    if (m_capacity <= 0 || m_usage <= m_capacity)
        return;

#ifdef QT_SAMPLECACHE_DEBUG
    qint64 recoveredSize = 0;
#endif

    //free unsed samples to keep usage under capacity limit.
    for (QMap<QUrl, QSample*>::iterator it = m_samples.begin();
        it != m_samples.end();)
    {
        QSample* sample = *it;
        if (sample->m_ref > 0) {
            ++it;
            continue;
        }
#ifdef QT_SAMPLECACHE_DEBUG
        recoveredSize += sample->m_soundData.size();
#endif
        m_usage -= sample->m_soundData.size();
        sample->deleteLater();
        it = m_samples.erase(it);
        if (m_usage <= m_capacity)
            return;
    }

#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSampleCache: refresh(" << usageChange
             << ") recovered size =" << recoveredSize
             << "new usage =" << m_usage;
#endif

    if (m_usage > m_capacity)
        qWarning() << "QSampleCache: usage[" << m_usage << " out of limit[" << m_capacity << "]";
}

QSample::~QSample()
{
#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSample: deleted [" << m_url << "]";
#endif
    cleanup();
}

bool QSampleCache::tryRemoveUnrefSample(const QUrl& url)
{
    QMutexLocker locker(&m_mutex);
    if (m_capacity > 0)
        return false;
    m_samples.remove(url);
    return true;
}

void QSample::release()
{
    QMutexLocker locker(&m_mutex);
    m_ref--;
    if (m_ref == 0 && m_parent->tryRemoveUnrefSample(m_url))
        this->deleteLater();
}

void QSample::cleanup()
{
    if (m_waveDecoder) {
        m_waveDecoder->deleteLater();
        m_waveDecoder = 0;
    }
    if (m_stream) {
        m_stream->deleteLater();
        m_stream = 0;
    }
}

void QSample::addRef()
{
    m_ref++;
}

void QSample::readSample()
{
#ifdef  QT_SAMPLECACHE_DEBUG
    qDebug() << "QSample: readSample";
#endif
    qint64 read = m_waveDecoder->read(m_soundData.data() + m_sampleReadLength,
                      qMin(m_waveDecoder->bytesAvailable(),
                           qint64(m_waveDecoder->size() - m_sampleReadLength)));
    if (read > 0)
        m_sampleReadLength += read;
    if (m_sampleReadLength < m_waveDecoder->size())
        return;
    Q_ASSERT(m_sampleReadLength == qint64(m_soundData.size()));
    onReady();
}

void QSample::decoderReady()
{
#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSample: decoder ready";
#endif
    m_parent->refresh(m_waveDecoder->size());

    m_soundData.resize(m_waveDecoder->size());
    m_sampleReadLength = 0;
    qint64 read = m_waveDecoder->read(m_soundData.data(), m_waveDecoder->size());
    if (read > 0)
        m_sampleReadLength += read;
    if (m_sampleReadLength >= m_waveDecoder->size())
        onReady();
}

QSample::State QSample::state() const
{
    return m_state;
}

void QSample::load()
{
#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSample: load [" << m_url << "]";
#endif
    m_stream = m_parent->networkAccessManager().get(QNetworkRequest(m_url));
    connect(m_stream, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(decoderError()));
    m_waveDecoder = new QWaveDecoder(m_stream);
    connect(m_waveDecoder, SIGNAL(formatKnown()), SLOT(decoderReady()));
    connect(m_waveDecoder, SIGNAL(invalidFormat()), SLOT(decoderError()));
    connect(m_waveDecoder, SIGNAL(readyRead()), SLOT(readSample()));
}

void QSample::decoderError()
{
#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSample: decoder error";
#endif
    cleanup();
    m_state = QSample::Error;
    emit error();
}

void QSample::onReady()
{
#ifdef QT_SAMPLECACHE_DEBUG
    qDebug() << "QSample: load ready";
#endif
    m_audioFormat = m_waveDecoder->audioFormat();
    cleanup();
    m_state = QSample::Ready;
    emit ready();
}

QSample::QSample(const QUrl& url, QSampleCache *parent)
    : m_parent(parent)
    , m_stream(0)
    , m_waveDecoder(0)
    , m_url(url)
    , m_sampleReadLength(0)
    , m_state(Creating)
    , m_ref(0)
{
}

QT_END_NAMESPACE

#include "moc_qsamplecache_p.cpp"
