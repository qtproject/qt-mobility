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
#include "qsystemalignedtimer_stub_p.h"

QTM_BEGIN_NAMESPACE

QSystemAlignedTimerPrivate::QSystemAlignedTimerPrivate(QObject *parent) :
    QObject(parent)
{
}

QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate()
{
}

void QSystemAlignedTimerPrivate::wokeUp()
{
}

int QSystemAlignedTimerPrivate::minimumInterval() const
{
    return -1;
}

void QSystemAlignedTimerPrivate::setMinimumInterval(int seconds)
{
    Q_UNUSED(seconds);
}

int QSystemAlignedTimerPrivate::maximumInterval() const
{
    return -1;
}

void QSystemAlignedTimerPrivate::setMaximumInterval(int seconds)
{
    Q_UNUSED(seconds);
}

void QSystemAlignedTimerPrivate::setSingleShot(bool singleShot)
{
    Q_UNUSED(singleShot);
}

bool QSystemAlignedTimerPrivate::isSingleShot() const
{
    return false;
}

void QSystemAlignedTimerPrivate::singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member)
{
    Q_UNUSED(minimumTime);
    Q_UNUSED(maximumTime);
    Q_UNUSED(receiver);
    Q_UNUSED(member);
}

QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimerPrivate::lastError() const
{
    return QSystemAlignedTimer::AlignedTimerNotSupported;
}

void QSystemAlignedTimerPrivate::start(int minimumTime, int maximumTime)
{
    Q_UNUSED(minimumTime);
    Q_UNUSED(maximumTime);
}

void QSystemAlignedTimerPrivate::start()
{
}

void QSystemAlignedTimerPrivate::stop()
{
}

bool QSystemAlignedTimerPrivate::isActive () const
{
    return false;
}

#include "moc_qsystemalignedtimer_stub_p.cpp"

QTM_END_NAMESPACE
