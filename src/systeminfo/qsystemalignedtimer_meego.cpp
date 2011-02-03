/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "qsystemalignedtimer.h"
#include "qsystemalignedtimer_meego_p.h"

QTM_BEGIN_NAMESPACE

QSystemAlignedTimerPrivate::QSystemAlignedTimerPrivate(QObject *parent) :
    QObject(parent),
    m_lastError(QSystemAlignedTimer::NoError),
    m_minimumInterval(0),
    m_maximumInterval(0),
    m_running(false),
    m_singleShot(false),
    m_iphbdHandler(0),
    m_notifier(0)
{
    m_iphbdHandler = iphb_open(0);

    if (!m_iphbdHandler) {
        m_lastError = QSystemAlignedTimer::InternalError;
        return;
    }

    int sockfd = iphb_get_fd(m_iphbdHandler);
    if (!(sockfd > -1)) {
        m_lastError = QSystemAlignedTimer::InternalError;
        return;
    }

    m_notifier = new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
    if (!QObject::connect(m_notifier, SIGNAL(activated(int)), this, SLOT(heartbeatReceived(int)))) {
        delete m_notifier, m_notifier = 0;
        m_lastError = QSystemAlignedTimer::TimerFailed;
        return;
    }
    m_notifier->setEnabled(false);
}

QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate()
{
    if (m_iphbdHandler) {
        (void)iphb_close(m_iphbdHandler), m_iphbdHandler = 0;
    }

    if (m_notifier) {
        delete m_notifier, m_notifier = 0;
    }
}

void QSystemAlignedTimerPrivate::wokeUp()
{
    if (m_singleShot) {
        stop();
        return;
    }

    int st = iphb_I_woke_up(m_iphbdHandler);
    if (!(st >= 0)) {
        m_lastError = QSystemAlignedTimer::TimerFailed;
        emit error(m_lastError);
        stop();
    }
}

int QSystemAlignedTimerPrivate::minimumInterval() const
{
    return m_minimumInterval;
}

void QSystemAlignedTimerPrivate::setMinimumInterval(int seconds)
{
    m_minimumInterval = seconds;
}

int QSystemAlignedTimerPrivate::maximumInterval() const
{
    return m_maximumInterval;
}

void QSystemAlignedTimerPrivate::setMaximumInterval(int seconds)
{
    m_maximumInterval = seconds;
}

void QSystemAlignedTimerPrivate::setSingleShot(bool singleShot)
{
    m_singleShot = singleShot;
}

bool QSystemAlignedTimerPrivate::isSingleShot() const
{
    return m_singleShot;
}

void QSystemAlignedTimerPrivate::singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member)
{
    QSystemAlignedTimerPrivate *alignedTimer = new QSystemAlignedTimerPrivate();

    if (QObject::connect(alignedTimer, SIGNAL(timeout()), alignedTimer, SLOT(singleShot()))) {
        alignedTimer->m_singleShotReceiver = receiver;
        alignedTimer->m_singleShotMember = member;
        alignedTimer->start(minimumTime, maximumTime);
    } else {
        delete alignedTimer;
    }
}

QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimerPrivate::lastError() const
{
    return m_lastError;
}

// public slots

void QSystemAlignedTimerPrivate::start(int minimumTime, int maximumTime)
{
    m_minimumInterval = minimumTime;
    m_maximumInterval = maximumTime;

    start();
}

void QSystemAlignedTimerPrivate::start()
{
    if (m_running) {
        return;
    }

    if (!(m_iphbdHandler && m_notifier)) {
        m_lastError = QSystemAlignedTimer::InternalError;
        emit error(m_lastError);
        return;
    }

    int mustWait = 0;
    time_t unixTime = iphb_wait(m_iphbdHandler, m_minimumInterval, m_maximumInterval, mustWait);

    if (unixTime == (time_t)-1) {
        m_lastError = QSystemAlignedTimer::TimerFailed;
        emit error(m_lastError);
        return;
    }

    m_notifier->setEnabled(true);
    m_running = true;
    m_lastError = QSystemAlignedTimer::NoError;
}

void QSystemAlignedTimerPrivate::stop()
{
    if (!m_running) {
        return;
    }

    if (!(m_iphbdHandler && m_notifier)) {
        m_lastError = QSystemAlignedTimer::InternalError;
        emit error(m_lastError);
        return;
    }

    (void)iphb_discard_wakeups(m_iphbdHandler);

    m_notifier->setEnabled(false);
    m_running = false;
    m_lastError = QSystemAlignedTimer::NoError;
}

// private slots

void QSystemAlignedTimerPrivate::heartbeatReceived(int sock) {
    Q_UNUSED(sock);

    stop();
    emit timeout();

    if (!m_singleShot) {
        start();
    }
}

void QSystemAlignedTimerPrivate::singleShot() {
   QMetaObject::invokeMethod(m_singleShotReceiver, m_singleShotMember);
   this->deleteLater();
}

bool	QSystemAlignedTimerPrivate::isActive () const
{
    return m_running;
}

#include "moc_qsystemalignedtimer_meego_p.cpp"

QTM_END_NAMESPACE
