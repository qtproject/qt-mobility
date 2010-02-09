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

#include "qfeedbackcontroller.h"

#ifdef ADVANCED_TACTILE_SUPPORT
#include <tacticon.h>
#endif
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

static TTacticonType convertToSymbian(TacticonEffect effect)
{
    TTacticonType type = ENoTacticon;
    switch (effect) {
    case TacticonPositive:
        type = EPositiveTacticon;
        break;
    case TacticonNeutral:
        type = ENeutralTacticon;
        break;
    case TacticonNegative:
        type = ENegativeTacticon;
        break;
    default:
        break;
    }
    return type;
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

static TTouchEventType convertToSymbian(HitAreaType hitAreaType)
{
    TTouchEventType touchEventType = ETouchEventStylusDown;
    if ( hitAreaType == HitAreaMouseButtonPress ) {
            touchEventType = ETouchEventStylusDown;
    } else if (hitAreaType == HitAreaMouseButtonRelease ) {
        // ETouchEventStylusUp not yet supported
        touchEventType = ETouchEventStylusDown;
    }
    return touchEventType;
}

static TRect convertToSymbian(const QRect& rect)
{
    return TRect(rect.left(), rect.top(), rect.right(), rect.bottom());
}


class QFeedbackControllerPrivate
{
public:
    QFeedbackControllerPrivate() : currentId(0)
    {
#ifdef ADVANCED_TACTILE_SUPPORT
        iTacticonReady = (iTacticon.Connect() == 0);
#endif
        iFeedback = MTouchFeedback::Instance();
    }

    ~QFeedbackControllerPrivate()
    {
#ifdef ADVANCED_TACTILE_SUPPORT
        iTacticon.Close();
#endif
        MTouchFeedback::DestroyInstance();
    }

    int getNewIdentifier()
    {
        return currentId++;
    }

    void cancelContinuousFeedbacks()
    {
#ifdef ADVANCED_TACTILE_SUPPORT
        // stop ongoing continuous feedbacks
        if (!iFeedback) {
            for(QHash<int, QContinuousEffect>::const_iterator it = ongoingContinuousFeedbacks.begin();
                    it != ongoingContinuousFeedbacks.end(); ++it) {
                iFeedback->StopFeedback(it.value().window()->winId());
            }
        }
#endif
        ongoingContinuousFeedbacks.clear();
    }

    void removeHitAreas()
    {
        if (iFeedback) {
            for(QHash<int, QHitAreaEffect>::const_iterator it = registeredHitAreas.begin(); it != registeredHitAreas.end(); ++it) {
                iFeedback->RemoveFeedbackArea(it.value().window()->winId(), it.key());
            }
        }
        registeredHitAreas.clear();
    }


#ifdef ADVANCED_TACTILE_SUPPORT
    //tacticon and continuous feedback is part of advanced tactile support
    RTacticon iTacticon;
    bool iTacticonReady;
#endif
    QHash<int, QContinuousEffect> ongoingContinuousFeedbacks;
    QHash<int, QHitAreaEffect> registeredHitAreas;
    MTouchFeedback *iFeedback;
    int currentId;
};



QFeedbackController::QFeedbackController(QObject *parent) :
    QObject(parent), d(new QFeedbackControllerPrivate)
{
}

QFeedbackController::~QFeedbackController()
{
    delete d;
}


void QFeedbackController::playFeedback(const QInstantEffect &feedback)
{
    if (d->iFeedback) {
        CCoeControl* control = feedback.window() ? feedback.window()->winId() : 0;
        if (control) {
            d->iFeedback->InstantFeedback(control, convertToSymbian(feedback.instantEffect()));
        } else {
            d->iFeedback->InstantFeedback(convertToSymbian(feedback.instantEffect()));
        }
    }
}

void QFeedbackController::playFeedback(const QTacticonEffect &feedback)
{
#ifdef ADVANCED_TACTILE_SUPPORT
        if (d->iTacticonReady) {
            d->iTacticon.PlayTacticon(convertToSymbian(feedback.tacticonEffect()));
        }
#else
        Q_UNUSED(feedback);
        if (d->iFeedback) {
            d->iFeedback->InstantFeedback(ETouchFeedbackBasic);
        }
#endif
}

int QFeedbackController::startContinuousFeedback(const QContinuousEffect &feedback)
{
    int identifier = -1;

    if (feedback.continuousEffect() == ContinuousNone || d->iFeedback == 0)
        return identifier; //nothing to do here

#ifdef ADVANCED_TACTILE_SUPPORT
    CCoeControl* control = feedback.window() ? feedback.window()->winId() : 0;
    if (control) {
        identifier = d->getNewIdentifier();
        d->iFeedback->StartFeedback(control, convertToSymbian(feedback.continuousEffect()),
                                    0, qRound(100 * feedback.intensity()),
                                   1000 * feedback.timeout());
            d->ongoingContinuousFeedbacks[identifier] = feedback;
    }
#else
    //if there is no advanced feedback, we just do the normal one and return -1
    d->iFeedback->InstantFeedback(ETouchFeedbackSensitive);
#endif
    return identifier;
}

void QFeedbackController::updateContinuousFeedback(int identifier, const QContinuousEffect &feedback)
{
    if (!d->iFeedback || !d->ongoingContinuousFeedbacks.contains(identifier))
        return;

#ifdef ADVANCED_TACTILE_SUPPORT
    CCoeControl* control = feedback.window() ? feedback.window()->winId() : 0;
    CCoeControl* storedControl = d->ongoingContinuousFeedbacks[identifier].window()->winId();
    if (storedControl != control)
        return;

    if (feedback.continuousEffect() != ContinuousNone) {
        d->iFeedback->StartFeedback( storedControl,
                                    convertToSymbian(feedback.continuousEffect()),
                                    0, 100 * feedback.intensity(),
                                    1000 * feedback.timeout());
    } else {
        d->iFeedback->StopFeedback(storedControl);
    }

#else
    d->iFeedback->InstantFeedback(ETouchFeedbackSensitive);
    Q_UNUSED(identifier);
    Q_UNUSED(feedback);
#endif
}

void QFeedbackController::cancelContinuousFeedback(int identifier)
{
    if (!d->iFeedback || !d->ongoingContinuousFeedbacks.contains(identifier))
        return;

#ifdef ADVANCED_TACTILE_SUPPORT
    d->iFeedback->StopFeedback(d->ongoingContinuousFeedbacks[identifier].window()->winId());
#endif
    d->ongoingContinuousFeedbacks.remove(identifier);
}

void QFeedbackController::cancelContinuousFeedbacks()
{
    d->cancelContinuousFeedbacks();
}

int QFeedbackController::insertHitArea(const QHitAreaEffect &feedback)
{
    int identifier = -1;
    if (!d->iFeedback)
        return identifier;

    CCoeControl* control = feedback.window() ? feedback.window()->winId() : 0;
    if (!control)
        return identifier;

    identifier = d->getNewIdentifier();
    TTouchLogicalFeedback logicalFeedback = convertToSymbian(feedback.instantEffect());
#ifdef ADVANCED_TACTILE_SUPPORT
        // current hit area implementation below does not yet support new logical feedbacks
        logicalFeedback = qMax(ETouchFeedbackSensitive, logicalFeedback);
#endif

        bool error = d->iFeedback->SetFeedbackArea(control, identifier,
                         convertToSymbian(feedback.rect()),
                         logicalFeedback,
                         convertToSymbian(feedback.hitAreaType())) != KErrNone;
        if (error)
            identifier = -1;
        else
            d->registeredHitAreas.insert(identifier, feedback);
    return identifier;
}

void QFeedbackController::updateHitArea(int identifier, const QHitAreaEffect &feedback)
{
    if (!d->iFeedback || !d->registeredHitAreas.contains(identifier))
        return;

    CCoeControl* storedControl = d->registeredHitAreas[identifier].window()->winId();

    CCoeControl* control = feedback.window() ? feedback.window()->winId() : 0;
    if (storedControl != control)
        return;
    d->iFeedback->ChangeFeedbackArea(storedControl, identifier,
                                     convertToSymbian(feedback.rect()));
}

void QFeedbackController::removeHitArea(int identifier)
{
    if (!d->iFeedback || !d->registeredHitAreas.contains(identifier))
        return;
    CCoeControl* storedControl = d->registeredHitAreas[identifier].window()->winId();
    d->iFeedback->RemoveFeedbackArea(storedControl, identifier);
    d->registeredHitAreas.remove(identifier);
}

void QFeedbackController::removeHitAreas()
{
    d->removeHitAreas();
}

QTM_END_NAMESPACE

