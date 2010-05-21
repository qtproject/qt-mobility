/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qfeedbackeffect.h"
#include "qfeedbackeffect_p.h"

QTM_BEGIN_NAMESPACE


/*!
    \class QFeedbackEffect
    \brief The QFeedback allows to play a tactile feedback on a device.

    It is possible to set the duration, intenisy and envelope of the effect.
    It is a subclass of QAbstractAnimation which makes it inherit properties from it like
    the possibility to loop or be integrated in an animation group. It can also be started
    stopped or paused.

    You can set the duration to INFINITE. It is then up to the program to stop the effect.

    A feedback effect always works on a feedback device.

    it can report errors through the error signal.

    \sa QAbstractAnimation, QFeedbackDevice
*/

QFeedbackEffect::QFeedbackEffect(QObject *parent) : QAbstractAnimation(*new QFeedbackEffectPrivate, parent)
{
}


QFeedbackEffect::~QFeedbackEffect()
{
}

/*!
    \property QFeedbackEffect::duration
    \brief the expected duration of the effect.

    This property defines the duration of the feedback effect.
*/
int QFeedbackEffect::duration() const
{
    return d_func()->duration;
}

/*!
    \property QFeedbackEffect::intensity
    \brief the intensity of the effect.

    This property defines the intensity of the feedback effect.
    The value can be between 0. and 1.
*/
qreal QFeedbackEffect::intensity() const
{
    return d_func()->intensity;
}

/*!
    \property QFeedbackEffect::attackTime
    \brief the duration of the fade-in effect.

    This property defines the duration of the fade-in effect in msecs.
*/
int QFeedbackEffect::attackTime() const
{
    return d_func()->attackTime;
}

/*!
    \property QFeedbackEffect::attackIntensity
    \brief the initial intensity of the effect.

    This property defines the initial intensity of the effect, before it fades in.
    It is usually lower than intensity.
*/
qreal QFeedbackEffect::attackIntensity() const
{
    return d_func()->attackIntensity;
}

/*!
    \property QFeedbackEffect::fadeTime
    \brief the duration of the fade-out effect.

    This property defines the duration of the fade-out effect in msecs.
*/
int QFeedbackEffect::fadeTime() const
{
    return d_func()->fadeTime;
}

/*!
    \property QFeedbackEffect::fadeIntensity
    \brief the final intensity of the effect.

    This property defines the final intensity of the effect, after it fades out.
    It is usually lower than intensity.
*/
qreal QFeedbackEffect::fadeIntensity() const
{
    return d_func()->fadeIntensity;
}

/*!
    \property QFeedbackEffect::device
    \brief the device on which the effect operates.

    This property defines the device on which the effect operates.
*/
void QFeedbackEffect::setDevice(const QFeedbackDevice &device)
{
    if (state() != Stopped) {
        qWarning("QFeedbackEffect::setDevice: The effect is not stopped");
        return;
    }

    d_func()->device = device;
}

QFeedbackDevice QFeedbackEffect::device() const
{
    return d_func()->device;
}

/*!
    \property QFeedbackEffect::period
    \brief set the period for the effect.

    It has a default value of -1, which mean that it is not a periodic effect.
    Note: not all devices can support periodic effects
*/

void QFeedbackEffect::setPeriod(int msecs)
{
    if (state() != Stopped) {
        qWarning("QFeedbackEffect::setPeriod: the period can only  be changed if the effect is stopped");
        return;
    }
    d_func()->period = msecs;
}

int QFeedbackEffect::period() const
{
    return d_func()->period;
}


/*!
    \fn void QFeedbackDevice::play(InstantEffect effect)

    plays instant feedback.

    That feedback is defined by the theme of the system.
*/

#include "moc_qfeedbackeffect.cpp"

QTM_END_NAMESPACE
