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

#include "qradiotuner.h"


QRadioTuner::QRadioTuner(QObject *parent):
    QAbstractMediaControl(parent)
{
    addPropertyWatch("duration");
}

QRadioTuner::~QRadioTuner()
{
}

int QRadioTuner::band() const
{
    return property("band").toInt();
}

void QRadioTuner::setBand(int b)
{
    changePropertyValue("band", b);
}

int QRadioTuner::frequency() const
{
    return property("frequency").toInt();
}

void QRadioTuner::setFrequency(int frequency)
{
    changePropertyValue("frequency", frequency);
}

bool QRadioTuner::isStereo() const
{
    return property("stereo").toBool();
}

void QRadioTuner::setStereo(bool stereo)
{
    changePropertyValue("stereo", stereo);
}

int QRadioTuner::signalStrength() const
{
    return property("signalStrength").toInt();
}

void QRadioTuner::setSignalStrength(int ss)
{
    changePropertyValue("signaStrength", ss);
}


qint64 QRadioTuner::duration() const
{
    return qvariant_cast<qint64>(property("duration"));
}

void QRadioTuner::setDuration(qint64 duration)
{
    changePropertyValue("duration", duration);
}

int QRadioTuner::volume() const
{
    return qvariant_cast<int>(property("volume"));
}

void QRadioTuner::setVolume(int volume)
{
    changePropertyValue("volume", volume);
}

bool QRadioTuner::isMuted() const
{
    return property("muted").toBool();
}

void QRadioTuner::setMuted(bool muted)
{
    changePropertyValue("muted", muted);
}

