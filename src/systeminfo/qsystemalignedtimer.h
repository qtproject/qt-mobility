/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSYSTEMALIGNEDTIMER_H
#define QSYSTEMALIGNEDTIMER_H

#include <QObject>
#include "qmobilityglobal.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemAlignedTimerPrivate;

class Q_SYSINFO_EXPORT QSystemAlignedTimer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int minimumInterval READ minimumInterval WRITE setMinimumInterval)
    Q_PROPERTY(int maximumInterval READ maximumInterval WRITE setMaximumInterval)
    Q_PROPERTY(bool singleShot READ isSingleShot WRITE setSingleShot)
    Q_PROPERTY(bool active READ isActive)

public:
    explicit QSystemAlignedTimer(QObject *parent = 0);
    ~QSystemAlignedTimer();

    enum AlignedTimerError {
        NoError = 0,
        AlignedTimerNotSupported,
        InvalidArgument,
        TimerFailed,
        InternalError
    };

    Q_INVOKABLE void wokeUp();

    int minimumInterval() const;
    void setMinimumInterval(int seconds);

    int maximumInterval() const;
    void setMaximumInterval(int seconds);

    void setSingleShot(bool single);
    bool isSingleShot() const;

    Q_INVOKABLE static void singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member);

    AlignedTimerError lastError() const;
    bool isActive () const;

public Q_SLOTS:
    void start(int minimumTime, int maximumTime);
    void start();
    void stop();

Q_SIGNALS:
    void timeout();
    void error(QSystemAlignedTimer::AlignedTimerError error);

private:
    QSystemAlignedTimerPrivate *d;
};

// FIXME how to know if this fails on some platforms where it is not supported, i.e. windows or symbian
// before mobility 1.2

QTM_END_NAMESPACE
QT_END_HEADER

#endif // QSYSTEMALIGNEDTIMER_H
