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

    Q_PROPERTY(int timerWindow READ timerWindow WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

    Q_PROPERTY(bool running READ isRunning WRITE setRunning CONSTANT)
    Q_PROPERTY(bool singleShot READ isSingleShot WRITE setSingleShot)
public:

    explicit QSystemAlignedTimer(QObject *parent = 0);
    ~QSystemAlignedTimer();

    Q_INVOKABLE bool wokeUp();

    void setWindow(int timerWindow);
    int timerWindow() const;

    void setInterval(int minTime, int maxTime = 0);
    int interval() const;

    inline void setSingleShot(bool singleShot);
    inline bool isSingleShot() const;


    Q_INVOKABLE static void singleShot(int msec, QObject *receiver, const char *member);

    bool isRunning() const { return isTimerRunning; }
    void setRunning(bool running);

public Q_SLOTS:
    void start(int sec);

    void start();
    void stop();

Q_SIGNALS:
    void timeout();
    void intervalChanged(int newInterval);
    void windowChanged(int newWindow);

private:
    QSystemAlignedTimerPrivate *d;
    int id;
    int preferredInterval;
    int currentTimerWindow;
    bool isTimerRunning;
    bool single;
};



class QSystemAlignedTimerPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QSystemAlignedTimerPrivate(QObject *parent = 0){Q_UNUSED(parent)};

private:
    QTimer *alignedTimer;
     void timerEvent(QTimerEvent *);
};


QTM_END_NAMESPACE

QT_END_HEADER

#endif // QSYSTEMALIGNEDTIMER_H
