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

#include <QtCore/QDebug>
#include <QtGui/QWidget>

#include "qfeedbackeffect.h"
#include "qfeedbackeffect_p.h"

#include <hwrmvibra.h>

#include <touchfeedback.h>

QTM_BEGIN_NAMESPACE

#ifdef ADVANCED_TACTILE_SUPPORT
static TTouchContinuousFeedback convertToSymbian(ContinuousEffect effect)
{
    TTouchContinuousFeedback continuousFeedbackSymbian = ETouchContinuousSmooth;
    switch (effect) {
        case ContinuousSmooth:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        case ContinuousSlider:
            continuousFeedbackSymbian = ETouchContinuousSlider;
            break;
        case ContinuousPopup:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        case ContinuousInput:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        case ContinuousPinch:
            continuousFeedbackSymbian = ETouchContinuousSmooth;
            break;
        default:
            break;
    }
    return continuousFeedbackSymbian;
}

#endif

static TTouchLogicalFeedback convertToSymbian(InstantEffect effect)
{
    TTouchLogicalFeedback instantFeedbackSymbian = ETouchFeedbackBasic;
    switch (effect) {
    case InstantNone:
        instantFeedbackSymbian = ETouchFeedbackNone;
        break;
    case InstantBasic:
        instantFeedbackSymbian = ETouchFeedbackBasic;
        break;
    case InstantSensitive:
        instantFeedbackSymbian = ETouchFeedbackSensitive;
        break;
#ifdef ADVANCED_TACTILE_SUPPORT
    case InstantBasicButton:
        instantFeedbackSymbian = ETouchFeedbackBasicButton;
        break;
    case InstantSensitiveButton:
        instantFeedbackSymbian = ETouchFeedbackSensitiveButton;
        break;
    case InstantBounceEffect:
        instantFeedbackSymbian = ETouchFeedbackBoundaryList;
        break;
    case InstantPopupOpen:
        instantFeedbackSymbian = ETouchFeedbackIncreasingPopUp;
        break;
    case InstantPopupClose:
        instantFeedbackSymbian = ETouchFeedbackDecreasingPopUp;
        break;
    case InstantBasicSlider:
        instantFeedbackSymbian = ETouchFeedbackSlider;
        break;
    case InstantSensitiveSlider:
        instantFeedbackSymbian = ETouchFeedbackSlider;
        break;
    case InstantCheckbox:
        instantFeedbackSymbian = ETouchFeedbackCheckbox;
        break;
#endif
    default:
        break;
    }
    return instantFeedbackSymbian;
}

static CCoeControl *convertToSymbian(const QT_PREPEND_NAMESPACE(QWidget) *w)
{
    return w ? w->winId() : 0;
}

struct QVibraEffectPrivate : public QFeedbackEffectPrivate
{
    QVibraEffectPrivate() : vibra(0)
    {
    }

    ~QVibraEffectPrivate()
    {
        delete vibra;
    }

    CHWRMVibra *vibra;
};

QVibraEffect::QVibraEffect() : QFeedbackEffect(*new QVibraEffectPrivate)
{
}

void QVibraEffect::play()
{
    Q_D(QVibraEffect);
    if (!d->vibra)
        d->vibra;// = CHWRMVibra::newL();

    d->vibra->StartVibraL(d->duration, qRound(100 * d->intensity));
}


//todo: make it thread-safe
static MTouchFeedback *feedback()
{
    MTouchFeedback *feedback = MTouchFeedback::Instance();
    return feedback;
}

QTouchEffect::QTouchEffect() : QFeedbackEffect(*new QTouchEffectPrivate)
{
}

void QTouchEffect::play()
{
    MTouchFeedback *fb = feedback();
    if (!fb)
        return;

#ifdef ADVANCED_TACTILE_SUPPORT
    Q_D(QTouchEffect);
    fb->StartFeedback(convertToSymbian(d->widget), convertToSymbian(d->continuousEffect),
                      0, qRound(100 * d->intensity),
                      1000 * d->duration);
#else
    //if there is no advanced feedback, we just do the normal one and return -1
    fb->InstantFeedback(ETouchFeedbackSensitive);
#endif
}


void QTouchEffect::play(InstantEffect effect)
{
    if (MTouchFeedback *fb = feedback())
        fb->InstantFeedback(convertToSymbian(effect));
}

QTM_END_NAMESPACE

