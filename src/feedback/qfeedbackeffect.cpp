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

#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>

QTM_BEGIN_NAMESPACE

QFeedbackEffect::QFeedbackEffect()
{
}

QFeedbackEffect::~QFeedbackEffect()
{
}

void QFeedbackEffect::setWindow(const QWidget *w)
{
    m_win = w;
}

const QWidget *QFeedbackEffect::window() const
{
    return m_win;
}

//the default timeout is 250 (like for the animations
QContinuousEffect::QContinuousEffect() : m_timeout(250), m_intensity(1), m_effect(ContinuousNone)
{
}

QContinuousEffect::~QContinuousEffect()
{
}

EffectType QContinuousEffect::effectType() const
{
    return ContinuousFeedback;
}

void QContinuousEffect::setTimeout(int msecs)
{
    m_timeout = msecs;
}

int QContinuousEffect::timeout() const
{
    return m_timeout;
}

//the intensity. Value should be between 0 and 1
void QContinuousEffect::setIntensity(qreal intensity)
{
    m_intensity = intensity;
}

qreal QContinuousEffect::intensity() const
{
    return m_intensity;
}

void QContinuousEffect::setContinuousEffect(ContinuousEffect effect)
{
    m_effect = effect;
}

ContinuousEffect QContinuousEffect::continuousEffect() const
{
    return m_effect;
}

QInstantEffect::QInstantEffect(InstantEffect effect) : m_effect(effect)
{
}

QInstantEffect::~QInstantEffect()
{
}

EffectType QInstantEffect::effectType() const
{
    return InstantFeedback;
}

void QInstantEffect::setInstantEffect(InstantEffect effect)
{
    m_effect = effect;
}

InstantEffect QInstantEffect::instantEffect() const
{
    return m_effect;
}


QHitAreaEffect::QHitAreaEffect() : m_hitAreaType(HitAreaMouseButtonPress)
{
}

QHitAreaEffect::~QHitAreaEffect()
{
}

void QHitAreaEffect::setRect(const QRect &rect)
{
    m_rect = rect;
}

void QHitAreaEffect::setRect(const QWidget* widget)
{
    // null rectangle
    m_rect = QRect();

    if (!widget)
        return;

    QWidget *tlw = widget->window();
    QPoint topLeft = widget->mapTo(tlw, QPoint(0, 0));
    m_rect = QRect(topLeft, widget->size());

    //TODO: Shouldn't we call setWindow here?
}

void QHitAreaEffect::setRect(const QGraphicsItem* graphicsItem, const QGraphicsView* graphicsView)
{
    // null rectangle
    m_rect = QRect();

    if (!graphicsItem || !graphicsView || graphicsItem->scene() != graphicsView->scene())
        return;

    // from scene to graphics view coordinates
    m_rect = graphicsView->mapFromScene(
            graphicsItem->mapToScene(graphicsItem->boundingRect()).boundingRect()
            ).boundingRect();

    // from graphics view to window coordinates
    QPoint topLeft = graphicsView->mapTo(graphicsView->window(), m_rect.topLeft());
    m_rect = QRect(topLeft, m_rect.size());

    //TODO: Shouldn't we call setWindow here?
}

void QHitAreaEffect::setHitAreaType(HitAreaType hitAreaType)
{
    m_hitAreaType = hitAreaType;
}

HitAreaType QHitAreaEffect::hitAreaType() const
{
    return m_hitAreaType;
}

QRect QHitAreaEffect::rect() const
{
    return m_rect;
}


QTacticonEffect::QTacticonEffect() : m_effect(TacticonNone)
{
}

QTacticonEffect::~QTacticonEffect()
{
}

EffectType QTacticonEffect::effectType() const
{
    return TacticonFeedback;
}

void QTacticonEffect::setTacticonEffect(TacticonEffect effect)
{
    m_effect = effect;
}

TacticonEffect QTacticonEffect::tacticonEffect() const
{
    return m_effect;
}


QTM_END_NAMESPACE
