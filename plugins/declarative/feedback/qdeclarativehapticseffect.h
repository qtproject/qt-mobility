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
#ifndef QDECLARATIVEHAPTICSEFFECT_H
#define QDECLARATIVEHAPTICSEFFECT_H

#include <QtDeclarative/qdeclarative.h>
#include "qdeclarativefeedbackeffect.h"
#include "qdeclarativefeedbackactuator.h"

QTM_USE_NAMESPACE

class QDeclarativeHapticsEffect : public QDeclarativeFeedbackEffect
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeFeedbackActuator> availableActuators READ availableActuators)
    Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
    Q_PROPERTY(int attackTime READ attackTime WRITE setAttackTime NOTIFY attackTimeChanged)
    Q_PROPERTY(qreal attackIntensity READ attackIntensity WRITE setAttackIntensity NOTIFY attackIntensityChanged)
    Q_PROPERTY(int fadeTime READ fadeTime WRITE setFadeTime NOTIFY fadeTimeChanged)
    Q_PROPERTY(qreal fadeIntensity READ fadeIntensity WRITE setFadeIntensity NOTIFY fadeIntensityChanged)
    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(QDeclarativeFeedbackActuator* actuator READ actuator WRITE setActuator NOTIFY actuatorChanged)

public:

    explicit QDeclarativeHapticsEffect(QObject *parent = 0) : QDeclarativeFeedbackEffect(parent), actuator_(0)
    {
        d = new QFeedbackHapticsEffect(this);
        setFeedbackEffect(d);

        QFeedbackActuator* fa = d->actuator();

        QList<QFeedbackActuator*> actuators = QFeedbackActuator::actuators();
        foreach (QFeedbackActuator* actuator, actuators) {
            QDeclarativeFeedbackActuator* dfa;
            dfa = new QDeclarativeFeedbackActuator(this, actuator);
            if (fa && *fa == *actuator) {
                actuator_ = dfa;
            }
            actuators_.push_back(dfa);
        }
    }

    void setDuration(int msecs)
    {
        if (msecs != d->duration()) {
            d->setDuration(msecs);
            emit durationChanged();
        }
    }

    int duration() const
    {
        return d->duration();
    }

    void setIntensity(qreal intensity)
    {
        if (qFuzzyCompare(intensity, d->intensity())) {
            d->setIntensity(intensity);
            emit intensityChanged();
        }
    }

    qreal intensity() const
    {
        return d->intensity();
    }

    //the envelope
    void setAttackTime(int msecs)
    {
        if (msecs != d->attackTime()) {
            d->setAttackTime(msecs);
            emit attackTimeChanged();
        }
    }

    int attackTime() const
    {
        return d->attackTime();
    }

    void setAttackIntensity(qreal intensity)
    {
        if (qFuzzyCompare(intensity, d->attackIntensity())) {
            d->setAttackIntensity(intensity);
            emit intensityChanged();
        }
    }

    qreal attackIntensity() const
    {
        return d->attackIntensity();
    }

    void setFadeTime(int msecs)
    {
        if (msecs != d->fadeTime()) {
            d->setFadeTime(msecs);
            emit fadeTimeChanged();
        }
    }

    int fadeTime() const
    {
        return d->fadeTime();
    }

    void setFadeIntensity(qreal intensity)
    {
        if (qFuzzyCompare(intensity, d->fadeIntensity())) {
            d->setFadeIntensity(intensity);
            emit fadeIntensityChanged();
        }
    }

    qreal fadeIntensity() const
    {
        return d->fadeIntensity();
    }

    void setPeriod(int msecs)
    {
        if (msecs != d->period()) {
            d->setPeriod(msecs);
            emit periodChanged();
        }
    }

    int period() const
    {
        return d->period();
    }

    void setActuator(QDeclarativeFeedbackActuator *actuator)
    {
        if (actuator != actuator_) {
            if (!actuator
             || !actuator_
             || !(*(actuator->feedbackActuator()) == *(actuator_->feedbackActuator()))) {
                actuator_ = actuator;
                d->setActuator(actuator_->feedbackActuator());
                emit actuatorChanged();
            }
        }
    }

    QDeclarativeFeedbackActuator* actuator() const
    {
        return actuator_;
    }

    QDeclarativeListProperty<QDeclarativeFeedbackActuator> availableActuators() {
        return QDeclarativeListProperty<QDeclarativeFeedbackActuator>(this,
                                                                      0,
                                                                      0 /*appending actuators are not allowed*/,
                                                                      actuator_count,
                                                                      actuator_at,
                                                                      0 /*removing actuators are not allowed*/);
    }

    static int actuator_count(QDeclarativeListProperty<QDeclarativeFeedbackActuator> *prop)
    {
        return static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.size();
    }
    static QDeclarativeFeedbackActuator *actuator_at(QDeclarativeListProperty<QDeclarativeFeedbackActuator> *prop, int index)
    {
        return static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.at(index);
    }

signals:
    void intensityChanged();
    void attackTimeChanged();
    void attackIntensityChanged();
    void fadeTimeChanged();
    void fadeIntensityChanged();
    void periodChanged();
    void actuatorChanged();
public slots:
private:
    QFeedbackHapticsEffect* d;
    QList<QDeclarativeFeedbackActuator*> actuators_;
    QDeclarativeFeedbackActuator* actuator_;
};

QML_DECLARE_TYPE(QDeclarativeHapticsEffect);

#endif // QDECLARATIVEHAPTICSEFFECT_H
