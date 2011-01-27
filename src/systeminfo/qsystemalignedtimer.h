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


#ifndef QSYSTEMALIGNEDTIMER_H
#define QSYSTEMALIGNEDTIMER_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include "qmobilityglobal.h"
#include "qsystemalignedtimer.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemAlignedTimerPrivate;

class Q_SYSINFO_EXPORT QSystemAlignedTimer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int minimumInterval READ minimumInterval WRITE setMinimumInterval)
    Q_PROPERTY(int maximumInterval READ maximumInterval WRITE setMaximumInterval)

    Q_PROPERTY(bool running READ running WRITE setRunning CONSTANT)
    Q_PROPERTY(bool singleShot READ isSingleShot WRITE setSingleShot CONSTANT)
public:

    explicit QSystemAlignedTimer(QObject *parent = 0);
    ~QSystemAlignedTimer();

    Q_INVOKABLE void wokeUp();  // This should be called when the application wakes up via other
    // means than QSystemAlignedTimer timeout.
    // Other applications that are in their wakeup window
    // may be woken up.  Single-shot timer is stopped.

    /*
     *
     * @param minimumInterval   Time in milliseconds that MUST be waited before timeout.
     *                  Value 0 means 'wake me up when someboy else is woken'.
     *                  It  is recommended that the first wait (if possible) uses minvalue as 0 to "jump to the train"
     * @param maximumInterval   Time in milliseconds when the wait MUST end. It is wise to have maxtime-mintime
     *                  quite big so all users of this service get synced.
     *                  For example if you preferred wait is 120 seconds, use minval 110 and maxval 130.
     */
    int minimumInterval() const;
    void setMinimumInterval(int seconds) const;

    int maximumInterval() const;
    void setMaximumInterval(int seconds) const;

    void setSingleShot(bool singleShot);
    bool isSingleShot() const;

    Q_INVOKABLE static void singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member);

    bool running() const;
    void setRunning(bool running);
    // running is a QML standard way of doing something like this, so let's use this

public Q_SLOTS:
    void start(int minimumTime, int maximumTime);  // See setInterval

    void start();
    void stop();

Q_SIGNALS:
    void timeout();

private:
    QSystemAlignedTimerPrivate *d;
};



class QSystemAlignedTimerPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemAlignedTimerPrivate(QObject *parent = 0){Q_UNUSED(parent)};

    int id;
    bool isTimerRunning;
    bool single;

private:
    QTimer *alignedTimer;
     void timerEvent(QTimerEvent *);
};


QTM_END_NAMESPACE

QT_END_HEADER

#endif // QSYSTEMALIGNEDTIMER_H
